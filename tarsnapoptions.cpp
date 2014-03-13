// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "tarsnapoptions.h"

#include <QGridLayout>
#include <QHBoxLayout>

#include <QLabel>
#include <QPushButton>

/*! \class TarsnapOptions tarsnapoptions.h
  
    This class allows editing the configuration variables, only two at
    present.
*/


/*! Constructs an optional dialog.

*/

TarsnapOptions::TarsnapOptions( QSettings * s )
    : QDialog( 0 ),
      cacheFile( new QLineEdit( this ) ),
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
    l->addWidget( cacheFile,
		  0, 1, 1, 1 );
    l->addWidget( b,
		  0, 2, 1, 1 );

    b = new QPushButton( tr( "..." ), this );
    connect( b, SIGNAL(clicked()),
	     this, SLOT(browseForKey()) );
    l->addWidget( new QLabel( tr( "Key file" ), this ),
		  1, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( keyFile,
		  1, 1, 1, 1 );
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

    cacheFile->setText( settings->value( "tarsnap/cachefile" ).toString() );
    keyFile->setText( settings->value( "tarsnap/keyfile" ).toString() );
}


/*! Destroys the object and frees any allocated resources. Needed only
    to please the C++ linker, bless its long-suffering soul.
*/

TarsnapOptions::~TarsnapOptions()
{
}


/*!

*/

void TarsnapOptions::browseForKey()
{
    
}


/*!

*/

void TarsnapOptions::browseForCache()
{
    
}


/*! This reimplementation of QDialog::accept() makes sure to write the
    settings and create the directory where the cache is to be written.
*/

void TarsnapOptions::accept()
{
    settings->setValue( "tarsnap/keyfile", keyFile->text() );
    settings->setValue( "tarsnap/cachefile", cacheFile->text() );
    QDialog::accept();
}
