#ifndef MARKER_H
#define MARKER_H

#include <iostream>

#include <Eigen/Dense>

#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/eigen.hpp>

#include <serialization.h>


class Marker
{
public:
    std::tuple<std::vector<int>, std::vector<Eigen::Matrix<double, 4, 4>>> get_marker(cv::Mat image);
    cv::Mat draw_marker(cv::Mat image);

    Marker();
    Marker(StateSerialization *last_state);

private:
    StateSerialization *last_state_;
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);
    cv::Mat cameraMatrix = (cv::Mat_<double>(3,3) << 1.3446766408879266e+03, 0., 9.5950000000000000e+02, 0.,
                            1.3446766408879266e+03, 5.3950000000000000e+02, 0., 0., 1.);

    cv::Mat distCoeffs = (cv::Mat_<double>(5,1) << 1.8016365024038658e-01, -6.1287684954211263e-01, 0., 0., 5.7737539730274112e-01);

};

#endif // MARKER_H
