#include "serial.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    serial w;

    w.setWindowTitle("Robot Programmer V2.1.0");
    w.setWindowIcon(QIcon(":/images/mc_robot_programmer_ico.ico"));
    w.setFixedSize(920,460); //固定窗口大小
    w.show();

    return a.exec();
}


