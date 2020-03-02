#include "marker.h"

void Marker::get_marker_positions(cv::Mat image)
{
//    if(port_==6000)
//    {
//        ids_.clear();
//        ids_ = ids;
//        positons_.clear();


//        cv::Mat rotation_matrix(3,3,CV_64F);
//        cv::Mat complete_matrix(4,4,CV_64F);




//        for(int i = 0; i<ids.size();i++){
//            cv::Rodrigues(rvecs[i],rotation_matrix);

//            complete_matrix.zeros(4,4,CV_64F);
//            complete_matrix.at<double>(0,0) = rotation_matrix.at<double>(0,0);
//            complete_matrix.at<double>(0,1) = rotation_matrix.at<double>(0,1);
//            complete_matrix.at<double>(0,2) = rotation_matrix.at<double>(0,2);
//            complete_matrix.at<double>(1,0) = rotation_matrix.at<double>(1,0);
//            complete_matrix.at<double>(1,1) = rotation_matrix.at<double>(1,1);
//            complete_matrix.at<double>(1,2) = rotation_matrix.at<double>(1,2);
//            complete_matrix.at<double>(2,0) = rotation_matrix.at<double>(2,0);
//            complete_matrix.at<double>(2,1) = rotation_matrix.at<double>(2,1);
//            complete_matrix.at<double>(2,2) = rotation_matrix.at<double>(2,2);


//            complete_matrix.at<double>(0,3) = tvecs[i][0];
//            complete_matrix.at<double>(1,3) = tvecs[i][1];
//            complete_matrix.at<double>(2,3) = tvecs[i][2];
//            complete_matrix.at<double>(3,3) = 1;



//            positons_.push_back(complete_matrix);



//        }
//    }
}

cv::Mat Marker::draw_marker(cv::Mat image)
{
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners;
    cv::aruco::detectMarkers(image, dictionary, corners, ids);

    if (ids.size() > 0) {


        cv::aruco::drawDetectedMarkers(image, corners, ids);
        std::vector<cv::Vec3d> rvecs, tvecs;
        cv::aruco::estimatePoseSingleMarkers(corners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);

        // draw axis for each marker
        for(int i=0; i<ids.size(); i++){
            cv::aruco::drawAxis(image, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.05);
        }
    }

    return image;

}

Marker::Marker()
{

}
