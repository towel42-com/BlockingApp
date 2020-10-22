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
    connect(ui->PB_OK, SIGNAL(clicked()), this, SLOT(slotThatTakesTime()));
    connect( this, &CMainWindow::sigToggle, ui->PB_OK, &QPushButton::setEnabled );

    QThread::currentThread()->setObjectName( "Main GUI Thread" );
}

#define kILOOPCNT 8
#define kOLOOPCNT 4
void CMainWindow::processNetlistCreate()
{
    qDebug() << "processNetlistCreate: Thread ID: " << QThread::currentThread()->objectName() << "-" << QThread::currentThread();

    // For loop mimicing long process time
    for ( int ii = 0; ii < kILOOPCNT; ii++ )
    {
        qDebug() << "processNetlistCreate inside Loop: Thread ID: " << QThread::currentThread()->objectName() << "-" << QThread::currentThread();
        qDebug() << "MainWindow::processNetlistCreate(): ii=" << ii;
        emit sigToggle( ( ii % 2 ) == 1 );
        for( int jj = 0; jj < 10900900; ++jj )
        {
            jj = jj + 100;
            ;
        }
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
            emit sigToggle( (loopCnt % 2) == 1 );
            loopCnt++;
        }
    };

    NSABUtils::CThreadedProgressDialog dlg( lFunc, this );
    dlg.setLabelText( "Long Running Thread Label" );
    dlg.exec();
    ui->PB_OK->setEnabled( true );
}

CMainWindow::~CMainWindow()
{
}

