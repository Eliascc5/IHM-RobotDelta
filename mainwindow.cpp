#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "doubleSprofile.h"
#include "GlobalFunc.h"

#define nbPointsTime 100

//https://www.youtube.com/watch?v=jrLNwrOf7wM&ab_channel=Sigmoid

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Adding title for widget
    QMainWindow::setWindowTitle("IHM Robot delta");

    // Ports
    QList<QSerialPortInfo> ports = info.availablePorts();
    QList<QString> stringPorts;
    for(int i = 0 ; i < ports.size() ; i++){
        stringPorts.append(ports.at(i).portName());
    }
    ui->comboBox_port->addItems(stringPorts);

    // Baud Rate Ratios
    QList<qint32> baudRates = info.standardBaudRates();
    QList<QString> stringBaudRates;
    for(int i = 0 ; i < baudRates.size() ; i++){
        stringBaudRates.append(QString::number(baudRates.at(i)));
    }
    ui->comboBox_baud->addItems(stringBaudRates);

    /*Configuration of QCustomPlot*/
    //Styles of graphs

    this->ui->velProfilePlot->setBackground(QBrush(QColor(104, 130, 170)));


    QPen pen1;
    pen1.setWidth(3);
    pen1.setColor(QColor(255,0,0));
    QPen pen2;
    pen2.setWidth(3);
    pen2.setColor(QColor(0,255,0));

    QPen pen3;
    pen3.setWidth(3);
    pen3.setColor(QColor(0,0,255));
    QPen pen4;
    pen4.setWidth(3);
    pen4.setColor(QColor(0,0,0));

    //Create Rectangle Area and add to QCustomplot
    QCPAxisRect * R00=new QCPAxisRect(this->ui->velProfilePlot);
    QCPAxisRect * R10=new QCPAxisRect(this->ui->velProfilePlot);
    QCPAxisRect * R20=new QCPAxisRect(this->ui->velProfilePlot);
    QCPAxisRect * R30=new QCPAxisRect(this->ui->velProfilePlot);

    this->ui->velProfilePlot->plotLayout()->clear();
    this->ui->velProfilePlot->plotLayout()->addElement(0,0,R00);
    this->ui->velProfilePlot->plotLayout()->addElement(1,0,R10);
    this->ui->velProfilePlot->plotLayout()->addElement(2,0,R20);
    this->ui->velProfilePlot->plotLayout()->addElement(3,0,R30);


    //Draw graphs with data
    this->ui->velProfilePlot->addGraph(R00->axis(QCPAxis::atBottom),R00->axis(QCPAxis::atLeft));
    this->ui->velProfilePlot->addGraph(R10->axis(QCPAxis::atBottom),R10->axis(QCPAxis::atLeft));
    this->ui->velProfilePlot->addGraph(R20->axis(QCPAxis::atBottom),R20->axis(QCPAxis::atLeft));
    this->ui->velProfilePlot->addGraph(R30->axis(QCPAxis::atBottom),R30->axis(QCPAxis::atLeft));

    this->ui->velProfilePlot->graph(0)->setName("Position");
    this->ui->velProfilePlot->graph(1)->setName("Velocity");
    this->ui->velProfilePlot->graph(2)->setName("Aceleration");
    this->ui->velProfilePlot->graph(3)->setName("Jerk");

    this->ui->velProfilePlot->graph(0)->setPen(pen1);
    this->ui->velProfilePlot->graph(1)->setPen(pen2);
    this->ui->velProfilePlot->graph(2)->setPen(pen3);
    this->ui->velProfilePlot->graph(3)->setPen(pen4);

    //Add Legends
    QCPLegend *arLegend00=new QCPLegend;
    R00->insetLayout()->addElement(arLegend00,Qt::AlignTop|Qt::AlignRight);
    arLegend00->setBrush(QColor(255, 255, 255, 100));
    arLegend00->setBorderPen(Qt::NoPen);
    arLegend00->setLayer("legend");
    arLegend00->addItem(new QCPPlottableLegendItem(arLegend00, this->ui->velProfilePlot->graph(0)));

    QCPLegend *arLegend10=new QCPLegend;
    R10->insetLayout()->addElement(arLegend10,Qt::AlignTop|Qt::AlignRight);
    arLegend10->setBrush(QColor(255, 255, 255, 100));
    arLegend10->setBorderPen(Qt::NoPen);
    arLegend10->setLayer("legend");
    arLegend10->addItem(new QCPPlottableLegendItem(arLegend10, this->ui->velProfilePlot->graph(1)));

    QCPLegend *arLegend20=new QCPLegend;
    R20->insetLayout()->addElement(arLegend20,Qt::AlignTop|Qt::AlignRight);
    arLegend20->setBrush(QColor(255, 255, 255, 100));
    arLegend20->setBorderPen(Qt::NoPen);
    arLegend20->setLayer("legend");
    arLegend20->addItem(new QCPPlottableLegendItem(arLegend20, this->ui->velProfilePlot->graph(2)));

    QCPLegend *arLegend30=new QCPLegend;
    R30->insetLayout()->addElement(arLegend30,Qt::AlignTop|Qt::AlignRight);
    arLegend30->setBrush(QColor(255, 255, 255, 100));
    arLegend30->setBorderPen(Qt::NoPen);
    arLegend30->setLayer("legend");
    arLegend30->addItem(new QCPPlottableLegendItem(arLegend30, this->ui->velProfilePlot->graph(3)));

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_connectButton_clicked()
{

    QString portName = ui->comboBox_port->currentText();
    serialPort.setPortName(portName);

    serialPort.open(QIODevice::ReadWrite);

    if(!serialPort.isOpen()){
        ui->textBrowser->setTextColor(Qt::red);
        ui->textBrowser->append("Something went wrong, try again!");
    }
    else {
        ui->textBrowser->setTextColor(Qt::green);
        ui->textBrowser->append("Connected!");


        QString stringbaudRate = ui->comboBox_baud->currentText();
        int intbaudRate = stringbaudRate.toInt();
        serialPort.setBaudRate(intbaudRate);

        serialPort.setDataBits(QSerialPort::Data8);
        serialPort.setParity(QSerialPort::NoParity);
        serialPort.setStopBits(QSerialPort::OneStop);
        serialPort.setFlowControl(QSerialPort::NoFlowControl);


        code = "\n";
        codeSize = code.size();

        connect(&serialPort,SIGNAL(readyRead()),this,SLOT(receiveMessage()));
        }


}

void MainWindow::receiveMessage()
{
    QByteArray dataBA = serialPort.readAll();
    QString data(dataBA);
    buffer.append(data);
    int index = buffer.indexOf(code);
    if(index != -1){
       QString message = buffer.mid(0,index);
       ui->textBrowser->setTextColor(Qt::blue); // Receieved message's color is blue.
       ui->textBrowser->append(message);
       buffer.remove(0,index+codeSize);
    }


}


void MainWindow::on_sendTrameButton_clicked()
{
    QString msg = ui->lineEdit_Trame->text()+13; //13 en ascii es CR
    ui->textBrowser->setTextColor(Qt::darkGreen); // Color of message to send is green.
    ui->textBrowser->append(msg);
    qDebug()<<"---------";

    serialPort.write(msg.toUtf8());
}



void MainWindow::on_clearTextBrowserButton_clicked()
{
     ui->textBrowser->clear();
}





void MainWindow::on_homingButton_clicked()
{
    if(!serialPort.isOpen()){
        //TODO: AGREGAR QMESSAGEBOX CON WARNING
        QMessageBox msgBox;
        msgBox.setText("The serial port is NOT connected!");
        msgBox.exec();
    }
    else {
       QString message = ":h\r";
       serialPort.write(message.toUtf8());
       FlagHoming = true;


    }
}


void MainWindow::on_xpButton_clicked()
{
    if(FlagHoming){

        //TODO: ENVIAR TRAMA PARA MOVERSE EN X+

        QString aux1{":px"};
        QString step{ui->lineEdit_step->text()};
        QString aux2 ={"\r"};

        QString command = aux1+step+aux2;

        qDebug()<<"aca abajo:\n";
        qDebug()<<command;
        serialPort.write(command.toUtf8());
    }
    else{

        QMessageBox msgBox;
        msgBox.setText("First, you have to do a homing!");
        msgBox.exec();
    }
}


void MainWindow::on_xnButton_clicked()
{
    if(FlagHoming){
        //TODO: ENVIAR TRAMA PARA MOVERSE EN X-
        QString aux1{":px"};
        QString step{"-" + ui->lineEdit_step->text()};
        QString aux2 ={"\r"};

        QString command = aux1+step+aux2;

        qDebug()<<"aca abajo:\n";
        qDebug()<<command;
        serialPort.write(command.toUtf8());
    }
    else{

        QMessageBox msgBox;
        msgBox.setText("First, you have to do a homing!");
        msgBox.exec();
    }

}


void MainWindow::on_ypButton_clicked()
{
    if(FlagHoming){
        //TODO: ENVIAR TRAMA PARA MOVERSE EN Y+
        QString aux1{":py"};
        QString step{ui->lineEdit_step->text()};
        QString aux2 ={"\r"};

        QString command = aux1+step+aux2;

        qDebug()<<"aca abajo:\n";
        qDebug()<<command;
        serialPort.write(command.toUtf8());
    }
    else{

        QMessageBox msgBox;
        msgBox.setText("First, you have to do a homing!");
        msgBox.exec();
    }
}


void MainWindow::on_ynButton_clicked()
{
    if(FlagHoming){
        //TODO: ENVIAR TRAMA PARA MOVERSE EN Y-

        QString aux1{":py"};
        QString step{"-" + ui->lineEdit_step->text()};
        QString aux2 ={"\r"};

        QString command = aux1+step+aux2;

        qDebug()<<"aca abajo:\n";
        qDebug()<<command;
        serialPort.write(command.toUtf8());
    }
    else{

        QMessageBox msgBox;
        msgBox.setText("First, you have to do a homing!");
        msgBox.exec();
    }
}


void MainWindow::on_zpButton_clicked()
{
    if(FlagHoming){
        //TODO: ENVIAR TRAMA PARA MOVERSE EN Z+
        QString aux1{":pz"};
        QString step{ui->lineEdit_step->text()};
        QString aux2 ={"\r"};

        QString command = aux1+step+aux2;

        qDebug()<<"aca abajo:\n";
        qDebug()<<command;
        serialPort.write(command.toUtf8());
    }
    else{

        QMessageBox msgBox;
        msgBox.setText("First, you have to do a homing!");
        msgBox.exec();
    }
}


void MainWindow::on_znButton_clicked()
{
    if(FlagHoming){
        //TODO: ENVIAR TRAMA PARA MOVERSE EN Z-
        QString aux1{":pz"};
        QString step{"-" + ui->lineEdit_step->text()};
        QString aux2 ={"\r"};

        QString command = aux1+step+aux2;

        qDebug()<<"aca abajo:\n";
        qDebug()<<command;
        serialPort.write(command.toUtf8());
    }
    else{

        QMessageBox msgBox;
        msgBox.setText("First, you have to do a homing!");
        msgBox.exec();
    }
}


void MainWindow::on_plotButton_clicked()
{
    //Settable parameters in term of microcontroller

    vmax = (ui->lineEdit_vm->text()).toFloat();
    vi = (ui->lineEdit_vi->text()).toFloat();
    vf = (ui->lineEdit_vf->text()).toFloat();
    amax = (ui->lineEdit_am->text()).toFloat();
    jmax = (ui->lineEdit_jm->text()).toFloat();
    //NON Settable parameters in term of microcontroller
    qf=(ui->lineEdit_qf->text()).toFloat();
    qi=(ui->lineEdit_qi->text()).toFloat();

    float *arrayT = update_ScurveTraj( qf , qi,  vi, vf , vmax, amax, jmax);

    for (int i=0;i<6;i++){
        qDebug()<<arrayT[i];
    }

    qDebug()<<"El valor de T es: "<<arrayT[5]<<"\n";

    float *time = linspace(0, arrayT[5], nbPointsTime);

    for (int i=0;i<nbPointsTime;i++){

        qDebug()<<"time: "<<time[i];
    }

    qDebug()<<"--------";

    float *arrayCoord;
    float array_q[nbPointsTime],array_qd[nbPointsTime],array_qdd[nbPointsTime],array_qddd[nbPointsTime];


    for (int i=0;i<nbPointsTime;i++){

         arrayCoord = get_Straj(time[i]);
         array_q[i] = arrayCoord[0];
         array_qd[i] = arrayCoord[1];
         array_qdd[i] = arrayCoord[2];
         array_qddd[i] = arrayCoord[3];
        /*
        for (int j=0;j<3;j++){
            qDebug()<<arrayCoord[j];

        }
        qDebug()<<"......";
        */

    }

    QVector<double> x(nbPointsTime), y(nbPointsTime),y2(nbPointsTime),
            y3(nbPointsTime),y4(nbPointsTime);

    for (int i=0; i<nbPointsTime; i++)
    {
      x[i] = time[i];
      y[i] = array_q[i];
      y2[i] = array_qd[i];
      y3[i] = array_qdd[i];
      y4[i] = array_qddd[i];
    }


    this->ui->velProfilePlot->graph(0)->setData(x,y);
    this->ui->velProfilePlot->graph(0)->rescaleAxes();

    this->ui->velProfilePlot->graph(1)->setData(x,y2);
    this->ui->velProfilePlot->graph(1)->rescaleAxes();

    this->ui->velProfilePlot->graph(2)->setData(x,y3);
    this->ui->velProfilePlot->graph(2)->rescaleAxes();

    this->ui->velProfilePlot->graph(3)->setData(x,y4);
    this->ui->velProfilePlot->graph(3)->rescaleAxes();




    this->ui->velProfilePlot->replot();

    free(time);
    free(arrayT);
    free(arrayCoord);
}


void MainWindow::on_applyParamButton_clicked()
{

    QString vm_s = (ui->lineEdit_vm->text());
    QString vi_s = (ui->lineEdit_vi->text());
    QString vf_s = (ui->lineEdit_vf->text());
    QString am_s = (ui->lineEdit_am->text());
    QString jm_s = (ui->lineEdit_jm->text());


    QString vm_a{":vm"};
    QString vf_a{" vf"};
    QString vi_a{" vi"};
    QString am_a{" am"};
    QString jm_a{" jm"};

    QString rc ={"\r"};

    QString trame = vm_a+vm_s+vf_a+vf_s+vi_a+vi_s+am_a+am_s+jm_a+jm_s+rc;
    qDebug()<<trame;

    serialPort.write(trame.toUtf8());



}

