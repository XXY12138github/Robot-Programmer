#include "serial.h"
#include "ui_serial.h"
#include <QDesktopServices> //用于按钮打开网页
#include <QTimer>
#include <QSettings>
#include <QFile>

#include "dialog.h"
Dialog *v;

unsigned char Reporting_Flag = 0;//正在报告标志
unsigned char Slider_Released_Time = 0;//滑动条松开后计时 (到时间后开始自动更新滑动条位置)

serial::serial(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::serial)
{
    ui->setupUi(this);
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) //检测可用的串口
    {
        QSerialPort serial;

        //定时器初始化--------------------------------------------------------------
        fTimer=new QTimer(this);
        fTimer->stop();
        fTimer->setInterval (50) ;//设置定时周期，单位：毫秒
        connect(fTimer,SIGNAL(timeout()),this,SLOT(on_timer_timeout()));//连接槽函数
        //显示黄绿指示灯------------------------------------------------------------
        ui->G->setPixmap(QPixmap(".\\G.png"));ui->G->setHidden(true);
        ui->Y->setPixmap(QPixmap(".\\Y.png"));ui->Y->setHidden(true);
        //存储参数设置-------------------------------------------------------------

        //创建配置文件
        //QSettings *pIni = new QSettings(QCoreApplication::applicationDirPath()+"/sys.ini", QSettings::IniFormat);
        //写入数据
        //pIni->setValue("/setting/arg1", 1);

        //读取存储参数-------------------------------------------------------------
        //QString sArg = "";
        //sArg = pIni->value("/setting/arg1").toString();


        serial.setPort(info);
        if (serial.open(QIODevice::ReadWrite))
        {
            ui->comboBox->addItem(info.portName() + " " + info.description());// + " " + info.manufacturer());/*不显示厂家信息*/  //加到串口选择下拉框
            serial.close();//关闭串口
        }
    }
    ui->Connect->setText(tr("连接"));

    ui->Open_List->addItems(QStringList()<<tr("Cache")<<tr("List01")<<tr("List02")<<tr("List03")<<tr("List04")<<tr("List05")<<tr("List06")<<tr("List07")<<tr("List08"));
    QVariant v1(0);
    ui->Open_List->setItemData(0,v1,Qt::UserRole - 1);//设置Cache不可用
    ui->Open_List->setCurrentIndex(1);//默认选项

    ui->Save_List->addItems(QStringList()<<tr("Cache")<<tr("List01")<<tr("List02")<<tr("List03")<<tr("List04")<<tr("List05")<<tr("List06")<<tr("List07")<<tr("List08"));
    QVariant v2(0);
    ui->Save_List->setItemData(0,v2,Qt::UserRole - 1);//设置Cache不可用
    ui->Save_List->setCurrentIndex(1);//默认选项

    ui->groupBox_List->setEnabled(false);
    ui->groupBox_Home->setEnabled(false);
    ui->groupBox_Jog->setEnabled(false);
    ui->groupBox_Output->setEnabled(false);
}

serial::~serial()
{
    delete ui;
}

//串口开关-------------------------------------------------------------XXY
void serial::on_Connect_clicked()
{
    if(ui->Connect->text()==tr("连接")){
        my_serialport= new QSerialPort();
      //my_serialport->setPortName(ui->comboBox->currentText());
        my_serialport->setPortName(ui->comboBox->currentText().mid(0,ui->comboBox->currentText().indexOf(" ")));

        my_serialport->open(QIODevice::ReadWrite);

      //my_serialport->setBaudRate(ui->comboBox_2->currentText().toInt());//读取设置框
        my_serialport->setBaudRate(115200);
        my_serialport->setParity(QSerialPort::NoParity);  //N
        my_serialport->setDataBits(QSerialPort::Data8);   //8
        my_serialport->setStopBits(QSerialPort::OneStop); //1
        my_serialport->setFlowControl(QSerialPort::NoFlowControl);

        connect(my_serialport,SIGNAL(readyRead()),this,SLOT(my_readuart()));
        ui->Connect->setText(tr("断开"));

        ui->SendButton->setEnabled(true);//使能发送按钮
        ui->Parameter->setEnabled(true);
        ui->Status->setEnabled(true);
        ui->Unlock->setEnabled(true);
        ui->Set->setEnabled(true);

        ui->groupBox_List->setEnabled(true);
        ui->groupBox_Home->setEnabled(true);
        ui->groupBox_Jog->setEnabled(true);
        ui->groupBox_Output->setEnabled(true);

        //打开串口成功 启动定时器，定时器发送查询下位机命令 @
        fTimer->start();//定时器开始工作
    }
    else {
        //关闭串口 关闭定时器，停止下发查询指令，不然关闭后再发会死机
        fTimer->stop();//定时器停止工作
        ui->Y->setHidden(true);
        ui->G->setHidden(true);

        my_serialport->clear();
        my_serialport->deleteLater();
        ui->Connect->setText(tr("连接"));

        ui->SendButton->setEnabled(false);//禁止发送按钮
        ui->Parameter->setEnabled(false);
        ui->Status->setEnabled(false);
        ui->Unlock->setEnabled(false);
        ui->Set->setEnabled(false);

        ui->groupBox_List->setEnabled(false);
        ui->groupBox_Home->setEnabled(false);
        ui->groupBox_Jog->setEnabled(false);
        ui->groupBox_Output->setEnabled(false);
    }
}

void serial::on_timer_timeout()
{
    //定时器中断响应 50ms
    my_serialport->write("@\r\n");

    if(Slider_Released_Time > 0)Slider_Released_Time --;
}


//接收函数-------------------------------------------------------------XXY
void serial::my_readuart()
{
    while (my_serialport->canReadLine()) {//这里必须这样写，循环读取完所有行，不然数据会堆积，很慢
      QString data = my_serialport->readLine().trimmed();//trimmed：去掉两边的空字符,空格回车换行等。

      // Status response
      if (data[0] == '<')
      {
        if(data.length() > 20){//如果长度大于20，说明是?查询的返回数据，显示在接收框里
            ui->Receive->setText(ui->Receive->toPlainText() + data + "\r\n");//测试用，把数据显示出来
            ui->Receive->moveCursor(QTextCursor::End);//光标移动到最底下，自动下拉
        }
        else if(data[1] == 'X'){
            static QRegExp mpox("X([^/]*)/([^/]*)/([^>]*)");
            if (mpox.indexIn(data) != -1){
                ui->J1_coord->setText("J1:"+mpox.cap(1));
            }
        }
        else if(data[1] == 'Y'){
            static QRegExp mpoy("Y([^/]*)/([^/]*)/([^>]*)");
            if (mpoy.indexIn(data) != -1){
                ui->J2_coord->setText("J2:"+mpoy.cap(1));
            }
        }
        else if(data[1] == 'Z'){
            static QRegExp mpoz("Z([^/]*)/([^/]*)/([^>]*)");
            if (mpoz.indexIn(data) != -1){
                ui->J3_coord->setText("J3:"+mpoz.cap(1));
            }
        }
        else if(data[1] == 'A'){
            static QRegExp mpoa("A([^/]*)/([^/]*)/([^>]*)");
            if (mpoa.indexIn(data) != -1){
                ui->J4_coord->setText("J4:"+mpoa.cap(1));
            }
        }
        else if(data[1] == 'B'){
            static QRegExp mpob("B([^/]*)/([^/]*)/([^>]*)");
            if (mpob.indexIn(data) != -1){
                ui->J5_coord->setText("J5:"+mpob.cap(1));
            }
        }
        else if(data[1] == 'C'){
            static QRegExp mpoc("C([^/]*)/([^/]*)/([^>]*)");
            if (mpoc.indexIn(data) != -1){
                ui->J6_coord->setText("J6:"+mpoc.cap(1));
            }
        }
        else if(data[1] == 'D'){
            static QRegExp mpod("D([^/]*)/([^/]*)/([^>]*)");
            if (mpod.indexIn(data) != -1){
                ui->J7_coord->setText("J7:"+mpod.cap(1));
            }
        }
        else if(data[1] == 'E'){
            static QRegExp mpoe("E([^/]*)/([^/]*)/([^>]*)");
            if (mpoe.indexIn(data) != -1){
                ui->J8_coord->setText("J8:"+mpoe.cap(1));
            }
        }
        else if(data[1] == 'I'){

        }
        else if(data[1] == 'O'){
            static QRegExp mpof("O([^/]*)/S([^/]*)/S([^>]*)");
            if (mpof.indexIn(data) != -1){
                if(mpof.cap(1).toInt() & 0x01)ui->Output1->setChecked(true);else ui->Output1->setChecked(false);
                if(mpof.cap(1).toInt() & 0x02)ui->Output2->setChecked(true);else ui->Output2->setChecked(false);
                if(mpof.cap(1).toInt() & 0x04)ui->Output3->setChecked(true);else ui->Output3->setChecked(false);
                if(mpof.cap(1).toInt() & 0x08)ui->Output4->setChecked(true);else ui->Output4->setChecked(false);
                if(mpof.cap(1).toInt() & 0x10)ui->Output5->setChecked(true);else ui->Output5->setChecked(false);
                if(mpof.cap(1).toInt() & 0x20)ui->Output6->setChecked(true);else ui->Output6->setChecked(false);
                if(mpof.cap(1).toInt() & 0x40)ui->Output7->setChecked(true);else ui->Output7->setChecked(false);
                //if(mpof.cap(1).toInt() & 0x80)ui->Output8->setChecked(true);else ui->Output8->setChecked(false);
                ui->ServoSlider1->setValue(mpof.cap(2).toInt());
                ui->ServoSlider2->setValue(mpof.cap(3).toInt());
            }
            //Servo_Value[0] = serx.cap(1).toInt();
            //释放滑动条时，延时1秒后，才开始自动更新位置
            //if((ui->ServoSlider1->isSliderDown() == false)&&(Slider_Released_Time == 0)){ui->ServoSlider1->setValue(Servo_Value[0]);}

            //Output_Speed[0] = spin.cap(1).toInt();
            //更新打勾状态
            //if(Output_Speed[0] < 150){ui->Output1->setChecked(false);}//关闭 去掉勾
            //else {ui->Output1->setChecked(true);}//开启 打勾
        }
        else if(data[1] == 'F'){
            static QRegExp mpof("F([^/]*)/([^>]*)");
            if (mpof.indexIn(data) != -1){
                ui->label_list_speed->setText("SPEED:F"+mpof.cap(1));
                ui->label_list_delay->setText("DELAY:"+mpof.cap(2)+"MS");
            }
        }
        else if(data[1] == 'L'){
            static QRegExp mpol("L([^/]*)/([^/]*)/([^/]*)/([^>]*)");
            if (mpol.indexIn(data) != -1){
                //ui->Open_List->setCurrentIndex(mpol.cap(1).toInt());
                ui->label_list_steps->setText("STEPS:"+mpol.cap(2)+"/"+mpol.cap(3));
            }
        }

        //交替闪烁黄绿指示灯，指示通信正常。 //注意：调试时图片无法显示，等最终输出可执行文件时，把图片放在exe所在目录就可以了。
        if(ui->G->isHidden() == true){ui->G->setHidden(false);ui->Y->setHidden(true);}else{ui->G->setHidden(true);ui->Y->setHidden(false);}
      }
      else if ((data[0] == 'o')&&(data[1] == 'k'))//屏蔽掉ok显示
      {

      }
      else //显示接收到的数据
      {
        ui->Receive->setText(ui->Receive->toPlainText() + data + "\r\n");//测试用，把数据显示出来
        ui->Receive->moveCursor(QTextCursor::End);//光标移动到最底下，自动下拉
      }
    }
}

//主轴系统----------------------------------------------------------------------------------
void serial::on_Output1_clicked(bool checked)
{
  if(checked == true){my_serialport->write("M18\r\n");}else {my_serialport->write("M19\r\n");}
}
void serial::on_Output2_clicked(bool checked)
{
  if(checked == true){my_serialport->write("M28\r\n");}else {my_serialport->write("M29\r\n");}
}
void serial::on_Output3_clicked(bool checked)
{
  if(checked == true){my_serialport->write("M38\r\n");}else {my_serialport->write("M39\r\n");}
}
void serial::on_Output4_clicked(bool checked)
{
  if(checked == true){my_serialport->write("M48\r\n");}else {my_serialport->write("M49\r\n");}
}
void serial::on_Output5_clicked(bool checked)
{
  if(checked == true){my_serialport->write("M58\r\n");}else {my_serialport->write("M59\r\n");}
}
void serial::on_Output6_clicked(bool checked)
{
  if(checked == true){my_serialport->write("M68\r\n");}else {my_serialport->write("M69\r\n");}
}
void serial::on_Output7_clicked(bool checked)
{
  if(checked == true){my_serialport->write("M78\r\n");}else {my_serialport->write("M79\r\n");}
}
void serial::on_Output8_clicked(bool checked)
{
  //if(checked == true){my_serialport->write("M88\r\n");}else {my_serialport->write("M89\r\n");}
}

//舵机调节----------------------------------------------------------------------------------
void serial::on_ServoSlider1_valueChanged(int value)
{
    my_serialport->write("Q"+QString::number(value +1000).toLatin1()+"\r\n");
    Slider_Released_Time = 10;//用来延时1秒后，在自动更新slider
}
void serial::on_ServoSlider2_valueChanged(int value)
{
    my_serialport->write("Q"+QString::number(value +2000).toLatin1()+"\r\n");
    Slider_Released_Time = 10;//用来延时1秒后，在自动更新slider
}
void serial::on_ServoSlider3_valueChanged(int value)
{
    //my_serialport->write("Q"+QString::number(value +3000).toLatin1()+"\r\n");
    //Slider_Released_Time = 10;//用来延时1秒后，在自动更新slider
}
void serial::on_ServoSlider4_valueChanged(int value)
{
    //my_serialport->write("Q"+QString::number(value +4000).toLatin1()+"\r\n");
    //Slider_Released_Time = 10;//用来延时1秒后，在自动更新slider
}

void serial::on_J1add_clicked(){my_serialport->write("$J=G91X+"+QString::number(ui->J123_step->value()).toLatin1()+"\r\n");}
void serial::on_J1mul_clicked(){my_serialport->write("$J=G91X-"+QString::number(ui->J123_step->value()).toLatin1()+"\r\n");}
void serial::on_J2add_clicked(){my_serialport->write("$J=G91Y+"+QString::number(ui->J123_step->value()).toLatin1()+"\r\n");}
void serial::on_J2mul_clicked(){my_serialport->write("$J=G91Y-"+QString::number(ui->J123_step->value()).toLatin1()+"\r\n");}
void serial::on_J3add_clicked(){my_serialport->write("$J=G91Z+"+QString::number(ui->J123_step->value()).toLatin1()+"\r\n");}
void serial::on_J3mul_clicked(){my_serialport->write("$J=G91Z-"+QString::number(ui->J123_step->value()).toLatin1()+"\r\n");}
void serial::on_J4add_clicked(){my_serialport->write("$J=G91A+"+QString::number(ui->J456_step->value()).toLatin1()+"\r\n");}
void serial::on_J4mul_clicked(){my_serialport->write("$J=G91A-"+QString::number(ui->J456_step->value()).toLatin1()+"\r\n");}
void serial::on_J5add_clicked(){my_serialport->write("$J=G91B+"+QString::number(ui->J456_step->value()).toLatin1()+"\r\n");}
void serial::on_J5mul_clicked(){my_serialport->write("$J=G91B-"+QString::number(ui->J456_step->value()).toLatin1()+"\r\n");}
void serial::on_J6add_clicked(){my_serialport->write("$J=G91C+"+QString::number(ui->J456_step->value()).toLatin1()+"\r\n");}
void serial::on_J6mul_clicked(){my_serialport->write("$J=G91C-"+QString::number(ui->J456_step->value()).toLatin1()+"\r\n");}
void serial::on_J7add_clicked(){my_serialport->write("$J=G91D+"+QString::number(ui->J78_step->value()).toLatin1()+"\r\n");}
void serial::on_J7mul_clicked(){my_serialport->write("$J=G91D-"+QString::number(ui->J78_step->value()).toLatin1()+"\r\n");}
void serial::on_J8add_clicked(){my_serialport->write("$J=G91E+"+QString::number(ui->J78_step->value()).toLatin1()+"\r\n");}
void serial::on_J8mul_clicked(){my_serialport->write("$J=G91E-"+QString::number(ui->J78_step->value()).toLatin1()+"\r\n");}

//发送按钮-------------------------------------------------------------XXY
void serial::on_SendButton_clicked(){
    my_serialport->write(ui->Send->text().toLatin1() + "\r\n");
}
//清空按钮----------------------------------------------
void serial::on_Clear_clicked(){
    ui->Receive->clear(); //清空显示文本框
}
//关于按钮----------------------------------------------
void serial::on_About_clicked(){
    QDesktopServices::openUrl(QUrl(QLatin1String("https://github.com/XXY12138github")));
}

void serial::on_Parameter_clicked(){
    my_serialport->write("$$\r\n");
}
void serial::on_Status_clicked(){
    my_serialport->write("?");
}
void serial::on_Unlock_clicked(){
    my_serialport->write("$X\r\n");
}
void serial::on_Goto0_clicked(){
    my_serialport->write("$GOTO0\r\n");
}
void serial::on_Home_All_clicked(){
    my_serialport->write("$H\r\n");
}
void serial::on_Home_J1_clicked(){
    my_serialport->write("$HX\r\n");
}
void serial::on_Home_J2_clicked(){
    my_serialport->write("$HY\r\n");
}
void serial::on_Home_J3_clicked(){
    my_serialport->write("$HZ\r\n");
}
void serial::on_Home_J4_clicked(){
    my_serialport->write("$HA\r\n");
}
void serial::on_Home_J5_clicked(){
    my_serialport->write("$HB\r\n");
}
void serial::on_Home_J6_clicked(){
    my_serialport->write("$HC\r\n");
}
void serial::on_Home_J7_clicked(){
    my_serialport->write("$HD\r\n");
}
void serial::on_Home_J8_clicked(){
    my_serialport->write("$HE\r\n");
}

void serial::on_Open_clicked(){
    my_serialport->write("$LIST" + QString::number(ui->Open_List->currentIndex()).toLatin1() + "\r\n");
}
void serial::on_Save_clicked(){
    my_serialport->write("$SAVE" + QString::number(ui->Save_List->currentIndex()).toLatin1() + "\r\n");
}
void serial::on_List_Clear_clicked(){
    my_serialport->write("$CLEAR\r\n");
}
void serial::on_Run_clicked(){
    my_serialport->write("$RUN\r\n");
}
void serial::on_Add_clicked(){
    my_serialport->write("$ADD\r\n");
}
void serial::on_Delete_clicked(){
    my_serialport->write("$DEL\r\n");
}
void serial::on_Update_clicked(){
    my_serialport->write("$UPD\r\n");
}
void serial::on_Reset_clicked(){
    char reset_command[2] = {0x18,'\0'}; //复位命令，实际发送时，只会发出第一个字符，第二个相当于字符串结尾
    my_serialport->write(reset_command);
}

void serial::on_Step_Mul_clicked(){
    my_serialport->write("$BACK\r\n");
}
void serial::on_Step_Add_clicked(){
    my_serialport->write("$NEXT\r\n");
}
void serial::on_Delay_Mul_clicked(){
    my_serialport->write("$TIME-10\r\n");//下位机收到后会乘以10
}
void serial::on_Delay_Add_clicked(){
    my_serialport->write("$TIME+10\r\n");//下位机收到后会乘以10
}
void serial::on_Speed_Mul_clicked(){
    my_serialport->write("$SPEED-1\r\n");//下位机收到后会乘以100
}
void serial::on_Speed_Add_clicked(){
    my_serialport->write("$SPEED+1\r\n");//下位机收到后会乘以100
}



void serial::on_Set_clicked()
{
    v =new Dialog();//打开新窗口

    v->setWindowModality(Qt::ApplicationModal);//子窗口弹出时，父窗口不能操作

    v->setWindowTitle("Settings");
    v->setWindowIcon(QIcon(":/images/mc_robot_programmer_ico.ico"));
    v->setFixedSize(800,400); //固定窗口大小
    v->show();
}
