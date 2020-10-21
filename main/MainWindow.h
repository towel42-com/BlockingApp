#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>
#include <QFutureWatcher>
#include <functional>

namespace Ui
{
    class CMainWindow;
}

using TVoidFunction = std::function< void() >;
class CThreadedProgressDialogImpl;
class CThreadedProgressDialog : public QProgressDialog
{
    Q_OBJECT;
public:
    CThreadedProgressDialog( TVoidFunction xFunc, const QString& xLabelText, const QString& xCancelButtonText, int xMinimum, int xMaximum, QWidget* xParent = nullptr, Qt::WindowFlags xFlags = Qt::WindowFlags() );
    CThreadedProgressDialog( TVoidFunction xFunc, QWidget* xParent = nullptr, Qt::WindowFlags xFlags = Qt::WindowFlags() );

    void setCancelButtonText( const QString& xText );
    QString cancelButtonText() const;

    void mSetHasCancel( bool xHasCancel );

    int exec() override;
private:
    std::unique_ptr< CThreadedProgressDialogImpl > dImpl;
};


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
