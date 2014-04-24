// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef INCLUDEPAGE_H
#define INCLUDEPAGE_H

#include <QWizardPage>

#include "backupwizard.h"

#include <QTreeView>
#include <QFileSystemModel>


class IncludePage: public QWizardPage
{
    Q_OBJECT
public:
    IncludePage( BackupWizard * parent );

    QStringList selectedDirectories();

    void initializePage() override;

    bool isComplete () const override;

    
private slots:
    void selectDirectoryChildren( const QString & );
    void checkCompleteness();

private:
    QTreeView * exceptions;
    QFileSystemModel * model;
    bool complete;
};


#endif
