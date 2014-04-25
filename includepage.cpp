// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "includepage.h"

#include <QGridLayout>


/*! \class IncludePage includepage.h

    IncludePage is the optional page to finetune which directories are
    included in backups.
*/

/*! Constructs an IncludePage with every directory selected.
*/

IncludePage::IncludePage( BackupWizard * parent )
    : QWizardPage( parent ),
      exceptions( new QTreeView( this ) ),
      model( new QFileSystemModel() ),
      complete( false )
{
    setTitle( tr( "Select subdirectories" ) );
    setSubTitle( tr( "Select the subdirectories to "
		     "be included in the backup" ) );
    
    connect( model, SIGNAL(directoryLoaded(const QString &)),
	     this, SLOT(selectDirectoryChildren(const QString & )) );

    model->setFilter( QDir::Dirs | QDir::NoSymLinks |
		      QDir::NoDotAndDotDot );

    QString s( field( "baseDirectory" ).toString() );
    model->setRootPath( s );
    exceptions->setRootIndex( model->setRootPath( s ) );

    exceptions->setModel( model );
    exceptions->setIndentation( 42 );
    exceptions->setSortingEnabled( true );
    exceptions->sortByColumn( 0, Qt::AscendingOrder );
    exceptions->setSelectionMode( QAbstractItemView::MultiSelection );
    exceptions->setColumnHidden( 1, true );
    exceptions->setColumnHidden( 2, true );
    exceptions->setColumnHidden( 3, true ); // even the date is pointless.

    QGridLayout * l = new QGridLayout( this );
    l->addWidget( exceptions,
		  0, 0 );
    // huh. rather degenerate grid, that.
}


/*! The user has to select something to back up in order to proceed.
*/

bool IncludePage::isComplete() const
{
    return exceptions->selectionModel()->hasSelection();
}


/*! This private directory makes sure all directories are selected as
    soon as the GUI learns they exist.
*/

void IncludePage::selectDirectoryChildren( const QString & n )
{
    QItemSelectionModel * selections = exceptions->selectionModel();
    auto parent = model->index( n );
    auto childRow = model->rowCount( parent );
    while ( childRow-- > 0 )
	selections->select( parent.child( childRow, 0 ),
			    QItemSelectionModel::Select );
}


void IncludePage::initializePage()
{
    QString s( field( "baseDirectory" ).toString() );
    model->setRootPath( s );
    exceptions->setRootIndex( model->setRootPath( s ) );
    exceptions->selectAll();
    connect( exceptions->selectionModel(),
	     SIGNAL(selectionChanged( const QItemSelection &, const QItemSelection & )),
	     this, SLOT(checkCompleteness()) );
}


/*! Returns a list of all selected directory names. The list may be
    empty while the page is on-screen, but cannot be empty after that
    point, due to the staunch efforts of isComplete().

    Each name is relative to the backup root.
*/

QStringList IncludePage::selectedDirectories()
{
    QStringList result;
    QModelIndexList includedDirectories =
	exceptions->selectionModel()->selectedIndexes();
    QString prefix = field( "baseDirectory" ).toString();
    if ( prefix != "/" )
	prefix += "/";
    auto x = includedDirectories.begin();
    while ( x != includedDirectories.end() ) {
	if ( x->column() == 0 ) {
	    QString name = x->data( QFileSystemModel::FilePathRole )
			   .toString()
			   .mid( prefix.length() );
	    if ( !name.isEmpty() )
		result << name;
	}
	++x;
    }
    return result;
}


void IncludePage::checkCompleteness()
{
    bool was = complete;
    complete = isComplete();
    if ( was != complete )
	emit completeChanged();
}
