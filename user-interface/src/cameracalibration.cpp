#include "cameracalibration.h"

void CameraCalibration::create_board()
{
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);
    cv::Ptr<cv::aruco::CharucoBoard> board = cv::aruco::CharucoBoard::create(5, 7, 0.04, 0.02, dictionary);
    cv::Mat boardImage;
    board->draw( cv::Size(800, 1000), boardImage, 10, 1 );
    imshow("CharucoBoard", boardImage);
}

void CameraCalibration::save_photo(cv::Mat photo)
{
    std::string folderName = "images";
    std::string folderCreateCommand = "mkdir -p " + folderName;

    system(folderCreateCommand.c_str());

    int save_image_counter_ = 0;
    for(std::experimental::filesystem::directory_iterator it("images"); it != std::experimental::filesystem::directory_iterator(); ++it)
    {
       save_image_counter_++;
    }

    cv::imwrite("images/"+std::to_string(save_image_counter_)+".jpg",photo);
}

void CameraCalibration::calibrate_camera()
{
    int save_image_counter = 0;
    for(std::experimental::filesystem::directory_iterator it("images"); it != std::experimental::filesystem::directory_iterator(); ++it)
    {
       save_image_counter++;
    }

    std::vector<cv::Mat> allCharucoCorners;
    std::vector<cv::Mat> allCharucoIds;

    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);
    cv::Ptr<cv::aruco::CharucoBoard> board = cv::aruco::CharucoBoard::create(5, 7, 0.04, 0.02, dictionary);

    for(int i = 0; i<save_image_counter; i++){

        std::vector<int> markerIds;
        std::vector<std::vector<cv::Point2f>> markerCorners;

        cv::Mat input_image = cv::imread("images/"+std::to_string(i)+".jpg");

        cv::aruco::detectMarkers(input_image, board->dictionary, markerCorners, markerIds);

        cv::Mat currentCharucoCorners, currentCharucoIds;
        if(markerIds.size() > 0)
            cv::aruco::interpolateCornersCharuco(markerCorners, markerIds, input_image, board, currentCharucoCorners,
                                                 currentCharucoIds);


        cv::Mat input_image_copy;
        input_image.copyTo(input_image_copy);
        if(markerIds.size() > 0) cv::aruco::drawDetectedMarkers(input_image_copy,markerCorners,markerIds );

        if(currentCharucoCorners.total() > 0)
            cv::aruco::drawDetectedCornersCharuco(input_image_copy, currentCharucoCorners, currentCharucoIds);

        putText(input_image_copy, "Press key to see next picture",
                cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 2);

        imshow("out", input_image_copy);
        cv::waitKey(0);

        allCharucoCorners.push_back(currentCharucoCorners);
        allCharucoIds.push_back(currentCharucoIds);
    }

    cv::destroyWindow("out");

    cv::Mat input_image = cv::imread("images/"+std::string("0")+".jpg");

    // After capturing in several viewpoints, start calibration
    cv::Mat cameraMatrix, distCoeffs;
    std::vector<cv::Mat> rvecs, tvecs;
    cv::aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, board, cv::Size(input_image.size[0],input_image.size[1]), cameraMatrix, distCoeffs, rvecs, tvecs);


    //directly use the buffer allocated by OpenCV
    Eigen::Matrix<float,Eigen::Dynamic,Eigen::Dynamic> cameraMatrix_eigen;
    cv2eigen(cameraMatrix,cameraMatrix_eigen);
    std::cout << cameraMatrix << std::endl;


    for(int i = 0; i<save_image_counter; i++){
        cv::Mat input_image = cv::imread("images/"+std::to_string(i)+".jpg");
        cv::Mat temp = input_image.clone();

        cv::undistort(temp, input_image, cameraMatrix, distCoeffs);
        cv::imshow("out", input_image);
        cv::waitKey(0);
    }
    cv::destroyWindow("out");

    cv::FileStorage fs;
    fs.open("camera_calibration_parameter.yaml", cv::FileStorage::WRITE);
    if( !fs.isOpened() ){
        std::cout << " Fail to open " << "camera_calibration_parameter.yaml" << std::endl;
        exit(EXIT_FAILURE);
    }

    fs << "cameraMatrix" << cameraMatrix;
    fs << "distCoeffs" << distCoeffs;
    fs.release();

}

CameraCalibration::CameraCalibration()
{

}
