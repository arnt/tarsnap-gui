// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef ACTIONPAGE_H
#define ACTIONPAGE_H

#include <QWizardPage>

#include "backupwizard.h"

#include "includepage.h"

#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QProcess>



class ActionPage: public QWizardPage
{
    Q_OBJECT
public:
    ActionPage( BackupWizard * parent, IncludePage * );
    
    void cleanupPage() override;
    bool validatePage() override;

private:
    QString shQuoted( const QString & ) const;
    QStringList commandLine( const QString & ) const;

private slots:
    void performBackup();
    void saveScript();
    void read();
    void finish();

private:
    QTextEdit * stdout;
    QPushButton * writeScript;
    QPushButton * backup;
    QProcess * tarsnap;
    BackupWizard * w;
    IncludePage * inclusions;
};


#endif
