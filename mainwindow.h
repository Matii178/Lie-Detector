#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <chartwindow.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonSearch_clicked();

    void on_pushButtonConnect_clicked();

    void on_pushButtonDisconnect_clicked();

    void on_pushButtonTryConnection_clicked();

    void on_pushButtonRealTimeChart_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort* device;
    ChartWindow* secondWindow;

    void sendLogMessage(QString Message);
};
#endif // MAINWINDOW_H
