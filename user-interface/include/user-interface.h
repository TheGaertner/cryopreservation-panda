#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTableView>

#include <cstdlib>

#include <boost/asio.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/algorithm/string.hpp>

#include "serialization.h"
#include "config_handler.h"

#include <Eigen/Dense>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>


#include "opencv2/opencv.hpp"

#include <cpp_utils/network.hpp>

#include <QGraphicsPixmapItem>
#include <QThread>

#include <videostream.h>
#include <tcp_command.h>
#include <marker.h>

#include <franka/model.h>


QT_BEGIN_NAMESPACE
namespace Ui { class UserInterface; }
QT_END_NAMESPACE



class UserInterface : public QMainWindow
{
    Q_OBJECT

signals:
    void tcp_connect( QString ip);
    void init_tcp();
    void tcp_autorecovery();
    void go_to_init();
    void tcp_custome_command(QString command);
    void tcp_abs_pose(QString command);

public:
    UserInterface(QWidget *parent = nullptr, QString  command="");

    QGraphicsPixmapItem pixmap_0;
    QGraphicsPixmapItem pixmap_1;
    ~UserInterface();


public slots:
    void processPendingDatagrams();
    void set_display_0(QPixmap item);
    void set_display_1(QPixmap item);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_lineEdit_returnPressed();

    void on_comboBox_activated(const QString &arg1);

    void on_comboBox_2_activated(int index);

    void on_comboBox_3_activated(const QString &arg1);

    void on_comboBox_4_activated(const QString &arg1);

    void on_comboBox_5_activated(const QString &arg1);

    void on_comboBox_6_activated(const QString &arg1);

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();


    void on_pushButton_14_clicked();

    void on_pushButton_15_clicked();

    void on_lineEdit_7_returnPressed();

    void on_lineEdit_8_returnPressed();

    void on_lineEdit_2_returnPressed();

    void on_pushButton_18_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_21_clicked();

private:
    Ui::UserInterface *ui;
    QUdpSocket *socket_udp_ = nullptr;

    StateSerialization last_state_;

    Videostream *videostream_0;
    Videostream *videostream_1;
    Tcp_command *tcp_command;
    QThread* thread;

    std::chrono::microseconds last_time;
    std::deque <double> frequency_buffer;
    QVector<double> graph_buffer_y;
    QVector<double> graph_buffer_x;
    double counter = 0;
    int save_image_counter_ = 0;
    QString command_;
    Marker marker_;

};
#endif // USER_INTERFACE_H
