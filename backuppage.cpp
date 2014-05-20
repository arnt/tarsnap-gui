// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "backuppage.h"

// geteuid()
#include <unistd.h>
#include <sys/types.h>

// getpwuid()
#include <sys/types.h>
#include <pwd.h>

#include <QDir>

#include <QGridLayout>

#include <QFileDialog>


/*! \class BackupPage backuppage.h

    The BackupPage is the only mandatory page in this. Well, the only
    mandatory real page. It asks the main question about what to back
    up, including any supplementary questions that have to be
    answered.  Questions that may not be needed are deferred to
    optional pages.
*/

/*! Constructs a defaultish backup page.
*/

BackupPage::BackupPage( BackupWizard * parent )
    : QWizardPage( parent ),
      behaviour( new QButtonGroup( this ) ),
      baseDirectory( new QLineEdit( this ) ),
      crossMountPoints( new QCheckBox( tr( "Cross mount points" ), this ) ),
      everything( new QRadioButton( tr( "Back up &everything" ),
				    this ) ),
      something( new QRadioButton( tr( "Back up only some &subdirectories" ),
				   this ) ),
      cacheDirectory( new QLineEdit( parent->initialCacheDirectory(),
				     this ) ),
      complete( false )
{
    setTitle( tr( "Backup" ) );
    setSubTitle( tr( "Select what to back up" ) );

    QPushButton * browse = new QPushButton( tr( "..." ), this );
    connect( browse, SIGNAL(clicked()),
	     this, SLOT(browseBaseDirectory()) );

    if ( cacheDirectory->text().isEmpty() ) {
	struct passwd * pw = getpwuid(geteuid());
	if ( pw && pw->pw_uid )
	    cacheDirectory->setText( QString::fromUtf8( pw->pw_dir ) +
				     "/.cache/tarsnap" );
	else
	    cacheDirectory->setText( "/var/lib/tarsnap/cache" );
    }

    connect( baseDirectory, SIGNAL(textChanged(const QString &)),
	     this, SLOT(checkDirectories()) );
    connect( cacheDirectory, SIGNAL(textChanged(const QString &)),
	     this, SLOT(checkDirectories()) );
	
    QGridLayout * l = new QGridLayout( this );

    l->addWidget( new QLabel( tr( "Directory" ), this ),
		  0, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( baseDirectory,
		  0, 1, 1, 2 );
    l->addWidget( browse,
		  0, 3, 1, 1 );

    l->addWidget( new QLabel( tr( "Behaviour" ), this ),
		  2, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( everything,
		  2, 1, 1, 1, Qt::AlignLeft );
    l->addWidget( something,
		  3, 1, 1, 1, Qt::AlignLeft );

    l->addWidget( new QLabel( tr( "Options" ), this ),
		  5, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( crossMountPoints,
		  5, 1, 1, 2, Qt::AlignLeft );

    l->addWidget( new QLabel( tr( "<html>Tarsnap caches information about what "
				  "it has uploaded in a local directory."
				  "</html" ),
			      this ),
		  7, 1, 1, 2, Qt::AlignLeft );
    l->addWidget( new QLabel( tr( "Cache" ), this ),
		  8, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( cacheDirectory,
		  8, 1, 1, 2 );

    l->setColumnStretch( 1, 2 );
    l->setColumnStretch( 2, 2 );

    l->setRowMinimumHeight( 1, 12 );
    l->setRowMinimumHeight( 4, 12 );
    l->setRowMinimumHeight( 6, 12 );

    behaviour->addButton( something );
    behaviour->addButton( everything );

    everything->setChecked( true );

    crossMountPoints->setChecked( true );

    baseDirectory->setText( "/" );

    registerField( "baseDirectory", baseDirectory );
    registerField( "cacheDirectory", cacheDirectory );
    registerField( "crossMountPoints", crossMountPoints );
    registerField( "something", something );
}


/*! The user is allowed to proceed when the base directory exists. No
    less and no more.
*/

bool BackupPage::isComplete() const
{
    if ( cacheDirectory->text().isEmpty() )
	return false;

    QString path = baseDirectory->text();
    if ( path.isEmpty() )
	return false;
    QDir candidate( path );
    return candidate.exists();
}


/*! Determines whether the user should be shown to the finetuning page
    or go directly to backup.
*/

int BackupPage::nextId() const
{
    if ( everything->isChecked() )
	return BackupWizard::Action;
    return BackupWizard::Inclusion;
}


/*! Opens a file dialog to find a directory, and reacts to the chosen
    name.
*/

void BackupPage::browseBaseDirectory()
{
    QString name = QFileDialog::getExistingDirectory( this );
    if( !name.isNull() )
	baseDirectory->setText( name );
}


/*! The base directory has to exist in order for Next to be
    enabled.
    
    The cache directory has to... well, not very much. Tarsnap will
    make it, so all we really need at this stage is a filename.
*/

void BackupPage::checkDirectories()
{
    QDir directory( baseDirectory->text() );
    bool was = complete;
    complete = ( !baseDirectory->text().isEmpty() &&
		 directory.exists() &&
		 !cacheDirectory->text().isEmpty() );
    if ( complete != was )
	emit completeChanged();
}


/*! Opens a dialog to browse for a location where Tarsnap can store
    its cache.
*/

void BackupPage::browseForCache()
{
    QString s = QFileDialog::getExistingDirectory( this );
    if ( !s.isNull() )
	cacheDirectory->setText( s );
}
