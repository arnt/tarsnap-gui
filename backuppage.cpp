// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "backuppage.h"

#include <QGridLayout>

#include <QDir>

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

    connect( baseDirectory, SIGNAL(textChanged(const QString &)),
	     this, SLOT(checkBaseDirectory(const QString &)) );
    
    QGridLayout * l = new QGridLayout( this );

    l->addWidget( new QLabel( tr( "Directory" ), this ),
		  0, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( baseDirectory,
		  0, 1, 1, 2 );
    l->addWidget( browse,
		  0, 3, 1, 1 );

    l->addWidget( new QLabel( tr( "Behaviour" ), this ),
		  1, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( everything,
		  1, 1, 1, 1, Qt::AlignLeft );
    l->addWidget( something,
		  2, 1, 1, 1, Qt::AlignLeft );

    l->addWidget( new QLabel( tr( "Options" ), this ),
		  3, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( crossMountPoints,
		  3, 1, 1, 2, Qt::AlignLeft );

    l->addWidget( new QLabel( tr( "<html>Tarsnap caches information about what "
				  "it has uploaded in a local directory."
				  "</html" ),
			      this ),
		  4, 1, 1, 2, Qt::AlignLeft );
    l->addWidget( new QLabel( tr( "Cache" ), this ),
		  5, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( cacheDirectory,
		  5, 1, 1, 2 );

    l->setColumnStretch( 1, 2 );
    l->setColumnStretch( 2, 2 );

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
*/

void BackupPage::checkBaseDirectory( const QString & name )
{
    QDir directory( name );
    bool was = complete;
    complete = ( !name.isEmpty() && directory.exists() );
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
