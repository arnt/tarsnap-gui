// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "backupwizard.h"

#include "tarsnappathpage.h"
#include "accountpage.h"
#include "backuppage.h"
#include "includepage.h"
#include "actionpage.h"

#include <QMessageBox>




/*! \class BackupWizard backupwizard.h
  
    This is the main class; it does everything from zero to either
    carrying out a backup or writing a backup script. As a bonus it
    nags the user about properly securing the key file.

    If this grows to do more than backup, then this class as to be
    split so someone else takes over the settings management.
*/


/*! Constructs a backup wizard. */

BackupWizard::BackupWizard()
    : QWizard( nullptr ),
      settings( new QSettings( "Tarsnap", "Backup Configuration") )
{
    setPage( Paths, new TarsnapPathPage( this ) );
    setPage( Account, new AccountPage( this ) );
    setPage( Backup, new BackupPage( this ) );
    IncludePage * i = new IncludePage( this );
    setPage( Inclusion, i );
    setPage( Action, new ActionPage( this, i ) );

    if ( !page( Paths )->isComplete() )
	setStartId( Paths );
    else if ( !page( Account )->isComplete() )
	setStartId( Account );
    else
	setStartId( Backup );

    resize( 700, 434 );
}


/*! Returns the stored value for the key file, or an empty string if
    there is no stored value (yet).
*/

QString BackupWizard::initialKeyFile() const
{
    return settings->value( "tarsnap/keyfile" ).toString();
}


/*! Returns the stored value for the cache directory, or an empty
    string if there no stored value yet.
*/

QString BackupWizard::initialCacheDirectory() const
{
    return settings->value( "tarsnap/cachedir" ).toString();
}


/*! Called at the end of the wizard to store the settings. The
    settings aren't stored earlier, since users who click cancel may
    expect the old settings to remain in force.
*/

void BackupWizard::storeSettings()
{
    settings->setValue( "tarsnap/keyfile",
			field( "keyFile" ).toString() );
    settings->setValue( "tarsnap/cachedir",
			field( "cacheDirectory" ).toString() );
}


/*! This reimplementation nags the user about copying the key file
    somewhere safe.
*/

void BackupWizard::nag()
{
    if ( settings->value( "tarsnap/copied" ).toBool() )
        return;

    QMessageBox * b = new QMessageBox( this );
    b->setText(
        tr( "<b>Have you copied %1 to a safe location yet?</b>" )
        .arg( field( "keyFile" ).toString() ) );
    b->setInformativeText(
        tr( "<html>You may want to copy the file to two USB sticks, "
            "label them well and store them separately. Or perhaps you "
            "prefer something else. Do whatever you want, but "
            "keep %1 safe.<p>"
            "You <b>will not be able to restore a backup</b> "
            "without that file. Even the NSA probably cannot "
            "help you decrypt your backups if you lose that file.</html>" )
        .arg( field( "keyFile" ).toString() ) );
    // that's not the kind of language favoured by corporate technical
    // writers. it's the kind of language favoured by arnts who have
    // sat too long in an office and need to get laid. I wonder how
    // well tarsnap's audience likes it.
    b->setStandardButtons( QMessageBox::Yes | QMessageBox::No );
    b->setDefaultButton( QMessageBox::No );
    int ret = b->exec();
    if ( ret == QMessageBox::Yes )
        settings->setValue( "tarsnap/copied", true );
}
