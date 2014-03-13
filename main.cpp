#include <QDateTime>
#include <QApplication>

#include "stdio.h"

#include "backupconfiguration.h"



static void breakHere()
{
}


static void errorHandler( QtMsgType t, const char * message )
{
    if ( message && *message ) {
	QTime t( QTime::currentTime() );
	fprintf( stderr, "%02d:%02d:%02d.%03d %s\n",
		 t.hour(), t.minute(), t.second(), t.msec(),
		 message );
    }
    if ( t != QtDebugMsg )
	breakHere();
}


int main( int argc, char ** argv ) {
    //qInstallMsgHandler( errorHandler );
    QApplication * app = new QApplication( argc, argv );

    QWidget * backup = new BackupConfiguration();
    backup->show();

    int b = app->exec();
    delete app;
    return b;
}
