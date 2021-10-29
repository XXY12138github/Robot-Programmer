#ifndef SERIAL_H
#define SERIAL_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class serial;
}

class serial : public QWidget
{
    Q_OBJECT

public:
    explicit serial(QWidget *parent = 0);
    ~serial();

private:
    Ui::serial *ui;
    QSerialPort *my_serialport;

    QTimer *fTimer; //--------------------------------定义一个定时器

private slots:

    void on_timer_timeout(); //-----------------------定时溢出处理槽函数

    void my_readuart();//串口接收数据槽函数
    void on_Connect_clicked();

    void on_Output1_clicked(bool checked);
    void on_Output2_clicked(bool checked);
    void on_Output3_clicked(bool checked);
    void on_Output4_clicked(bool checked);
    void on_Output5_clicked(bool checked);
    void on_Output6_clicked(bool checked);
    void on_Output7_clicked(bool checked);
    void on_Output8_clicked(bool checked);

    void on_ServoSlider1_valueChanged(int value);
    void on_ServoSlider2_valueChanged(int value);
    void on_ServoSlider3_valueChanged(int value);
    void on_ServoSlider4_valueChanged(int value);

    void on_J1add_clicked();
    void on_J1mul_clicked();
    void on_J2add_clicked();
    void on_J2mul_clicked();
    void on_J3add_clicked();
    void on_J3mul_clicked();
    void on_J4add_clicked();
    void on_J4mul_clicked();
    void on_J5add_clicked();
    void on_J5mul_clicked();
    void on_J6add_clicked();
    void on_J6mul_clicked();
    void on_J7add_clicked();
    void on_J7mul_clicked();
    void on_J8add_clicked();
    void on_J8mul_clicked();

    void on_SendButton_clicked();
    void on_Clear_clicked();
    void on_About_clicked();
    void on_Parameter_clicked();
    void on_Status_clicked();
    void on_Unlock_clicked();
    void on_Goto0_clicked();
    void on_Home_All_clicked();
    void on_Home_J1_clicked();
    void on_Home_J2_clicked();
    void on_Home_J3_clicked();
    void on_Home_J4_clicked();
    void on_Home_J5_clicked();
    void on_Home_J6_clicked();
    void on_Home_J7_clicked();
    void on_Home_J8_clicked();

    void on_Open_clicked();
    void on_Save_clicked();
    void on_List_Clear_clicked();
    void on_Run_clicked();
    void on_Add_clicked();
    void on_Delete_clicked();
    void on_Update_clicked();
    void on_Reset_clicked();
    void on_Step_Mul_clicked();
    void on_Step_Add_clicked();
    void on_Delay_Mul_clicked();
    void on_Delay_Add_clicked();
    void on_Speed_Mul_clicked();
    void on_Speed_Add_clicked();

    void on_Set_clicked();
};

#endif // SERIAL_H
