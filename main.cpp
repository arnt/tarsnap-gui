#include <QDateTime>
#include <QApplication>

#include "stdio.h"

#include "backupconfiguration.h"



int main( int argc, char ** argv ) {
    QApplication * app = new QApplication( argc, argv );

    QWidget * backup = new BackupConfiguration();
    backup->show();

    int b = app->exec();
    delete app;
    return b;
}
