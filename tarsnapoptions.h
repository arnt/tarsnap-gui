// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef TARSNAPOPTIONS_H
#define TARSNAPOPTIONS_H

// what we inherit
#include <QDialog>

// and what we use
#include <QLineEdit>
#include <QSettings>


class TarsnapOptions: public QDialog
{
    Q_OBJECT
public:
    TarsnapOptions( QSettings *, const QString & );
    ~TarsnapOptions();

private:
    void accept();

private slots:
    void createNewKey();
    void browseForKey();
    void browseForCache();

private:
    QString executableDirectory;
    QLineEdit * cacheDir;
    QLineEdit * keyFile;
    QSettings * settings;
};



#endif
