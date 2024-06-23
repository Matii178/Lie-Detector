#include "chartwindow.h"
#include "ui_chartwindow.h"
#include <QTimer>
#include <QSerialPortInfo>
#include <QDebug>
#include <stdlib.h>
#include <string.h>
#include <QVector>
#include <QFile>
#include <QTextStream>

static bool firstTime = true;

ChartWindow::ChartWindow(QWidget *parent,QSerialPort* device) :
    QDialog(parent),
    ui(new Ui::ChartWindow)
{
    ui->setupUi(this);
    this->dataTimer = new QTimer(this);
    this->device = device;

    this->dataECG = 0;
    this->dataPNG = 0;
    this->dataGSR = 0;

}

ChartWindow::~ChartWindow()
{
    delete ui;
}

void ChartWindow::setupGraphs()
{
    this->ui->widgetGSR->addGraph();
    this->ui->widgetPneumograph->addGraph();
    this->ui->widgetPneumograph->addGraph(); //nowy graf na tym samym wykresie
    this->ui->widgetECG->addGraph();

    this->ui->widgetGSR->graph()->setPen(QColor(40,110,255));
    this->ui->widgetPneumograph->graph(0)->setPen(QColor(40,110,255)); //mod
    this->ui->widgetPneumograph->graph(1)->setPen(QColor(210,110,110)); // .
    this->ui->widgetECG->graph()->setPen(QColor(40,110,255));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");

    //part for GSR
    this->ui->widgetGSR->xAxis->setTicker(timeTicker);
    this->ui->widgetGSR->axisRect()->setupFullAxesBox();
    this->ui->widgetGSR->yAxis->setRange(-100,4200);
    this->ui->widgetGSR->setInteraction(QCP::iRangeDrag, true);
    this->ui->widgetGSR->axisRect()->setRangeDrag(Qt::Horizontal);
    this->ui->widgetGSR->setInteraction(QCP::iRangeZoom, true);
    this->ui->widgetGSR->axisRect()->setRangeZoom(Qt::Vertical);

    //part for pneumograph
    this->ui->widgetPneumograph->xAxis->setTicker(timeTicker);
    this->ui->widgetPneumograph->axisRect()->setupFullAxesBox();
    this->ui->widgetPneumograph->yAxis->setRange(900,2250);
    this->ui->widgetPneumograph->setInteraction(QCP::iRangeDrag, true);
    this->ui->widgetPneumograph->axisRect()->setRangeDrag(Qt::Horizontal);
    this->ui->widgetPneumograph->setInteraction(QCP::iRangeZoom, true);
    this->ui->widgetPneumograph->axisRect()->setRangeZoom(Qt::Vertical);

    //part for ECG
    this->ui->widgetECG->xAxis->setTicker(timeTicker);
    this->ui->widgetECG->axisRect()->setupFullAxesBox();
    this->ui->widgetECG->yAxis->setRange(-100,4200);//-100 4200
    this->ui->widgetECG->setInteraction(QCP::iRangeDrag, true);
    this->ui->widgetECG->axisRect()->setRangeDrag(Qt::Horizontal);
    this->ui->widgetECG->setInteraction(QCP::iRangeZoom, true);
    this->ui->widgetECG->axisRect()->setRangeZoom(Qt::Vertical);



    connect(ui->widgetGSR->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetGSR->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->widgetGSR->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetGSR->yAxis2, SLOT(setRange(QCPRange)));

    connect(ui->widgetPneumograph->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetPneumograph->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->widgetPneumograph->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetPneumograph->yAxis2, SLOT(setRange(QCPRange)));

    connect(ui->widgetECG->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetECG->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->widgetECG->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetECG->yAxis2, SLOT(setRange(QCPRange)));

    connect(dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer->start(0);

}

void ChartWindow::realtimeDataSlot() {
    //static QTime time(QTime::currentTime());
    receivePlotData();
    // calculate two new data points:
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
    static double lastPointKey = 0;
    currentPoint = &lastPointKey;
    if (key-lastPointKey > 0.001) // at most add point every 1 ms
    {
      // add data to lines:
      ui->widgetGSR->graph(0)->addData(key, this->dataGSR);
      ui->widgetPneumograph->graph(0)->addData(key, this->dataPNG_2);
      ui->widgetPneumograph->graph(1)->addData(key, this->dataPNG);        //..
      ui->widgetECG->graph(0)->addData(key, this->dataECG);

      lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):

    ui->widgetGSR->xAxis->setRange(key, 15, Qt::AlignRight);
    ui->widgetGSR->graph(0)->rescaleValueAxis(false,true);
    ui->widgetGSR->replot();

    ui->widgetPneumograph->xAxis->setRange(key, 15, Qt::AlignRight);
    //ui->widgetPneumograph->graph()->rescaleValueAxis(false,true);
    //->widgetPneumograph->yAxis->rescale();
    ui->widgetPneumograph->replot();

    ui->widgetECG->xAxis->setRange(key, 15, Qt::AlignRight);
    ui->widgetECG->graph(0)->rescaleValueAxis(false,true);
    ui->widgetECG->replot();

}


void ChartWindow::receivePlotData()
{
    QString store;
    uint16_t finalData;
    uint8_t receivedData[2];
    uint8_t watchdog = 0;
    if(this->device != NULL){

        if(this->device->bytesAvailable() > 11) { //not sure whether "isReadable" is ok instead of readLine...
            while(1) {
                this->device->flush(); // to cokolwiek zmienia?
                this->device->read((char*) receivedData,1); //czyta po 1 bajcie i szuka magicWorda
                if (receivedData[0] == 0x01) {
                    this->device->read((char*) receivedData,2); //czyta po 1 bajcie i szuka magicWorda
                    if(receivedData[0] == 0x02 && receivedData[1] == 0x03) {
                        break; // wyjdz z szukania magic worda, znalazl sie
                    }
                }
                if(watchdog == 20) {
                    this->device->clear();
                    watchdog = 0;
                }
                watchdog++;
            }
            this->device->read((char*) receivedData, 2); // odczytaj 2 bajty danych
            finalData = (receivedData[1] << 8) | receivedData[0]; // sprawdzic czy dobrze dziala
            this -> dataGSR = finalData;
            finalData = 0;

            this->device->read((char*) receivedData, 2); // odczytaj 2 bajty danych
            finalData = (receivedData[1] << 8) | receivedData[0]; // sprawdzic czy dobrze dziala
            this -> dataECG = finalData;
            finalData = 0;

            this->device->read((char*) receivedData, 2); // odczytaj 2 bajty danych
            finalData = (receivedData[1] << 8) | receivedData[0]; // sprawdzic czy dobrze dziala
            this -> dataPNG = finalData;

            this->device->read((char*) receivedData, 2); // odczytaj 2 bajty danych
            finalData = (receivedData[1] << 8) | receivedData[0]; // sprawdzic czy dobrze dziala
            this -> dataPNG_2 = finalData;
        }

            store.append(QString::number((double)time.elapsed()/1000));
            store.push_back(',');
            store.append(QString::number(this->dataGSR));
            store.push_back(',');
            store.append(QString::number(this->dataECG));
            store.push_back(',');
            store.append(QString::number(this->dataPNG));
            store.push_back(',');
            store.append(QString::number(this->dataPNG_2));

            storedData.push_back(store);
    }

}

//chyba pozostalosc z przykladu
uint16_t ChartWindow::testSignal()
{
    double currentTime = time.elapsed()/1000;
   if ((int)currentTime % 40 < 20) {
       return qCos(currentTime)+100;

   } else {
       return qCos(currentTime)+ 10;
   }
}


void ChartWindow::on_pushButtonResume_clicked()
{

    if(dataTimer->isActive()) return;
    if(firstTime) {
        time.start();
        firstTime = false;

    }

    ui->pushButtonClearChart->setStyleSheet("background-color: red");
    this->setupGraphs();

    dataTimer->start(0);
    //this->device->clear(QSerialPort::Direction::Input); //po zakomentowaniu wyeliminowany skok danych na poczatku


}

void ChartWindow::on_pushButtonPause_clicked()
{

    dataTimer->stop();

    ui->pushButtonClearChart->setStyleSheet("background-color: green");

}

void ChartWindow::on_pushButtonClearChart_clicked()
{

    if(!dataTimer->isActive()){

        for (int i = 0; i < markerVect.size(); i++) {
            delete markerVect.at(i);

        }
        *currentPoint = 0;
        time.restart();

        storedData.clear();

        ui->widgetGSR->clearGraphs();
        ui->widgetPneumograph->clearGraphs();
        ui->widgetECG->clearGraphs();

        ui->widgetPneumograph->replot();
        ui->widgetGSR->replot();
        ui->widgetECG->replot();

        markerVect.clear();
        time.restart();
        this->device->clear(QSerialPort::Direction::Input);
        firstTime = true;
    }
}

void ChartWindow::on_pushButton_clicked() // do markerow
{

    QCPItemStraightLine* markerGSR = new QCPItemStraightLine(ui->widgetGSR);
    markerVect.push_back(markerGSR);

    QCPItemStraightLine* markerECG = new QCPItemStraightLine(ui->widgetECG);
    markerVect.push_back(markerECG);

    QCPItemStraightLine* markerPneumograph = new QCPItemStraightLine(ui->widgetPneumograph);
    markerVect.push_back(markerPneumograph);

    markerGSR->setPen(QColor(255,0,0));
    markerECG->setPen(QColor(255,0,0));
    markerPneumograph->setPen(QColor(255,0,0));


    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds

    markerGSR->point1->setCoords(key,0);
    markerGSR->point2->setCoords(key,1);

    markerPneumograph->point1->setCoords(key,0);
    markerPneumograph->point2->setCoords(key,1);

    markerECG->point1->setCoords(key,0);
    markerECG->point2->setCoords(key,1);

    storedData.last().append(",1");
//    storedData.last()[storedData.last().length()-1] = ' ';
//    storedData.last() = storedData.last() + "1\n";


}

void ChartWindow::on_SavetoFile_clicked()
{

    QString a = QFileDialog::getSaveFileName(this,tr("save data"),"Osoba__TestNR_","plik csv(*.csv);;plik txt(*.txt)");
    QFile txtFile(a);

    txtFile.open(QIODevice::WriteOnly);
    //txtFile.open("investigation.txt",std::ios::out);
    if(txtFile.isOpen()) {
        QTextStream out(&txtFile);
        for(int i = 0; i < storedData.length(); i++) {
            out << storedData.at(i) << '\n';
        }

        txtFile.flush();
        txtFile.close();
    } else {
        char data = 0b10100101;
        this->device->write(&data,1);
    }

}

