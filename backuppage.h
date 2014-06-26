// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef BACKUPPAGE_H
#define BACKUPPAGE_H

#include <QWizardPage>

#include "backupwizard.h"

#include <QButtonGroup>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>


class BackupPage: public QWizardPage
{
    Q_OBJECT
public:
    BackupPage( BackupWizard * parent );

    bool isComplete () const override;
    int nextId() const override;

signals:
    void completeChanged();

private slots:
    void browseBaseDirectory();
    void checkDirectories();

private:
    QButtonGroup * behaviour;
    QLineEdit * baseDirectory;
    QCheckBox * crossMountPoints;
    QRadioButton * everything;
    QRadioButton * something;
    bool complete;
};



#endif
