#include "newdevice.h"
#include "ui_newdevice.h"

NewDevice::NewDevice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewDevice)
{
    ui->setupUi(this);
}

NewDevice::~NewDevice()
{
    delete ui;
}

void NewDevice::on_pushButton_clicked()
{
    //emit device_created(ui->lineEdit->text());
    close();
}
