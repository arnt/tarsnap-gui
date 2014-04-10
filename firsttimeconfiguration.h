// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef FISTTIMECONFIGURATION_H
#define FISTTIMECONFIGURATION_H

// what we inherit
#include <QDialog>

// and what we use... could be replaced by forward declarations, but...
#include <QCheckBox>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QTreeView>


class FirstTimeConfiguration: public QDialog
{
    Q_OBJECT
public:
    FirstTimeConfiguration( QWidget *, const QString &, const QString & );
    ~FirstTimeConfiguration();

    QString filename() const;

private slots:
    void browseForFile();
    void checkFile( const QString & );
    void act();

private:
    QString executableDirectory;
    QLineEdit * login;
    QLineEdit * password;
    QLineEdit * host;
    QLineEdit * file;
    QPushButton * ok;
};

#endif
