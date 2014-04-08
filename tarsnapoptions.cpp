// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "tarsnapoptions.h"

#include "firsttimeconfiguration.h"

#include <QGridLayout>
#include <QHBoxLayout>

#include <QLabel>
#include <QPushButton>

#include <QFileDialog>


/*! \class TarsnapOptions tarsnapoptions.h

    This class allows editing the configuration variables, only two at
    present.
*/


/*! Constructs a rather optional dialog. */

TarsnapOptions::TarsnapOptions( QSettings * s )
    : QDialog( 0 ),
      cacheDir( new QLineEdit( this ) ),
      keyFile( new QLineEdit( this ) ),
      settings( s )
{
    setModal( true );

    QGridLayout * l = new QGridLayout( this );

    QPushButton * b = new QPushButton( tr( "..." ), this );
    connect( b, SIGNAL(clicked()),
	     this, SLOT(browseForCache()) );
    l->addWidget( new QLabel( tr( "Cache file" ), this ),
		  0, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( cacheDir,
		  0, 1, 1, 1 );
    l->addWidget( b,
		  0, 2, 1, 1 );

    l->addWidget( new QLabel( tr( "Key file" ), this ),
		  1, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( keyFile,
		  1, 1, 1, 1 );

    QBoxLayout * keyFileLayout = new QHBoxLayout();
    l->addLayout( keyFileLayout,
		  1, 1, 1, 1 );
    keyFileLayout->addWidget( keyFile, 2 );
    b = new QPushButton( tr( "Make &new key" ), this );
    connect( b, SIGNAL(clicked()),
	     this, SLOT(createNewKey()) );
    keyFileLayout->addWidget( b );

    b = new QPushButton( tr( "..." ), this );
    connect( b, SIGNAL(clicked()),
	     this, SLOT(browseForKey()) );
    l->addWidget( b,
		  1, 2, 1, 1 );

    QBoxLayout * actionButtons = new QHBoxLayout();
    l->addLayout( actionButtons,
		  2, 1, 1, 1, Qt::AlignRight );

    b = new QPushButton( tr( "Cancel" ), this );
    connect( b, SIGNAL(clicked()),
	     this, SLOT(reject()) );
    actionButtons->addWidget( b );

    b = new QPushButton( tr( "OK" ), this );
    b->setDefault( true );
    connect( b, SIGNAL(clicked()),
	     this, SLOT(accept()) );
    actionButtons->addWidget( b );

    cacheDir->setText( settings->value( "tarsnap/cachedir" ).toString() );
    keyFile->setText( settings->value( "tarsnap/keyfile" ).toString() );
}


/*! Destroys the object and frees any allocated resources. Needed only
    to please the C++ linker, bless its long-suffering soul.
*/

TarsnapOptions::~TarsnapOptions()
{
}


/*! Creates a new key and stores it in the file system. */

void TarsnapOptions::createNewKey()
{
    FirstTimeConfiguration f( this, keyFile->text() );
    f.resize( width() * 7 / 8, f.sizeHint().height() );
    f.exec();
    keyFile->setText( f.filename() );
}


/*! Opens a dialog to browse for Tarsnap's key.
*/

void TarsnapOptions::browseForKey()
{
    keyFile->setText( QFileDialog::getOpenFileName( this ) );
}


/*! Opens a dialog to browse for a location where Tarsnap can store
    its cache.
*/

void TarsnapOptions::browseForCache()
{
    cacheDir->setText( QFileDialog::getExistingDirectory( this ) );
}


/*! This reimplementation of QDialog::accept() makes sure to write the
    settings and create the directory where the cache is to be written.
*/

void TarsnapOptions::accept()
{
    settings->setValue( "tarsnap/keyfile", keyFile->text() );
    settings->setValue( "tarsnap/cachedir", cacheDir->text() );
    QDialog::accept();
}
