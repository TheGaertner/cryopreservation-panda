#include "videostream.h"

#include <opencv2/videoio.hpp>
#include <opencv2/calib3d.hpp>

void Videostream::stop()
{
    running_ = false;
}

void Videostream::go()
{
    running_ = true;
}

void Videostream::set_port(int port)
{
    port_ =port;
    port_changed_ = true;

}

Videostream::Videostream(int port, int position){
    port_ =port;
    position_=position;
}


void Videostream::run()
{
    cv::VideoCapture capture("udpsrc port=" + std::to_string(port_) + " ! application/x-rtp, payload=127 ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! appsink sync=false", cv::CAP_GSTREAMER);
    if (!capture.isOpened()) {
        //Error
        qDebug() << "Error while open GStreamer" << capture.isOpened() << endl;
    }

    cv::Mat frame;
    std::chrono::microseconds start, end;

    running_ = true;
    while(running_) {
        if(port_changed_){
            port_changed_ = false;
            capture = cv::VideoCapture("udpsrc port=" + std::to_string(port_) + " ! application/x-rtp, payload=127 ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! appsink sync=false", cv::CAP_GSTREAMER);
            if (!capture.isOpened()) {
                //Error
                qDebug() << "Error while open GStreamer" << endl;
            }
        }

        // Start time for showing frame rate
        start= std::chrono::duration_cast< std::chrono::microseconds >(
                    std::chrono::system_clock::now().time_since_epoch()
                    );

        if (!capture.read(frame)) {
            // qDebug() << "Error during read frame" << endl;
        }
        else
        {
            // Save current frame
            frame.copyTo(last_image_);

            // Calculating and inserting frame rate
            end= std::chrono::duration_cast< std::chrono::microseconds >(
                        std::chrono::system_clock::now().time_since_epoch()
                        );
            double seconds = ((end.count())+1 - start.count())*0.000001;
            double fps  = 1.0 / seconds;
            putText(frame, "FPS: "+std::to_string(fps).substr(0,4), cv::Point2f(80, 60),  cv::FONT_HERSHEY_SIMPLEX, 1.5 ,cv::Scalar(0,0,255,255),3,cv::LINE_AA);

            // Adding detected marker
            frame = marker_detector.draw_marker(frame);


            // Send image to interface
            QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
            QImage small = qimg.scaled(500, 1000,Qt::KeepAspectRatio);

            if(position_ == 0){
                emit sendQPixmap_0(QPixmap::fromImage(small.rgbSwapped()));
                emit update_marker_ids(marker_detector.get_ids(last_image_));
            }

            if(position_ == 1){
                emit sendQPixmap_1(QPixmap::fromImage(small.rgbSwapped()));
            }



        }


    }
}
