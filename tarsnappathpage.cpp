// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "tarsnappathpage.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QFileDialog>

#include <QFile>
#include <QDir>

#include <QTextStream>
#include <QDateTime>

#include <QMessageBox>
#include <QLabel>
#include <QProcess>


/*! \class TarsnapPathPage tarsnappathpage.h
  
    The TarsnapPathPage class encompasses this program's knowledge of
    one thing: Where tarsnap itself is to be found. With any luck, no
    user will ever see this.
*/

/*! Constructs a wizard page to ask where the tarsnap executable may
    perchance be located. The default should be good, but...
*/

TarsnapPathPage::TarsnapPathPage( BackupWizard * parent )
    : QWizardPage( parent ),
      tarsnapPath( new QLineEdit( this ) )
{
    setDefaultPath();
    registerField( "tarsnapPath", tarsnapPath );

    QGridLayout * l = new QGridLayout( this );
    
    QLabel * first = new QLabel(
	tr( "<html>"
	    "This UI works by executing tarsnap and perhaps tarsnap-keygen, "
	    "so it needs to know where those programs are.</html>" ),
	this );
    first->setWordWrap( true );

    l->addWidget( first,
		  0, 0, 1, 3 );

    l->addWidget( new QLabel( tr( "Directory" ), this ),
		  1, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( tarsnapPath,
		  1, 1, 1, 1 );
}


/*! This private helper sets the path to tarsnap based on $PATH, or
    does nothing if tarsnap cannot be found. isComplete() will ensure
    that the user helps us if not, and isComplete() together with
    BackupWizard will ensure that this entire wizard page generalyl
    isn't shown.
*/

void TarsnapPathPage::setDefaultPath()
{
    QStringList path = QString::fromUtf8( ::getenv( "PATH" ) )
		       .split( ':', QString::SkipEmptyParts );
    auto dir = path.begin();
    while ( dir != path.end() ) {
	QFile candidate( *dir + "/tarsnap" );
	if ( candidate.exists() ) {
	    tarsnapPath->setText( *dir );
	    return;
	}
	++dir;
    }
}


/*! Returns true if we can proceed. Strictly speaking we could even
    without, in some rare circumstances, but that's too odd. Best to
    demand that we can find tarsnap. Simplicity is a feature.
*/

bool TarsnapPathPage::isComplete() const
{
    QString path = tarsnapPath->text();
    if ( path.isEmpty() )
	return false;
    if ( !path.endsWith( "/" ) )
	path.append( "/" );
    path.append( "tarsnap" );
    QFile candidate( path );
    return candidate.exists();
}
