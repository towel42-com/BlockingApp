#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "SABUtils/ThreadedProgressDialog.h"

#include <QDebug>
#include <QAbstractNativeEventFilter>
#include <QThread>

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
    connect(ui->progessBtn, &QPushButton::clicked, this, &CMainWindow::slotProgressBtn );
    connect( ui->eventBtn, &QPushButton::clicked, this, &CMainWindow::slotEventBtn );
    connect( this, &CMainWindow::sigToggle, this, &CMainWindow::slotToggle );

    QThread::currentThread()->setObjectName( "Main GUI Thread" );
}


CMainWindow::~CMainWindow()
{
}


#define kILOOPCNT 2
#define kOLOOPCNT 1
void CMainWindow::processNetlistCreate()
{
    qDebug() << "processNetlistCreate: Thread ID: " << QThread::currentThread()->objectName() << "-" << QThread::currentThread();

    // For loop mimicing long process time
    for ( int ii = 0; ii < kILOOPCNT; ii++ )
    {
        qDebug() << "processNetlistCreate inside Loop: Thread ID: " << QThread::currentThread()->objectName() << "-" << QThread::currentThread();
        qDebug() << "MainWindow::processNetlistCreate(): ii=" << ii;
        emit sigToggle( ( ii % 2 ) == 1 );
        QThread::sleep( 1 );
    }
}

void CMainWindow::processStatusUpdate()
{
    // For loop mimicing long process time
    for(int ii =0 ; ii < kILOOPCNT; ii++) {
        qDebug() << "processStatusUpdate inside Loop: Thread ID: " << QThread::currentThread()->objectName() << "-" << QThread::currentThread();
        qDebug() << "MainWindow::processStatusUpdate(): ii=" << ii;
        emit sigToggle( (ii % 2) == 1 );
        QThread::sleep(1);
    }
}

std::function< void() > CMainWindow::getFunc()
{
    std::function< void() > lFunc =
        [this]()
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
            loopCnt++;
            emit sigToggle( (loopCnt % 2) == 1 );
        }
    };
    return lFunc;
}

void CMainWindow::slotProgressBtn()
{
    lAllowEnable = true;
    emit sigToggle( false );

    NSABUtils::CThreadedProgressDialog dlg( getFunc(), this );
    dlg.setLabelText( "Long Running Thread Label" );
    dlg.exec();

    emit sigToggle( true );
}

void CMainWindow::slotEventBtn()
{
    lAllowEnable = false;
    emit sigToggle( false );

    NSABUtils::CThreadedEventLoop eLoop( getFunc(), this );
    eLoop.exec();

    lAllowEnable = true;
    emit sigToggle( true );
}

void CMainWindow::slotToggle( bool xEnabled )
{
    if ( xEnabled && !lAllowEnable )
        return;
    ui->progessBtn->setEnabled( xEnabled );
    ui->eventBtn->setEnabled( xEnabled );
}
