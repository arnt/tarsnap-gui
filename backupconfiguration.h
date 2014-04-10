// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef BACKUPCONFIGURATION_H
#define BACKUPCONFIGURATION_H

// what we inherit
#include <QWidget>

// and what we use... could be replaced by forward declarations, but...
#include <QCheckBox>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QTreeView>


class BackupConfiguration: public QWidget
{
    Q_OBJECT
public:
    BackupConfiguration();
    ~BackupConfiguration();
    
    void closeEvent( QCloseEvent * );

private:
    void setupLayout();
    void setupActions();
    void setupSubstance();

    QString shQuoted( const QString & ) const;

    QStringList commandLine( const QString & ) const;

private slots:
    void checkBaseDirectory( const QString & );
    void browseBaseDirectory();
    void performBackup();
    void saveScript();
    void configure();

private:
    QString executableDirectory;
    QLineEdit * baseDirectory;
    QPushButton * browse;
    QCheckBox * crossMountPoints;

    QTreeView * exceptions;
    QFileSystemModel * model;

    QPushButton * settings;
    QPushButton * backup;
    QPushButton * writeScript;

    QSettings * settingsStorage;
};

#endif
