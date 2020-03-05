#include "marker.h"

void Marker::create_aruco_marker(int id)
{
    int marker_size = 1000;

    cv::Mat markerImage;
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);
    cv::aruco::drawMarker(dictionary, id, marker_size, markerImage, 1);

    cv::Mat row = cv::Mat(50,markerImage.size[1], CV_8U,255);
    markerImage.push_back(row);

    putText(markerImage, std::to_string(id), cv::Point(30,marker_size+40),
            cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 0.8, cv::Scalar(0,0,0), 2,cv::ADAPTIVE_THRESH_MEAN_C);

    imshow("ArUco Marker: " + std::to_string(id), markerImage);
}

std::vector<int> Marker::get_ids(cv::Mat image)
{
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners;
    cv::aruco::detectMarkers(image, dictionary, corners, ids);

    return ids;
}

std::tuple<std::vector<int>, std::vector<Eigen::Matrix<double, 4, 4>>> Marker::get_marker(cv::Mat image)
{


    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners;
    cv::aruco::detectMarkers(image, dictionary, corners, ids);


    std::vector<Eigen::Matrix<double, 4, 4>> positions;

    for(int i = 0; i < int(ids.size());i++){
        double marker_size;
        if(ids[i]< 50)
            marker_size = 0.05;
        else if(ids[i]<100)
            marker_size = 0.08;
        else
            marker_size = 0.16;

        std::cout << "Marker Size:  " << marker_size << std::endl;

        std::vector<cv::Vec3d> rvecs, tvecs;
        cv::aruco::estimatePoseSingleMarkers(corners, marker_size, cameraMatrix, distCoeffs, rvecs, tvecs);

        cv::Mat rotation_matrix(3,3,CV_64F);
        cv::Mat complete_matrix(4,4,CV_64F);

        cv::Rodrigues(rvecs[i],rotation_matrix);

        complete_matrix.zeros(4,4,CV_64F);
        complete_matrix.at<double>(0,0) = rotation_matrix.at<double>(0,0);
        complete_matrix.at<double>(0,1) = rotation_matrix.at<double>(0,1);
        complete_matrix.at<double>(0,2) = rotation_matrix.at<double>(0,2);
        complete_matrix.at<double>(1,0) = rotation_matrix.at<double>(1,0);
        complete_matrix.at<double>(1,1) = rotation_matrix.at<double>(1,1);
        complete_matrix.at<double>(1,2) = rotation_matrix.at<double>(1,2);
        complete_matrix.at<double>(2,0) = rotation_matrix.at<double>(2,0);
        complete_matrix.at<double>(2,1) = rotation_matrix.at<double>(2,1);
        complete_matrix.at<double>(2,2) = rotation_matrix.at<double>(2,2);


        complete_matrix.at<double>(0,3) = tvecs[i][0];
        complete_matrix.at<double>(1,3) = tvecs[i][1];
        complete_matrix.at<double>(2,3) = tvecs[i][2];
        complete_matrix.at<double>(3,3) = 1;

        Eigen::Matrix<double, 4, 4> C_T_M;
        cv::cv2eigen(complete_matrix, C_T_M);

        Eigen::Matrix<double, 4, 4> EE_T_C;
        EE_T_C = Eigen::Array44d::Zero();


        EE_T_C(1,0) = 1;
        EE_T_C(0,1) = -1;
        EE_T_C(2,2) = 1;

        EE_T_C(0,3) = 0.05;
        EE_T_C(1,3) = -0.025;
        EE_T_C(3,3) = 1;
        EE_T_C(3,3) = 1;


        Eigen::Matrix<double, 4, 4> M_T_FIN;
        M_T_FIN = Eigen::Array44d::Zero();
        M_T_FIN(1,0) = 1;
        M_T_FIN(0,1) = 1;
        M_T_FIN(2,2) = -1;
        M_T_FIN(3,3) = 1;


        Eigen::Matrix<double, 4, 4> O_T_EE = Eigen::Map<Eigen::Matrix<double, 4, 4> >(last_state_->state_->O_T_EE.data());

        Eigen::Matrix<double, 4, 4>  O_T_M = O_T_EE * EE_T_C * C_T_M;

        positions.push_back(O_T_M*M_T_FIN);
    }

    return std::make_tuple(ids,positions);
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

            cv::Point2f midpoint = (corners[i][0]+corners[i][1]+corners[i][2]+corners[i][3])/4;
            putText(image, std::to_string(ids[i]), midpoint,  cv::FONT_HERSHEY_SIMPLEX, 2 ,cv::Scalar(255,255,255,255),4,cv::LINE_AA);
        }
    }

    return image;

}

Marker::Marker()
{

}

Marker::Marker(StateSerialization *last_state):last_state_(last_state)
{

}
