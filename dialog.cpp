#include "dialog.h"
#include "ui_dialog.h"
#include <QStandardItemModel>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    //ui->label->setPixmap(QPixmap(".\\help.bmp"));

    /*
    //提供了用于存储自定义数据的通用模型
    QStandardItemModel *model = new QStandardItemModel(0,3);
    model->setHeaderData(0,Qt::Horizontal,tr("#"));
    model->setHeaderData(1,Qt::Horizontal,tr("值"));
    model->setHeaderData(2,Qt::Horizontal,tr("描述"));

    //插入数据
    model->insertRows(0,2); //插入两行
    model->setData(model->index(0,0), "$0"); //插入第一列
    model->setData(model->index(0,1), "10");
    model->setData(model->index(0,2), "step pulse, usec");

    model->setData(model->index(1,0), "$1"); //插入第一列
    model->setData(model->index(1,1), "250");
    model->setData(model->index(1,2), "step idle delay, msec");

    //ui->tableView->horizontalHeader()->setVisible(true);//显示或隐藏表头
    //ui->tableView->verticalHeader()->setVisible(false);//显示或隐藏序列行

    //ui->tableView->setAutoScroll(true);//自动滚动条
    //ui->tableView->resizeColumnsToContents(); //根据内容调整大小
    //ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//根据容器调整大小
    //ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//只读
    //ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选中模式为选中行
    //ui->tableView->setSelectionMode( QAbstractItemView::SingleSelection); //设置选中单行

    ui->tableView->setModel(model); //使用

    ui->tableView->setColumnWidth(0,40);//列宽设置，必须在setModel之后，否则不起作用
    ui->tableView->setColumnWidth(1,80);
    ui->tableView->setColumnWidth(2,600);
    */

}

Dialog::~Dialog()
{
    delete ui;
}
