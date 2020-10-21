#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui
{
    class CMainWindow;
}

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMainWindow( QWidget* parent = nullptr );
    ~CMainWindow();

private slots:
    void slotThatTakesTime();

private:
    void processNetlistCreate();
    void processStatusUpdate();
private:
    std::unique_ptr < Ui::CMainWindow > ui;
};

#endif // MAINWINDOW_H
