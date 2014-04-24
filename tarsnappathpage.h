// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef TARSNAPPATHPAGE_H
#define TARSNAPPATHPAGE_H

#include <QWizardPage>

#include "backupwizard.h"

#include <QLineEdit>


class TarsnapPathPage: public QWizardPage
{
    Q_OBJECT
public:
    TarsnapPathPage( BackupWizard * parent );

    bool isComplete () const override;

private:
    void setDefaultPath();

private:
    QLineEdit * tarsnapPath;
};


#endif
