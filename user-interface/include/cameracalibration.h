#ifndef CAMERACALIBRATION_H
#define CAMERACALIBRATION_H

#include <iostream>

#include <experimental/filesystem>

#include <Eigen/Dense>

#include "opencv2/opencv.hpp"
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>

class CameraCalibration
{
public:
    static void create_board();
    static void save_photo(cv::Mat photo);
    static void calibrate_camera();
    CameraCalibration();
};

#endif // CAMERACALIBRATION_H
