// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "accountpage.h"

// geteuid()
#include <unistd.h>
#include <sys/types.h>

// getpwuid()
#include <sys/types.h>
#include <pwd.h>

#include <QGridLayout>

#include <QFileDialog>

#include <QFile>
#include <QDir>

#include <QTextStream>
#include <QDateTime>

#include <QMessageBox>
#include <QProcess>




/*! \class AccountPage accountpage.h

    This class makes sure we have the details we need about the
    tarsnap account; at the end all we need is the key file, but
    getting there may require more.
*/

/*! Constructs an empty wizard page with the filename preset if
    possible.
*/

AccountPage::AccountPage( BackupWizard * parent )
    : QWizardPage( parent ),
      file( new QLineEdit( this ) ),
      login( new QLineEdit( this ) ),
      password( new QLineEdit( this ) ),
      host( new QLineEdit( "", this ) ),
      tarsnap( nullptr ),
      makeKey( new QPushButton( tr( "&Make new key" ), this ) ),
      processStatus( new QLabel( "", this ) )
{
    setTitle( "Host Key" );
    setSubTitle( "Make or select host key" );
    connect( file, SIGNAL(textChanged(const QString &)),
	     this, SLOT(checkFile()) );

    login->setPlaceholderText( tr( "Email address" ) );
    password->setPlaceholderText( tr( "tarsnap.com password" ) );

    password->setEchoMode( QLineEdit::Password );

    char buf[1024];
    ::gethostname( buf, 1023 );
    buf[1023] = 0;
    host->setText( QString::fromAscii( buf ) );

    file->setText( parent->initialKeyFile() );
    if ( file->text().isEmpty() ) { // have to have some default...
	struct passwd * pw = getpwuid(geteuid());
	if ( pw )
	    file->setText( QString::fromUtf8( pw->pw_dir ) + "/" +
			   host->text() + ".tarsnapkey" );
	else
	    file->setText( "/tmp/" + host->text() + ".tarsnapkey" );
    }

    registerField( "keyFile", file );
    registerField( "host", host );

    connect( makeKey, SIGNAL(clicked()),
	     this, SLOT(act()) );
    enableMakeKey();

    connect( login, SIGNAL(textChanged(const QString &)),
	     this, SLOT(enableMakeKey()) );
    connect( password, SIGNAL(textChanged(const QString &)),
	     this, SLOT(enableMakeKey()) );

    QGridLayout * l = new QGridLayout( this );

    l->addWidget( new QLabel( tr( "Key file" ), this ),
		  0, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( file,
		  0, 1, 1, 1 );

    QPushButton * b = new QPushButton( tr( "..." ), this );
    connect( b, SIGNAL(clicked()),
	     this, SLOT(browseForFile()) );
    l->addWidget( b,
		  0, 2, 1, 1 );

    QLabel * howto = new QLabel(
	tr( "<html>"
	    "If you don't have a host key yet, you have to register at "
	    "<a href=https://tarsnap.com/register.cgi>tarsnap.com</a> and "
	    "choose a password. In that case, please follow "
	    "the link and register, and add enough funds to pay for the first "
	    "backup.<p>"
	    "The email address and password "
	    "are needed to create the host key, and the host name so you can "
	    "back up several hosts using one tarsnap account.<p>"
	    "The key is stored only in the file you specify. "
	    "The email address is stored on tarsnap.com. "
	    "The password is not stored anywere.</html>" ),
	this );
    howto->setWordWrap( true );
    howto->setOpenExternalLinks( true );

    l->addWidget( howto,
		  1, 1, 1, 1 );

    l->addWidget( new QLabel( tr( "Email" ), this ),
		  2, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( login,
		  2, 1, 1, 1 );

    l->addWidget( new QLabel( tr( "Password" ), this ),
		  3, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( password,
		  3, 1, 1, 1 );

    l->addWidget( new QLabel( tr( "Backup host" ), this ),
		  4, 0, 1, 1, Qt::AlignLeft );
    l->addWidget( host,
		  4, 1, 1, 1 );

    QHBoxLayout * processLayout = new QHBoxLayout();
    l->addLayout( processLayout,
		  5, 1, 1, 1, Qt::AlignLeft );
    processLayout->addWidget( makeKey );
    processLayout->addWidget( processStatus, 2 );
}


bool AccountPage::isComplete() const
{
    QString name = file->text();
    if ( name.isEmpty() )
	 return false;
    QFileInfo f( name );
    return f.exists() && f.isFile();
}


/*! Browses for for the local file name. */

void AccountPage::browseForFile()
{
    QString result = QFileDialog::getSaveFileName( this );
    if ( !result.isNull() )
	file->setText( result );
    emit completeChanged();
}




/*! Runs tarsnap-keygen and makes sure we'll provide feedback later.
*/

void AccountPage::act()
{
    if( tarsnap )
	delete tarsnap;
    tarsnap = new QProcess( this );
    tarsnap->setProcessChannelMode( QProcess::MergedChannels );
    QStringList cli;
    cli << "--keyfile" << file->text().toUtf8()
	<< "--user" << login->text().toUtf8()
	<< "--machine" << host->text().toUtf8();
    connect( tarsnap, SIGNAL(finished(int, QProcess::ExitStatus)),
	     this, SLOT(handleExit(int, QProcess::ExitStatus)) );
    tarsnap->start( field("tarsnapPath").toString() + "/tarsnap-keygen", cli );
    QByteArray pw( password->text().toUtf8() + "\n" );
    tarsnap->write( pw.data(), pw.size() );
    processStatus->setText( tr( "Running..." ) );
}


/*! Contemplates what may have happened to the tarsnap-keygen process. */

void AccountPage::handleExit(int code, QProcess::ExitStatus status)
{
    if ( status == QProcess::NormalExit && code == 0 ) {
	processStatus->setText( tr( "Key successfully made" ) );
	makeKey->setEnabled( false );
	emit completeChanged();
	return;
    }

    processStatus->setText( tr( "tarsnap-keygen failed" ) );
    QMessageBox::critical( wizard(),
			   tr( "Error running tarsnap-keygen" ),
			   tr( "Oddly, tarsnap-keygen did not finish "
			       "normally. It seems likely that something "
			       "may have failed. The command line that "
			       "failed was quite likely "
			       "<code>tarsnap-keygen %1</code>" )
			   .arg("--keyfile " + file->text() +
				" --user " + login->text() +
				" --machine " + host->text() ) );
}


/*! This helper makes sure to emit completeChanged() whenever the key
    file's status changes.
*/

void AccountPage::checkFile()
{
    bool was = complete;
    complete = isComplete();
    // let's not casually overwrite a valuable key
    enableMakeKey();
    if ( was != complete )
	emit completeChanged();
}


/*! This helper enables/disables the "make key" button depending on
    whether the user has provided the necessary input.
*/

void AccountPage::enableMakeKey()
{
    makeKey->setEnabled( !isComplete() &&
			 login->text().contains( "@" ) &&
			 !password->text().isEmpty() );
}
