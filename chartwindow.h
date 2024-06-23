#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QDialog>
#include <qcustomplot.h>
#include <QTimer>
#include <QSerialPort>
#include <QVector>


namespace Ui {
class ChartWindow;
}

class ChartWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ChartWindow(QWidget *parent = nullptr,QSerialPort* device = nullptr);
    ~ChartWindow();

private slots:
    void on_pushButtonResume_clicked();
    void realtimeDataSlot();

    void on_pushButtonPause_clicked();

    void on_pushButtonClearChart_clicked();

    void on_pushButton_clicked();

    void on_SavetoFile_clicked();

private:
    Ui::ChartWindow *ui;
    QTimer* dataTimer;
    QSerialPort* device;
    //QTime time = QTime::currentTime();
    QElapsedTimer time;

    QString allData; //here is current data read from serial port
    QVector<QString> storedData; // in this vector I store data from current investigation
    QVector<double> storedMarkers; // in this vector I store moments when markers occured

    uint16_t dataGSR;
    uint16_t dataECG;
    uint16_t dataPNG;
    uint16_t dataPNG_2;


    uint16_t receiveDataGSR();
    uint16_t receiveDataECG();
    uint16_t receiveDataPNG();
    uint16_t testSignal();

    double *currentPoint; //holds a current X value of graphs, for which an Y value is going to be assigned

    QCPItemStraightLine* markerTab[18];
    QVector <QCPItemStraightLine*> markerVect;

    void setupGraphs(); //standard setup for graphs
    void receivePlotData(); // reads all data from serial port, and saves it in allData atribute

};

#endif // CHARTWINDOW_H
