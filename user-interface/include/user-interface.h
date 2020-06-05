#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTableView>
#include <QString>
#include <QListWidgetItem>
#include <QTableWidget>

#include <cstdlib>

#include <boost/asio.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/algorithm/string.hpp>

#include <Eigen/Dense>

#include <opencv2/core.hpp>

#include <QGraphicsPixmapItem>
#include <QThread>

#include "cpp_utils/network.hpp"
#include "serialization.h"
#include "config_handler.h"
#include "cameracalibration.h"
#include "videostream.h"
#include "tcp_command.h"
#include "marker.h"
// #include "newdevice.h"
#include "skillhandlerinterface.h"




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
    void tcp_adresse_changed(QString adresse);
    void current_robot_state(StateSerialization *state);

public:
    UserInterface(QWidget *parent = nullptr, QString  command="");

    QGraphicsPixmapItem pixmap_0;
    QGraphicsPixmapItem pixmap_1;
    ~UserInterface();


public slots:
    void processPendingUDPDatagrams();
    void update_marker_list(std::vector<int> ids);

private slots:

    void updateTabelWidget(QTableWidget* pointer, QString name);

    void update_statusbar(StateSerialization *state);

    void update_tablewidgets(StateSerialization *state);

    void update_plot(StateSerialization *state);

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_comboBox_activated(const QString &arg1);

    void on_comboBox_2_activated(int index);

    void on_pushButton_15_clicked();

    void on_lineEdit_7_returnPressed();

    void on_lineEdit_8_returnPressed();

    void on_pushButton_18_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_21_clicked();

    void on_lineEdit_2_textChanged(const QString &arg1);

private:
    Ui::UserInterface *ui;
    QUdpSocket *socket_udp_ = nullptr;

    StateSerialization last_state_;

    Videostream *videostream_0;
    Videostream *videostream_1;
    Tcp_command *tcp_command_;
    QThread* tcp_thread_;

    std::chrono::microseconds last_time;
    std::deque <double> frequency_buffer;
    QVector<double> graph_buffer_y;
    QVector<double> graph_buffer_x;
    double counter = 0;
    int save_image_counter_ = 0;
    QString command_;
    Marker marker_;
    SkillHandlerInterface skill_handler_interface_;


};
#endif // USER_INTERFACE_H
