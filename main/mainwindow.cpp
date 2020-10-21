#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QAbstractNativeEventFilter>
#include <QThread>
#include <QTimer>
#include <QtConcurrent>

class MyNativeEventFilter: public QAbstractNativeEventFilter
{
public:
    virtual bool nativeEventFilter(const QByteArray &eventType, void* message, long* param)
    {
        (void)eventType;
        (void)message;
        (void)param;
        qDebug() << "Inside nativeEventFilter";
        return false;
    }
};

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMainWindow)
{
    ui->setupUi(this);
    //QApplication::instance()->installNativeEventFilter(new MyNativeEventFilter);
    connect(ui->PB_OK, SIGNAL(clicked()), this, SLOT(slotThatTakesTime()));

    QThread::currentThread()->setObjectName( "Main GUI Thread" );
}

#define kILOOPCNT 2
#define kOLOOPCNT 1
void CMainWindow::processNetlistCreate()
{
    qDebug() << "processNetlistCreate: Thread ID: " << QThread::currentThread()->objectName() << "-" << QThread::currentThread();

    // For loop mimicing long process time
    for ( int i = 0; i < kILOOPCNT; i++ )
    {
        qDebug() << "processNetlistCreate inside Loop: Thread ID: " << QThread::currentThread()->objectName() << "-" << QThread::currentThread();
        qDebug() << "MainWindow::processNetlistCreate(): i=" << i;
        QThread::sleep( 1 );
    }
}

void CMainWindow::processStatusUpdate()
{
    // For loop mimicing long process time
    for(int i =0 ; i < kILOOPCNT; i++) {
        qDebug() << "processStatusUpdate inside Loop: Thread ID: " << QThread::currentThread()->objectName() << "-" << QThread::currentThread();
        qDebug() << "MainWindow::processStatusUpdate(): i=" << i;
        QThread::sleep(1);
    }
}

void CMainWindow::slotThatTakesTime()
{
    ui->PB_OK->setEnabled( false );
    std::function< void() > lFunc =
    [ this ]()
    {
        qDebug() << "Inside MainWindow::slotOKClicked";
        int loopCnt = 0;
        while ( loopCnt < kOLOOPCNT )
        {
            // Calling func that takes time so GUI will hang intermittently until process events is called
            qDebug() << "slotThatTakesTime inside Loop: Thread ID: " << QThread::currentThread()->objectName() << "-" << QThread::currentThread();
            qDebug() << "MainWindow::processStatusUpdate(): loopCnt=" << loopCnt;
            processNetlistCreate();
            processStatusUpdate();
            QApplication::instance()->processEvents( QEventLoop::ExcludeUserInputEvents );
            loopCnt++;
        }
    };

    CThreadedProgressDialog dlg( lFunc, this );
    dlg.setLabelText( "Long Running Thread Label" );
    dlg.exec();
    ui->PB_OK->setEnabled( true );
}

CMainWindow::~CMainWindow()
{
}

class CThreadedProgressDialogImpl 
{
public:
    CThreadedProgressDialogImpl( TVoidFunction xFunc, CThreadedProgressDialog * xParent ) :
        dFunction( xFunc ),
        dParent( xParent )
    {
    }

    void mRunIt()
    {
        auto lFuture = QtConcurrent::run( QThreadPool::globalInstance(), dFunction );
        dWatcher = new QFutureWatcher<void>( dParent );
        dWatcher->setFuture( lFuture );
        QObject::connect( dWatcher, &QFutureWatcher< void >::finished, dParent, &CThreadedProgressDialog::close );
    }
    QString dCancelButtonText{ QObject::tr( "&Cancel" ) };
    TVoidFunction dFunction;
    CThreadedProgressDialog * dParent{ nullptr };
    QFutureWatcher<void>* dWatcher{ nullptr };
};

CThreadedProgressDialog::CThreadedProgressDialog( TVoidFunction xFunc, const QString& xLabelText, const QString& xCancelButtonText, int xMinimum, int xMaximum, QWidget* xParent /*= nullptr*/, Qt::WindowFlags xFlags /*= Qt::WindowFlags() */ ) :
    QProgressDialog( xLabelText, xCancelButtonText, xMinimum, xMaximum, xParent, xFlags ),
    dImpl( new CThreadedProgressDialogImpl( xFunc, this ) )
{
    setWindowModality( Qt::WindowModal );
    mSetHasCancel( false );
}

CThreadedProgressDialog::CThreadedProgressDialog( TVoidFunction xFunc, QWidget* xParent /*= nullptr*/, Qt::WindowFlags xFlags /*= Qt::WindowFlags() */ ) :
    QProgressDialog( xParent, xFlags ),
    dImpl( new CThreadedProgressDialogImpl( xFunc, this ) )
{
    setWindowModality( Qt::WindowModal );
    mSetHasCancel( false );
    setMinimum( 0 );
    setMaximum( 0 );
}

void CThreadedProgressDialog::mSetHasCancel( bool xHasCancel )
{
    if ( xHasCancel )
        setCancelButton( new QPushButton( dImpl->dCancelButtonText ) );
    else
        setCancelButton( nullptr );
}

QString CThreadedProgressDialog::cancelButtonText() const
{
    return dImpl->dCancelButtonText;
}

void CThreadedProgressDialog::setCancelButtonText( const QString & xText )
{
    dImpl->dCancelButtonText = xText;
    QProgressDialog::setCancelButtonText( xText );
}

int CThreadedProgressDialog::exec()
{
    dImpl->mRunIt();
    return QProgressDialog::exec();
}

