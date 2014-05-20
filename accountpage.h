// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef ACCOUNTPAGE_H
#define ACCOUNTPAGE_H

#include <QWizardPage>

#include "backupwizard.h"

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QProcess>


class AccountPage: public QWizardPage
{
    Q_OBJECT
public:
    AccountPage( BackupWizard * parent );

    bool isComplete () const override;


signals:
    void completeChanged();


private slots:
    void act();
    void browseForFile();
    void handleExit(int, QProcess::ExitStatus);
    void checkFile();
    void enableMakeKey();


private:
    QLineEdit * file;
    QLineEdit * login;
    QLineEdit * password;
    QLineEdit * host;
    QProcess * tarsnap;
    QPushButton * makeKey;
    QLabel * processStatus;
    bool complete;
};

#endif
