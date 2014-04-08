// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "watcher.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>


/*! \class Watcher watcher.cpp

    The Watcher class watches stdout from a process, and even, in a
    classic case of feature creep, allows cancelling that
    process. It's singularly useless, backups should run as root and
    GUIs should not, not even a little, but I feel so humble right now
    that I write it anyway. Maybe I'm wrong sometimes.
*/


/*! Constructs a Watcher presenting and somewhat controlling \a source. */

Watcher::Watcher( QProcess * source, const QString & label, QWidget * parent )
    : QDialog( parent ),
      tarsnap( source ),
      stdout( new QTextEdit( this ) ),
      close( new QPushButton( tr( "Run in Background" ), this ) )
{
    close->setDefault( true );

    stdout->setLineWrapMode( QTextEdit::NoWrap );
    stdout->setReadOnly( true );

    QVBoxLayout * v = new QVBoxLayout( this );

    v->addWidget( new QLineEdit( tr( "Options: " ) + label, this ),
		  Qt::AlignLeft );
    v->addWidget( stdout );

    QBoxLayout * actionButtons = new QHBoxLayout();
    v->addLayout( actionButtons,
		  Qt::AlignRight );
    actionButtons->addStretch( 1 );
    QPushButton * b = new QPushButton( tr( "&Cancel" ), this );
    connect( b, SIGNAL(clicked()),
	     this, SLOT(reject()) );
    actionButtons->addStretch( 1 );
    actionButtons->addWidget( b );

    actionButtons->addWidget( close );
    connect( close, SIGNAL(clicked()),
	     this, SLOT(accept()) );

    connect( tarsnap, SIGNAL(readyReadStandardOutput()),
	     this, SLOT(read()) );
    connect( tarsnap, SIGNAL(finished(int, QProcess::ExitStatus)),
	     this, SLOT(finish()) );
}


/*! Exists merely to relieve the existential angst of the linker. */

Watcher::~Watcher()
{
    // nothing needed
}


/*! This slot reads tarsnap's stdout and puts it onscreen. */

void Watcher::read()
{
    QByteArray a = tarsnap->readAll();
    stdout->setPlainText( stdout->toPlainText() +
			  QString::fromUtf8( a.data(), a.size() ) );
}


/*! This slot changes the UI to signal that the backup is done. */

void Watcher::finish()
{
    close->setText( tr( "Close" ) );
}
