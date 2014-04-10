// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "backupconfiguration.h"

#include "firsttimeconfiguration.h"
#include "tarsnapoptions.h"
#include "watcher.h"

#include <QLabel>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QFileDialog>

#include <QFile>
#include <QDir>

#include <QTextStream>
#include <QDateTime>

#include <QMessageBox>
#include <QProcess>

#include <iostream>

#include <unistd.h>




/*! \class BackupConfiguration backupconfiguration.h

    The BackupConfiguration class allows the user to configure one
    backup and either store it for later retrieval, carry it out, or
    write a shell script to carry it out.

    The main idea here is that the user tweaks, perhaps performs a dry
    run or two, then carries out a backup. Once that has been done, we
    make it easy to write a script to repeat that exact backup.

    This class does not need to read the script back. The script is
    intended to be modifiable by the user, not rereadable by
    BackupConfiguration.
*/

/*! Constructs a BackupConfiguration with everything set to default
   values.
*/

BackupConfiguration::BackupConfiguration()
    : QWidget( 0 ),
      baseDirectory( new QLineEdit( this ) ),
      browse( new QPushButton( tr( "..." ), this ) ),
      crossMountPoints( new QCheckBox( tr( "Cross mount points" ), this ) ),
      exceptions( new QTreeView( this ) ),
      model( new QFileSystemModel() ),
      settings( new QPushButton( tr( "&Settings" ), this ) ),
      backup( new QPushButton( tr( "&Backup" ), this ) ),
      writeScript( new QPushButton( tr( "&Write Backup Script" ), this ) ),
      settingsStorage( new QSettings( "Tarsnap", "Backup Configuration") )
{
    setWindowTitle( tr( "Configure Tarsnap Backup" ) );
    setupLayout();
    setupActions();
    setupSubstance();
    checkBaseDirectory( baseDirectory->text() );
}


/*! Destroys the object and frees any allocated resources. Notably,
    this has to stop subprocesses.
*/

BackupConfiguration::~BackupConfiguration()
{
    // todo, really
}


/*! Sets up the layout parameters.

    This is part of the constructor, separated out for clarity.
*/

void BackupConfiguration::setupLayout()
{
    QGridLayout * l = new QGridLayout( this );
    // first row: pick base directory
    l->addWidget( new QLabel( tr( "Directory" ), this ),
		  0, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( baseDirectory,
		  0, 1, 1, 1 );
    l->addWidget( browse,
		  0, 2, 1, 1 );
    // second row: selection tuning flags. this will probably need a
    // child layout, we'll see.
    l->addWidget( new QLabel( tr( "Options" ), this ),
		  1, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( crossMountPoints,
		  1, 1, 1, 2, Qt::AlignLeft );
    // third row, occupies several rows for better alignment:
    // exceptions and buttons to add/remove
    l->addWidget( new QLabel( tr( "Exceptions" ), this ),
		  2, 0, 1, 1, Qt::AlignLeft | Qt::AlignTop );
    l->addWidget( exceptions,
		  2, 1, 3, 1 );
    // last row: action buttons
    QBoxLayout * actionButtons = new QHBoxLayout();
    l->addLayout( actionButtons,
		  5, 1, 1, 1 );
    actionButtons->addWidget( settings );
    actionButtons->addSpacing( 12 );
    actionButtons->addStretch( 2 );
    actionButtons->addWidget( backup );
    actionButtons->addWidget( writeScript );

    resize( 800, 800 );
}


/*! Connects all UI elements to the right slots. This is effectively
    part of the constructor, but separated out so the constructor need
    not be so long.
*/

void BackupConfiguration::setupActions()
{
    connect( baseDirectory, SIGNAL(textChanged(const QString &)),
	     this, SLOT(checkBaseDirectory(const QString &)) );
    connect( browse, SIGNAL(clicked()),
	     this, SLOT(browseBaseDirectory()) );

    connect( settings, SIGNAL(clicked()),
	     this, SLOT(configure()) );
    connect( backup, SIGNAL(clicked()),
	     this, SLOT(performBackup()) );
    connect( writeScript, SIGNAL(clicked()),
	     this, SLOT(saveScript()) );
}


/*! Sets up internal state. Again, this is really part of the
    constructor, but it would have been so long.
*/

void BackupConfiguration::setupSubstance()
{
    model->setFilter( QDir::Dirs | QDir::NoSymLinks |
		      QDir::NoDotAndDotDot );
    exceptions->setModel( model );
    exceptions->setIndentation( 42 );
    exceptions->setSortingEnabled( true );
    exceptions->setSelectionMode( QAbstractItemView::MultiSelection );
    exceptions->setColumnHidden( 1, true );
    exceptions->setColumnHidden( 2, true );
    exceptions->setColumnWidth( 0, width() * 2 / 3 );
    baseDirectory->setText( "/" );

    if ( settingsStorage->value( "tarsnap/cachedir" ).isNull() ) {
	settingsStorage->setValue( "tarsnap/cachedir",
				   "/var/lib/tarsnap/cache" );
    }
    if ( settingsStorage->value( "tarsnap/keyfile" ).isNull() ) {
	FirstTimeConfiguration f( 0, "" );
	f.resize( width() * 3 / 4, f.sizeHint().height() );
	f.exec();
	if ( !f.filename().isNull() )
	    settingsStorage->setValue( "tarsnap/keyfile",
				       f.filename() );
    }
}


/*! Verifies that \a name refers to a directory we can back up, and
    makes the buttons available iff so.
*/

void BackupConfiguration::checkBaseDirectory( const QString & name )
{
    QDir directory( name );
    if ( !name.isEmpty() && directory.exists() ) {
	backup->setEnabled( true );
	writeScript->setEnabled( true );
    } else {
	backup->setEnabled( false );
	writeScript->setEnabled( false );
    }
    model->setRootPath( name );
    exceptions->clearSelection();
    exceptions->setRootIndex( model->setRootPath( name ) );
}


/*! Opens a file dialog to find a directory, and reacts to the chosen
    name.
*/

void BackupConfiguration::browseBaseDirectory()
{
    QString name = QFileDialog::getExistingDirectory( this );
    if( !name.isNull() )
	baseDirectory->setText( name );
}


/*! Runs one entire backup using the current configuration, opening a
    separate window with progress information.
*/

void BackupConfiguration::performBackup()
{
    QString key = settingsStorage->value( "tarsnap/keyfile" ).toString();
    QString cache = settingsStorage->value( "tarsnap/cachedir" ).toString();

    QFile keyFile( key );
    if ( !keyFile.exists() ) {
	QMessageBox::critical( this,
			       tr( "Error opening key file" ),
			       tr( "Tarsnap key file does not exist, "
				   "or cannot be opened.\n"
				   "Perhaps root access is needed, or perhaps "
				   "you need to run tarsnap-keygen.\n"
				   "File name: %1" ).arg( key ) );
	return;
    }

    QDir cacheDir( cache );
    if ( !cacheDir.exists() ) {
	QMessageBox::critical( this,
			       tr( "Error opening cache file" ),
			       tr( "Tarsnap cache directory does not exist, "
				   "or cannot be opened.\n"
				   "Perhaps root access is needed, or you may "
				   "need to create or chmod the directory.\n"
				   "File name: %1" ).arg( cache ) );
	return;
    }

    char tmp[64];
    ::gethostname( tmp, 64 );
    tmp[63] = 0;
    QString backupName( tmp );
    QDateTime now = QDateTime::currentDateTime();
    backupName.append( now.toString( "-yyyy-MM-dd-hh-mm" ) );
    QProcess * tarsnap = new QProcess( this ); // a memory leak. how terrible.
    tarsnap->setWorkingDirectory( baseDirectory->text() );
    tarsnap->setProcessChannelMode( QProcess::MergedChannels );
    QStringList cli = commandLine( backupName );
    Watcher watcher( tarsnap, cli.join( " " ), this );
    watcher.resize( width() * 7 / 8, height() * 7 / 8 );
    tarsnap->start( "/usr/local/bin/tarsnap", cli );
    watcher.show();
    watcher.exec();
}


/*! Writes a script to carry out the currently configured backup,
    asking the user for a suitable name.
*/

void BackupConfiguration::saveScript()
{
    QString name = QFileDialog::getSaveFileName( this );
    if ( name.isNull() )
	return;
    QFile script( name );
    if ( !script.open( QIODevice::WriteOnly |
		       QIODevice::Text |
		       QIODevice::Truncate) )
	return;

    QString key = settingsStorage->value( "tarsnap/keyfile" ).toString();
    QString cache = settingsStorage->value( "tarsnap/cachedir" ).toString();

    QTextStream out( &script );
    out << "#!/bin/sh\n"
	<< "\n"
	<< "# Generated at "
	<< QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss" )
	<< " by tarsnap-gui.\n" // might want a version number
	<< "\n"
	<< "if [ ! -f \"" << key << "\" ]; then\n"
	<< "    echo Cannot access " << key << "\n"
	<< "    [ 0 -eq $(id -u) ] || echo Root access may be needed.\n"
	<< "    exit 1\n"
	<< "fi\n"
	<< "\n"
	<< "if [ ! -d \"$(dirname " << cache << ")\" ]; then\n"
	<< "    echo Cannot access " << cache << "\n"
	<< "    echo Please make sure that $(dirname " << cache << ") exists\n"
	<< "    exit 1\n"
	<< "fi\n"
	<< "\n"
	<< "# Feel free to delete everything above this point. And to change\n"
	<< "# this filename pattern to something you like. It is probably a\n"
	<< "# good idea to remove the -v option from the tarsnap invocation\n"
	<< "# once you have seen it work correctly.\n"
	<< "\n"
	<< "cd " << shQuoted( baseDirectory->text() ) << "\n"
	<< "\n"
	<< "FN=$(/bin/hostname)-$(date +%Y-%m-%d-%H-%M)\n"
	<< "\n";

    QStringList l;
    l << "tarsnap";
    l << commandLine( "$FN" );
    auto s = l.begin();
    int col = 0;
    while ( s != l.end() ) {
	QString quoted;
	if ( *s == "$FN" )
	    quoted = *s;
	else
	    quoted = shQuoted( *s );
	if ( col == 0 ) {
	    // it's the start. just do it.
	} else if ( col + quoted.length() > 75 &&
		    quoted.length() < 75 ) {
	    // would wrap and making a new line helps
	    out << " \\\n  ";
	    col = 2;
	} else {
	    // not too wide, or too wide anyway
	    out << " ";
	    col++;
	}

	out << quoted;
	col += quoted.length();
	++s;
    }
    out << "\n";

    script.setPermissions( QFile::ReadOwner |
			   QFile::WriteOwner |
			   QFile::ExeOwner |
			   QFile::ReadGroup |
			   QFile::ExeGroup |
			   QFile::ReadOther |
			   QFile::ExeOther );
}


/*! Returns the tarsnap command line to perform one backup.

*/

QStringList BackupConfiguration::commandLine( const QString & filename ) const
{
    QStringList result;

    result << "-c"
	   << "-f"
	   << filename
	   << "-v"
	   << "--keyfile"
	   << settingsStorage->value( "tarsnap/keyfile" ).toString()
	   << "--cachedir"
	   << settingsStorage->value( "tarsnap/cachedir" ).toString();

    if ( !crossMountPoints->isChecked() )
	result << "--one-file-system";

    QModelIndexList excludedDirectories =
	exceptions->selectionModel()->selectedIndexes();
    QString prefix = baseDirectory->text();
    if ( prefix != "/" )
	prefix += "/";
    auto x = excludedDirectories.begin();
    while ( x != excludedDirectories.end() ) {
	// this doesn't quite work; tarsnap up to 1.0.35 at least
	// offers no (warningless) way to exclude a subdirectory
	// without excluding others of the same name.
	if ( x->column() == 0 )
	    result << "--exclude"
		   << x->data( QFileSystemModel::FilePathRole )
		       .toString()
		       .mid( prefix.length() );
	++x;
    }

    result << ".";

    return result;
}


/*! Returns a sh-quoted version of \a s.

    This is rather conservative; it quotes with ' at the slightest
    hint of trouble.
*/

QString BackupConfiguration::shQuoted( const QString & s ) const
{
    int i = 0;
    while ( i < s.length() &&
	    ( s[i] == '-' || s[i] == '/' || s[i] == '.' ||
	      s[i].isLetterOrNumber() ) )
	i++;
    if ( !s.isEmpty() && i == s.length() )
	return s;

    QString result = "'";
    i = 0;
    while ( i < s.length() ) {
	if ( s[i] == '\'' || s[i] == '\\' || s[i] < ' ' )
	    result += '\\';
	result += s[i];
	i++;
    }
    result += "\'";
    return result;
}


/*! Opens the settings configuration dialog, and afterwards ensures
    that the settings arex somehow acceptable. Specifically, the cache
    file must be in an existing directory and the key file must exist.
*/

void BackupConfiguration::configure()
{
    TarsnapOptions o( settingsStorage );
    o.resize( width() * 3 / 4, o.minimumHeight() );
    o.show();
    o.exec();
}


/*! This reimplementation nags the user about copying the key file
    somewhere safe.
*/

void BackupConfiguration::closeEvent( QCloseEvent * e )
{
    QWidget::closeEvent( e );
    if ( settingsStorage->value( "tarsnap/copied" ).toBool() )
	return;

    QMessageBox * b = new QMessageBox( 0 );
    b->setText(
	tr( "<b>Have you copied %1 to a safe location yet?</b>" )
	.arg( settingsStorage->value( "tarsnap/keyfile" ).toString() ) );
    b->setInformativeText(
	tr( "<html>You may want to copy the file to two USB sticks, "
	    "label them well and store them separately. Or perhaps you "
	    "prefer something else. Do whatever you want, but "
	    "keep %1 safe.<p>"
	    "You <b>will not be able to restore a backup</b> "
	    "without that file. Even the NSA probably cannot "
	    "help you decrypt your backups if you lose that file.</html>" )
	.arg( settingsStorage->value( "tarsnap/keyfile" ).toString() ) );
    // that's not the kind of language favoured by corporate technical
    // writers. it's the kind of language favoured by arnts who have
    // sat too long in an office and need to get laid. I wonder how
    // well tarsnap's audience likes it.
    b->setStandardButtons( QMessageBox::Yes | QMessageBox::No );
    b->setDefaultButton( QMessageBox::No );
    int ret = b->exec();
    if ( ret == QMessageBox::Yes )
	settingsStorage->setValue( "tarsnap/copied", true );
}
