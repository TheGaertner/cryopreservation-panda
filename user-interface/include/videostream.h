#ifndef VIDEOSTREAM_H
#define VIDEOSTREAM_H


#include <QDebug>
#include <QThread>
#include <QGraphicsPixmapItem>


#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/aruco.hpp>

#include "marker.h"


class Videostream : public QThread
{
    Q_OBJECT
public:
    void run();
    void stop();
    void go();
    void set_port(int port);

    Videostream(int port, int position);

    QPixmap pixmap_;
    std::vector <int> ids_;
    std::vector <cv::Mat> positons_;
    cv::Mat last_image_;

signals:
    void sendQPixmap_0( QPixmap pixmap);
    void sendQPixmap_1( QPixmap pixmap);
    void update_marker_ids(std::vector<int>);

private:
    int port_;
    bool port_changed_ = false;
    int position_;
    bool running_;
    Marker marker_detector;
};

#endif // VIDEOSTREAM_H
