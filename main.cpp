#include <QDateTime>
#include <QApplication>

#include "stdio.h"

#include "backupwizard.h"



int main( int argc, char ** argv ) {
    QApplication * app = new QApplication( argc, argv );

    BackupWizard * backup = new BackupWizard();
    backup->show();

    int b = app->exec();
    backup->storeSettings();
    delete app;
    return b;
}
