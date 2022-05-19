#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serialport = new QSerialPort(this);
    timer = new QTimer(this);
    //更新时间
    timer->start(1000);
    connect(timer,&QTimer::timeout,this,&MainWindow::showTime);
    ShowTimeLabel = new QLabel(this);
    scanSerialPort();
   // serialPortInit();
    //接收数据
    connect(serialport,&QSerialPort::readyRead,this,[=]()
    {
        qDebug() << "接收数据" << ShowTimeLabel->text() << endl;
        if(serialport->isReadable())
        {
            //取出读出的数据
            QByteArray temp = serialport->readAll();
            QString rx_data = QString::fromLocal8Bit(temp);
            //取出当前文本里的数据
            QString str = ui->RecvEdit->toPlainText();
            str.append("\n");
            str.append(QString("[%1]RX: %2").arg(ShowTimeLabel->text()).arg(rx_data));
            ui->RecvEdit->setText(str);
            ui->RecvEdit->setReadOnly(true);
        }
    }
    );

    //编辑区TapPage设置
    ui->tabWidget->setTabText(0,"单条发送");
    ui->tabWidget->setTabText(1,"多条发送");
    ui->tabWidget->setTabText(2,"协议传输");
    ui->tabWidget->setTabText(3,"帮助");

}

void MainWindow::showTime()
{
    //显示当前时间
    QDateTime CurrentTime = QDateTime::currentDateTime();
    QString strTime = CurrentTime.toString(" 当前时间：yyyy年MM月dd日 hh:mm:ss "); //设置显示的格式
    ShowTimeLabel->setText(strTime);
    statusBar()->addPermanentWidget(ShowTimeLabel);
    statusBar()->setSizeGripEnabled(true); //设置是否显示右边的大小控制点
}

MainWindow::~MainWindow()
{
    delete ui;
    delete serialport;
    serialport = NULL;
    delete timer;
    timer = NULL;
}

void MainWindow::scanSerialPort()
{
    //查询可用的串口信息
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {

        ui->serialportBox->addItem(info.portName());
        qDebug() << "Description" << info.description() << endl;
    }
     qDebug()<<"已发现串口："<<ui->serialportBox->currentText();
     qDebug()<<"串口数量：" << QSerialPortInfo::availablePorts().count();
}

void MainWindow::serialPortInit()
{
    //设置串口名称
    serialport->setPortName(ui->serialportBox->currentText());
    qDebug()<<"串口名称:"<<serialport->portName();

    //设置波特率
    serialport->setBaudRate(ui->baudRateBox->currentText().toInt());
    qDebug()<<"波特率:"<<ui->baudRateBox->currentText().toInt();

    //设置停止位
    switch (ui->stopBitBox->currentText().toInt())
    {
    case 1 :  serialport->setStopBits(QSerialPort::OneStop); break;
    case 2 :  serialport->setStopBits(QSerialPort::TwoStop); break;
    default: break;
    }
    qDebug()<<"停止位:"<<serialport->stopBits();
    //设置数据位
    switch (ui->dataBitsBox->currentText().toInt())
    {
    case 5 :  serialport->setDataBits(QSerialPort::Data5); break;
    case 6 :  serialport->setDataBits(QSerialPort::Data6); break;
    case 7 :  serialport->setDataBits(QSerialPort::Data7); break;
    case 8 :  serialport->setDataBits(QSerialPort::Data8); break;
    default: break;
    }
    qDebug() << "数据位："<< ui->dataBitsBox->currentText();
    //设置奇偶位
    switch (ui->parityBox->currentIndex())
    {
    case 0 :  serialport->setParity(QSerialPort::NoParity); break;
    case 1 :  serialport->setParity(QSerialPort::OddParity); break;
    case 2 :  serialport->setParity(QSerialPort::EvenParity); break;
    default: break;
    }
    qDebug()<<"奇偶位:"<<serialport->parity();

    serialport->setFlowControl(QSerialPort::NoFlowControl) ;

    /* 打开串口提示框 */
      if (true == serialport->open(QIODevice::ReadWrite))
      {
          QMessageBox::information(this, "提示", "串口打开成功");
      }
      else
      {
          QMessageBox::critical(this, "提示", "串口打开失败");
      }

}

void MainWindow::on_openButton_clicked()
{
    if (btn_on_off == 0) //串口状态：开
    {
        //串口开时，不允许修改参数
        ui->serialportBox->setEnabled(false);
        ui->baudRateBox->setEnabled(false);
        ui->dataBitsBox->setEnabled(false);
        ui->stopBitBox->setEnabled(false);
        ui->parityBox->setEnabled(false);
        ui->openButton->setText("关闭串口");
        qDebug()<<"打开串口:";
        serialPortInit();
    }

    if (btn_on_off == 1)  //串口状态：关
    {

        serialport->close();
        ui->serialportBox->setEnabled(true);
        ui->baudRateBox->setEnabled(true);
        ui->dataBitsBox->setEnabled(true);
        ui->stopBitBox->setEnabled(true);
        ui->parityBox->setEnabled(true);
        ui->openButton->setText("打开串口");
    }
    btn_on_off = !btn_on_off;
}
//发送数据
void MainWindow::on_sendButton_clicked()
{
    QByteArray tx_data;
    QString rx_data;
    QString show_data;
    //判断是否非空
    if(!ui->sendEdit->toPlainText().isEmpty())
    {
        tx_data = ui->sendEdit->toPlainText().toLocal8Bit();//用于传输TX数据
        rx_data = ui->sendEdit->toPlainText();//用于显示RX数据
        show_data = ui->RecvEdit->toPlainText();
        show_data.append("\n");
        show_data += QString("[%1]TX: %2").arg(ShowTimeLabel->text()).arg(rx_data);
        ui->RecvEdit->setText(show_data);
        serialport->write(tx_data);

    }
}

void MainWindow::on_clearSedButton_clicked()
{
    ui->sendEdit->clear();
}

void MainWindow::on_clearRecvButton_clicked()
{
    ui->RecvEdit->clear();
}

//保存窗口内容到文件
void MainWindow::on_clearRecvButton_2_clicked()
{
    QString recv_info = ui->RecvEdit->toPlainText();
    QByteArray recv_data = recv_info.toLocal8Bit();
    recv_save(recv_data.data(), recv_data.length());
}
//保存窗口
void MainWindow::recv_save(char *msg, quint32 len)
{
    QString filename = QFileDialog::getSaveFileName(this,
        tr("Save File"),
        "",//当前目录
        tr("Text files (*.txt)")); //选择路径
    if(filename.isEmpty())
    {
        QMessageBox::information(this,"提示","保存失败");
        return;
    }
    else
    {
        QFile file(filename);
        file.open(QIODevice::ReadWrite|QIODevice::Text);
        file.write((const char *)msg,len);
        file.close();
        QMessageBox::critical(this,"提示","保存成功");
    }
}

