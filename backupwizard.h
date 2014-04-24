// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef BACKUPWIZARD_H
#define BACKUPWIZARD_H

#include <QWizard>

#include <QSettings>


class BackupWizard: public QWizard
{
    Q_OBJECT
public:
    BackupWizard();
    
    void nag();

    enum { Paths, Account, Backup, Inclusion, Action };

    QString initialCacheDirectory() const;
    QString initialKeyFile() const;

    void storeSettings();

private:
    QSettings * settings;
};

#endif
