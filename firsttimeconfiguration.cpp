// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "firsttimeconfiguration.h"

#include "watcher.h"

#include <unistd.h>

#include <QGridLayout>

#include <QLabel>
#include <QPushButton>

#include <QFileDialog>

#include <QProcess>



/*! \class FirstTimeConfiguration firsttimeconfiguration.h
  
    The FirstTimeConfiguration class registers the user with tarsnap
    and creates a key. About half the work is done by calling out to
    the user and the other half by calling out to tarsnap-keygen.
*/


/*! Constructs a naked FirstTimeConfiguration. */

FirstTimeConfiguration::FirstTimeConfiguration( QWidget * parent,
						const QString & defaultName )
    : QDialog( parent ),
      login( new QLineEdit( this ) ),
      password( new QLineEdit( this ) ),
      host( new QLineEdit( "", this ) ),
      file( new QLineEdit( defaultName, this ) ),
      ok( new QPushButton( tr( "OK" ), this ) )
{
    setModal( true );
    
    connect( file, SIGNAL(textChanged(const QString &)),
	     this, SLOT(checkFile(const QString &)) );
    
    login->setPlaceholderText( tr( "Email address" ) );
    password->setPlaceholderText( tr( "tarsnap.com password" ) );

    password->setEchoMode( QLineEdit::Password );

    char buf[1024];
    ::gethostname( buf, 1023 );
    buf[1023] = 0;
    host->setText( QString::fromAscii( buf ) );

    if ( file->text().isEmpty() ) // have to have some default...
	file->setText( "/root/" + host->text() + ".tarsnapkey" );

    QGridLayout * l = new QGridLayout( this );
    
    QLabel * first = new QLabel(
	tr( "<html>"
	    "Before you can use tarsnap, you have to register at "
	    "<a href=https://tarsnap.com/register.cgi>tarsnap.com</a> and "
	    "choose a login and password. The data you choose there "
	    "are needed by this dialog to create a host-specific key.<p>"
	    "If you already have a key, you can point to it in the "
	    "<i>key file</i> field and hit OK.<p>"
	    "Only the key is stored.</html>" ),
	this );
    first->setWordWrap( true );
    first->setOpenExternalLinks( true );

    l->addWidget( first,
		  0, 0, 1, 3 );

    l->addWidget( new QLabel( tr( "Email" ), this ),
		  1, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( login,
		  1, 1, 1, 1 );

    l->addWidget( new QLabel( tr( "Password" ), this ),
		  2, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( password,
		  2, 1, 1, 1 );

    l->addWidget( new QLabel( tr( "Backup host" ), this ),
		  3, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( host,
		  3, 1, 1, 1 );

    l->addWidget( new QLabel( tr( "Key file" ), this ),
		  4, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( file,
		  4, 1, 1, 1 );

    QPushButton * b = new QPushButton( tr( "..." ), this );
    connect( b, SIGNAL(clicked()),
	     this, SLOT(browseForFile()) );
    l->addWidget( b,
		  4, 2, 1, 1 );

    QBoxLayout * actionButtons = new QHBoxLayout();
    l->addLayout( actionButtons,
		  5, 1, 1, 1, Qt::AlignRight );

    b = new QPushButton( tr( "Cancel" ), this );
    connect( b, SIGNAL(clicked()),
	     this, SLOT(reject()) );
    actionButtons->addWidget( b );

    ok->setDefault( true );
    connect( ok, SIGNAL(clicked()),
	     this, SLOT(act()) );
    actionButtons->addWidget( ok );
    
}


/*! Needed only by the linker. */

FirstTimeConfiguration::~FirstTimeConfiguration()
{
}


/*! Browses for for the local file name. */

void FirstTimeConfiguration::browseForFile()
{
    file->setText( QFileDialog::getOpenFileName( this ) );
}


/*! Runs tarsnap-keygen and if all is well, closes this dialog. */

void FirstTimeConfiguration::act()
{
    QFile f( file->text() );
    if( f.exists() ) {
	accept();
	return;
    }

    QProcess * tarsnap = new QProcess( this ); // * -> leak
    tarsnap->setProcessChannelMode( QProcess::MergedChannels );
    QStringList cli;
    cli << "--keyfile" << file->text().toUtf8()
	<< "--user" << login->text().toUtf8()
	<< "--machine" << host->text().toUtf8();
    connect( tarsnap, SIGNAL(finished(int, QProcess::ExitStatus)),
	     this, SLOT(accept()) );
    tarsnap->start( "/usr/local/bin/tarsnap-keygen", cli );
    QByteArray pw( password->text().toUtf8() + "\n" );
    tarsnap->write( pw.data(), pw.size() );
    ok->setText( tr( "Working" ) );
    ok->setEnabled( false );
}


/*! Returns the filename of the created key, or a null string if no
    key has been created.
*/

QString FirstTimeConfiguration::filename() const
{
    return file->text();
}


/*! Enables/disables the OK button based on whether filename() is at
    all reasonable. Acting on OK may require creating a file there,
    but that's okay, we know how to do that.
*/

void FirstTimeConfiguration::checkFile( const QString & name )
{
    QFileInfo f( name );
    ok->setEnabled( !name.isEmpty() && f.dir().exists() );
}
