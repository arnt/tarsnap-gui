// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef WATCHER_H
#define WATCHER_H

// what we inherit
#include <QDialog>

// and what we use
#include <QLineEdit>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QTextEdit>


class Watcher: public QDialog
{
    Q_OBJECT
public:
    Watcher( QProcess *, const QString &, QWidget * );
    ~Watcher();

    void cancel();

private slots:
    void read();
    void finish();


private:
    QProcess * tarsnap;
    QTextEdit * stdout;
    QPushButton * close;
};


#endif
