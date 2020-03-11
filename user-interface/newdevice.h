#ifndef NEWDEVICE_H
#define NEWDEVICE_H

#include <QDialog>

namespace Ui {
class NewDevice;
}

class NewDevice : public QDialog
{
    Q_OBJECT

signals:
    void device_created(QString name);


public:
    explicit NewDevice(QWidget *parent = nullptr);
    ~NewDevice();

private slots:
    void on_pushButton_clicked();

private:
    Ui::NewDevice *ui;
};

#endif // NEWDEVICE_H
