#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QList>
#include "chartwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->device = new QSerialPort(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButtonSearch_clicked()
{
    QList <QSerialPortInfo> devices;
    devices = QSerialPortInfo::availablePorts();
    ui->comboBoxPorts->clear();

    for(int i =0; i < devices.size(); i++) {

        ui->comboBoxPorts->addItem(devices.at(i).portName());

    }
}


void MainWindow::on_pushButtonConnect_clicked()
{
    if(ui->comboBoxPorts->count() == 0) {
        sendLogMessage("there are no devices ready to connect");
        return;
    }

    QString portName = ui->comboBoxPorts->currentText();
    this->device->setPortName(portName);

    if(!this->device->isOpen()) {
        if(this->device->open(QIODevice::ReadWrite)){
            this->device->setBaudRate(QSerialPort::Baud115200);
            this->device->setParity(QSerialPort::NoParity);
            this->device->setDataBits(QSerialPort::Data8);
            this->device->setStopBits(QSerialPort::OneStop);
            this->device->setFlowControl(QSerialPort::NoFlowControl);

            sendLogMessage("Serial port has been opened with settings::"
                           "Baudrate:115200, NP, D8, 1S");


        } else {

            sendLogMessage("The serial port was unreachable");
        }
    } else {

        sendLogMessage("Currently there is other port opened");

    }
}

void MainWindow::sendLogMessage(QString Message)
{
    ui->textEditLog->clear();
    ui->textEditLog->append(Message + "\r");
}


void MainWindow::on_pushButtonDisconnect_clicked()
{
    if(this->device->isOpen()) {

        this->device->close();
        sendLogMessage("Port has been closed");
    } else {
        sendLogMessage("Currently there are no ports to be closed");
    }
}


void MainWindow::on_pushButtonTryConnection_clicked()
{
    if(this->device->isOpen() && this->device->isWritable()) {
        char signal = 0b10100101;
        this->device->write(&signal,1);

    } else {
        sendLogMessage("Device not connected or not writable");
    }
}


void MainWindow::on_pushButtonRealTimeChart_clicked()
{
    this->window()->close();

    this->secondWindow = new ChartWindow(this,this->device);
    this->secondWindow ->show();
}

