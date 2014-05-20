// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "actionpage.h"

#include <QGridLayout>

#include <QGridLayout>

#include <QFileDialog>

#include <QFile>
#include <QDir>

#include <QTextStream>
#include <QDateTime>

#include <QMessageBox>

#include <iostream>



/*! \class ActionPage actionpage.h
  
    The ActionPage is the other mandatory page. This is what performs
    a backup, or writes a script.
*/

/*! Constructs a 

*/

ActionPage::ActionPage( BackupWizard * parent, IncludePage * details )
    : QWizardPage( parent ),
      archived( 0 ),
      unusualOutput( new QTextEdit( this ) ),
      lastArchived( new QLineEdit( this ) ),
      writeScript( new QPushButton( tr( "&Write Backup Script" ), this ) ),
      backup( new QPushButton( tr( "&Perform backup" ), this ) ),
      tarsnap( nullptr ),
      w( parent ),
      inclusions( details )
{
    setTitle( tr( "Back up" ) );
    setSubTitle( tr( "Perform backup, or write a script for later" ) );

    connect( backup, SIGNAL(clicked()),
	     this, SLOT(performBackup()) );
    connect( writeScript, SIGNAL(clicked()),
	     this, SLOT(saveScript()) );

    QGridLayout * l = new QGridLayout( this );

    l->addWidget( writeScript,
		  0, 0 );
    l->addWidget( backup,
		  0, 1 );

    l->addWidget( lastArchived,
		  1, 0, 1, 3 );
    l->addWidget( unusualOutput,
		  2, 0, 1, 3 );

    lastArchived->hide();
    unusualOutput->hide();

    l->setColumnStretch( 2, 2 );
}


/*! Runs one entire backup using the current configuration, displaying
    progress information along the way.
*/

void ActionPage::performBackup()
{
    QString key = field( "keyFile" ).toString();
    QString cache = field( "cacheDirectory" ).toString();

    QString backupName( field( "host" ).toString() );
    QDateTime now = QDateTime::currentDateTime();
    backupName.append( now.toString( "-yyyy-MM-dd-hh-mm" ) );

    if ( tarsnap )
	delete tarsnap;
    tarsnap = new QProcess( this );
    connect( tarsnap, SIGNAL(readyReadStandardOutput()),
	     this, SLOT(read()) );
    connect( tarsnap, SIGNAL(finished(int, QProcess::ExitStatus)),
	     this, SLOT(finish(int, QProcess::ExitStatus)) );
    tarsnap->setWorkingDirectory( field( "baseDirectory").toString() );
    tarsnap->setProcessChannelMode( QProcess::MergedChannels );
    QStringList cli = commandLine( backupName );
    tarsnap->start( "/usr/local/bin/tarsnap", cli );
    lastArchived->show();
    lastArchived->setText( "" );
    unusualOutput->setText( "" );
}


/*! Writes a script to carry out the currently configured backup,
    asking the user for a suitable name.
*/

void ActionPage::saveScript()
{
    QString name = QFileDialog::getSaveFileName( this );
    if ( name.isNull() )
	return;
    QFile script( name );
    if ( !script.open( QIODevice::WriteOnly |
		       QIODevice::Text |
		       QIODevice::Truncate) )
	return;

    QString key = field( "keyFile" ).toString();
    QString cache = field( "cacheDirectory" ).toString();

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
	<< "# Feel free to delete everything above this point. And to change\n"
	<< "# this filename pattern to something you like. It is probably a\n"
	<< "# good idea to remove the -v option from the tarsnap invocation\n"
	<< "# once you have seen it work correctly.\n"
	<< "\n"
	<< "cd " << shQuoted( field( "baseDirectory" ).toString() ) << "\n"
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

QStringList ActionPage::commandLine( const QString & filename ) const
{
    QStringList result;

    result << "-c"
	   << "-f"
	   << filename
	   << "-v"
	   << "--keyfile"
	   << field( "keyFile" ).toString()
	   << "--cachedir"
	   << field( "cacheDirectory" ).toString();

    if ( !field( "crossMountPoints" ).toBool() )
	result << "--one-file-system";

    if( field( "something" ).toBool() )
	result << inclusions->selectedDirectories();
    else
	result << ".";

    return result;
}


/*! Returns a sh-quoted version of \a s.

    This is rather conservative; it quotes with ' at the slightest
    hint of trouble.
*/

QString ActionPage::shQuoted( const QString & s ) const
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


/*! This slot reads tarsnap's output and puts it onscreen. Only the
    last filename is displayed because that's routine and not terribly
    interesting, but all the odd and unusual output is shown to the
    user.
*/

void ActionPage::read()
{
    partialLine.append( tarsnap->readAll() );
    int bol = 0;
    bool progress = true;
    QString filename;
    QString other;
    while ( progress ) {
	progress = false;
	int i = bol;
	while ( i < partialLine.size() && partialLine[i] != '\n' )
	    i++;
	if ( i < partialLine.size() ) {
	    progress = true;
	    if ( i - bol >= 3 &&
		 partialLine[bol] == 'a' &&
		 partialLine[bol+1] == ' ' ) {
		filename = QString::fromUtf8( partialLine.data() + bol + 2,
					      i - bol - 2 );
		archived++;
	    } else {
		other.append( QString::fromUtf8( partialLine.data() + bol,
						 i + 1 - bol ) );
	    }
	    bol = i + 1;
	}
    }
    partialLine = partialLine.mid( bol );
    unusualOutput->append( other );
    if ( !unusualOutput->isVisible() && !other.isEmpty() )
	unusualOutput->show();
    if ( !filename.isEmpty() )
	lastArchived->setText( filename );
}


/*! This slot changes the UI to signal that the backup is done. */

void ActionPage::finish(int code, QProcess::ExitStatus status)
{
    if ( status != QProcess::NormalExit && code != 0 ) {
	QMessageBox::critical( wizard(),
			       tr( "Error running tarsnap" ),
			       tr( "Oddly, tarsnap did not finish "
				   "normally. It seems likely that something "
				   "may have failed. The command line that "
				   "failed was something like "
				   "<code>tarsnap %1</code>" )
			       .arg( commandLine( "hostname" ).join( " " ) ) );
    }
    // this is ungood. a label would be better. where should it go?
    lastArchived->setText( tr( "Done; %1 files archived" ).arg( archived ) );
    delete tarsnap;
    tarsnap = nullptr;
}


/*! Clicking Back aborts any running backup. Is that perfect? Not sure. */

void ActionPage::cleanupPage()
{
    if ( !tarsnap )
	return;
    tarsnap->terminate();
    // we leak tarsnap rather than wait for termination to complete
    tarsnap = nullptr;
}


/*! This reimplementation of validatePage() doesn't actually validate,
    it just prepares for going away.
*/

bool ActionPage::validatePage()
{
    if ( tarsnap && tarsnap->state() == QProcess::Running )
	QMessageBox::critical( this,
			       tr( "A backup is running" ),
			       tr( "The backup may be interrupted when you "
				   "leave this program. Perhaps you should "
				   "write a backup script and run that." ) );
    w->nag();
    return true;
}
