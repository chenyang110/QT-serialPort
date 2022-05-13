#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//模仿正点原子的XCOM串口软件
#include <QMainWindow>
#include <QSerialPort>			//访问串口的库
#include <QSerialPortInfo>		//查询串口信息的库
#include <QDebug>				//用于调试打印输出的库
#include <QTimer>				//定时器的库
#include <QDateTime>                        //日期的库
#include <QMessageBox>			//一个小的弹窗库
#include <QLabel>                       //显示时间的文本栏

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void showTime();
    int btn_on_off = 0;//串口状态：开
    int btn_state = 0;

private slots:
    void on_openButton_clicked();

    void on_sendButton_clicked();

private:
    Ui::MainWindow *ui;
    //定时器
    QTimer *timer;
    //显示时间
    QLabel *ShowTimeLabel;
    // 串口对象
    QSerialPort *serialport;
    //扫描串口
    void scanSerialPort();
    //初始化
    void serialPortInit();
};

#endif // MAINWINDOW_H
