#include "mainwindow.h"
#include "ui_mainwindow.h"
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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QApplication::instance()->installNativeEventFilter(new MyNativeEventFilter);
    connect(ui->PB_OK, SIGNAL(clicked()), this, SLOT(slotThatTakesTime()));
}

void MainWindow::processNetlistCreate()
{
    // For loop mimicing long process time
    for(int i =0 ; i < 10; i++) {
        qDebug() << "MainWindow::processNetlistCreate(): i=" << i;
        QThread::sleep(1);
    }
}

void MainWindow::processStatusUpdate()
{
    // For loop mimicing long process time
    for(int i =0 ; i < 10; i++) {
        qDebug() << "MainWindow::processStatusUpdate(): i=" << i;
        QThread::sleep(1);
    }
}

void MainWindow::slotThatTakesTime()
{
    qDebug() << "Inside MainWindow::slotOKClicked";
    int loopCnt = 0;
    while(loopCnt < 10) {
        // Calling func that takes time so GUI will hang intermittently until process events is called
        qDebug() << "MainWindow::processStatusUpdate(): loopCnt=" << loopCnt;
        processNetlistCreate();
        processStatusUpdate();
        QApplication::instance()->processEvents(QEventLoop::ExcludeUserInputEvents);
        loopCnt++;
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}


