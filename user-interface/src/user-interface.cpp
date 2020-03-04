#include "user-interface.h"
#include "ui_userinterface.h"

#include "serialization.h"

void setTableWidgetContent(QTableWidget* pointer, QString name, StateSerialization* state2){
    double number = state2->getData(name.toStdString());
    QString text;
    for(int i = 0; i <number; i++)
    {
        if(number == 16){
            if(pointer->item(i%4,i/4) == nullptr){
                pointer->setItem(i%4,i/4,new QTableWidgetItem("Test"));
            }

            pointer->item(i%4,i/4)->setTextAlignment(Qt::AlignRight);
            pointer->item(i%4,i/4)->setText(text.sprintf("%.2f", state2->getData(name.toStdString(),i)));
        }else{
            if(pointer->item(i,0) == nullptr){
                pointer->setItem(i,0,new QTableWidgetItem("Test"));
            }

            pointer->item(i,0)->setTextAlignment(Qt::AlignRight);
            pointer->item(i,0)->setText(text.sprintf("%.2f", state2->getData(name.toStdString(),i)));


        }

    }
}

void UserInterface::processPendingDatagrams()
{
    QHostAddress sender;
    u_int16_t port;
    while (socket_udp_->hasPendingDatagrams())
    {

        double duration = std::chrono::duration_cast< std::chrono::microseconds >(
                    std::chrono::system_clock::now().time_since_epoch()
                    ).count()+1 - last_time.count();

        last_time = std::chrono::duration_cast< std::chrono::microseconds >(
                    std::chrono::system_clock::now().time_since_epoch());

        frequency_buffer.push_back(duration);
        if(frequency_buffer.size()>4000){
            frequency_buffer.pop_front();
        }
        double sum = std::accumulate(frequency_buffer.begin(), frequency_buffer.end(), 0.0);
        double mean = sum / frequency_buffer.size();

        ui->label_12->setText(QString::number(1/(mean*0.000001)));


        // Read datagram
        QByteArray datagram;
        datagram.resize(socket_udp_->pendingDatagramSize());
        socket_udp_->readDatagram(datagram.data(),datagram.size(),&sender,&port);

        // Datagram to streambuf
        boost::asio::streambuf buf;
        std::ostream os(&buf);
        os.write(datagram.data(), datagram.size());

        // Deserialize streambuf
        //        StateSerialization last_state_ = StateSerialization();

        {
            std::istream os(&buf);
            boost::archive::binary_iarchive in_archive(os);
            in_archive >> last_state_;
        }

        // Print to gui
        ui->label_3->setText(sender.toString()+" : "+QString::number(port));

        ui->label_4->setText(std::asctime(std::localtime(&last_state_.current_time)));

        QString text;


        graph_buffer_y.push_back(last_state_.getData(ui->comboBox->currentText().toStdString(),ui->comboBox_2->currentIndex()));
        graph_buffer_x.push_back(counter++);

        if(graph_buffer_y.size()>5000){
            graph_buffer_y.pop_front();
            graph_buffer_x.pop_front();
        }


        ui->widget->graph(0)->setData(graph_buffer_x,graph_buffer_y);

        ui->widget->yAxis->setRange(*std::min_element(graph_buffer_y.constBegin(), graph_buffer_y.constEnd()), *std::max_element(graph_buffer_y.constBegin(), graph_buffer_y.constEnd()));
        ui->widget->xAxis->setRange(*std::min_element(graph_buffer_x.constBegin(), graph_buffer_x.constEnd()), *std::max_element(graph_buffer_x.constBegin(), graph_buffer_x.constEnd()));

        ui->widget->replot();



        setTableWidgetContent(ui->tableWidget,ui->comboBox_3->currentText(),&last_state_);
        setTableWidgetContent(ui->tableWidget_2,ui->comboBox_4->currentText(),&last_state_);
        setTableWidgetContent(ui->tableWidget_3,ui->comboBox_5->currentText(),&last_state_);
        setTableWidgetContent(ui->tableWidget_4,ui->comboBox_6->currentText(),&last_state_);

        QString mode;
        switch(last_state_.robot_mode_converted){
        case 0:
            mode="Other";
            break;
        case 1:
            mode="Idle";
            break;
        case 2:
            mode="Move";
            break;
        case 3:
            mode="Guiding";
            break;
        case 4:
            mode="Reflex";
            break;
        case 5:
            mode="UserStopped";
            break;
        case 6:
            mode="AutomaticErrorRecovery";
            break;
        default:
            mode="No mode?";
        }

        ui->label_14->setText(mode);


    }
}

void UserInterface::set_display_0(QPixmap item)
{
    pixmap_0.setPixmap(item);
}

void UserInterface::set_display_1(QPixmap item)
{
    pixmap_1.setPixmap(item);
}


UserInterface::UserInterface(QWidget *parent, QString  command)
    : QMainWindow(parent)
    , ui(new Ui::UserInterface)
    , last_time(std::chrono::duration_cast< std::chrono::microseconds >(
                    std::chrono::system_clock::now().time_since_epoch()))
    , marker_(&last_state_)
{

    ui->setupUi(this);

    // Loading YAML config
    ConfigHandler::setConfig(command.toStdString(),true);
    YAML::Node config = ConfigHandler::getConfig();

    // Set up plot in gui
    ui->widget->addGraph();

    // Set up two displays
    ui->graphicsView->setScene(new QGraphicsScene(this));
    ui->graphicsView->scene()->addItem(&pixmap_0);

    ui->graphicsView_2->setScene(new QGraphicsScene(this));
    ui->graphicsView_2->scene()->addItem(&pixmap_1);

    // Set up two videostream
    ui->lineEdit_7->setText(QString::fromStdString(config["camera_0_port"].as<std::string>()));
    videostream_0 = new Videostream(config["camera_0_port"].as<int>(),0);
    videostream_0->start();

    ui->lineEdit_8->setText(QString::fromStdString(config["camera_1_port"].as<std::string>()));
    videostream_1 = new Videostream(config["camera_1_port"].as<int>(),1);
    videostream_1->start();

    // Connect videostreams to displays
    connect(videostream_0,&Videostream::sendQPixmap_0,this,&UserInterface::set_display_0);
    connect(videostream_1,&Videostream::sendQPixmap_1,this,&UserInterface::set_display_1);


    // Filling combo boxes with selection possibilites
    for (auto name : StateSerialization::getNames()) {
        ui->comboBox->addItem(QString::fromStdString(name));
        ui->comboBox_3->addItem(QString::fromStdString(name));
        ui->comboBox_4->addItem(QString::fromStdString(name));
        ui->comboBox_5->addItem(QString::fromStdString(name));
        ui->comboBox_6->addItem(QString::fromStdString(name));
    }


    // tbd - Setting initial values for comboboxes
    ui->comboBox_3->setCurrentIndex(0);
    ui->comboBox_4->setCurrentIndex(21);
    on_comboBox_4_activated("q");
    ui->comboBox_5->setCurrentIndex(0);
    ui->comboBox_6->setCurrentIndex(0);

    // Setup TCP connection
    std::string input = config["nuc_hostname"].as<std::string>()+":"+config["nuc_tcp_port"].as<std::string>();
    ui->lineEdit_2->setText(QString::fromStdString(input));
    tcp_command = new Tcp_command();
    thread = new QThread;
    tcp_command->moveToThread(thread);
    thread->start();
    connect(this,SIGNAL(init_tcp()),tcp_command,SLOT(init()));
    connect(this,SIGNAL(tcp_autorecovery()),tcp_command,SLOT(autorecovery()));
    connect(this,SIGNAL(go_to_init()),tcp_command,SLOT(go_to_init()));
    connect(this,SIGNAL(tcp_custome_command(QString)),tcp_command,SLOT(custome_command(QString)));
    connect(this,SIGNAL(tcp_abs_pose(QString)),tcp_command,SLOT(custome_command(QString)));
    emit init_tcp();
    connect(this,SIGNAL(tcp_connect(QString)),tcp_command,SLOT(connect(QString)));

    // Set up UDP connection for receiving robot states
    socket_udp_ = new QUdpSocket(this);
    socket_udp_->bind(QHostAddress::AnyIPv4, config["udp_client_port"].as<int>());
    connect(socket_udp_, &QUdpSocket::readyRead, this, &UserInterface::processPendingDatagrams);


}

UserInterface::~UserInterface()
{
    delete ui;
}

void UserInterface::on_pushButton_clicked()
{
    emit tcp_autorecovery();


}


void UserInterface::on_pushButton_2_clicked()
{
    YAML::Node config = ConfigHandler::getConfig();


    std::vector<std::string> tcp_connection;

    std::string input = ui->lineEdit_2->displayText().toStdString();

    boost::split(tcp_connection,input, boost::is_any_of(":"));

    config["nuc_hostname"] = tcp_connection[0];
    config["nuc_tcp_port"] = tcp_connection[1];

    ConfigHandler::updateConfig(config);

    emit tcp_connect(ui->lineEdit_2->displayText().toUtf8());
}


void UserInterface::on_pushButton_3_clicked()
{
    emit tcp_custome_command(ui->lineEdit->displayText());
}

void UserInterface::on_pushButton_4_clicked()
{
    emit go_to_init();
}


<<<<<<< HEAD
// #include "rpc/client.h"
#include <nlohmann/json.hpp>
//#include "cpp_utils/network.hpp"
=======
>>>>>>> 57265779b998334f0dc6e57cb035075b9cb1c144
using json = nlohmann::json;

void UserInterface::on_pushButton_6_clicked()
{
    nlohmann::json response;
    cpp_utils::rpc_call("biolab-017",9001,"lock_brakes",{"192.168.3.101","franka","frankaRSI"},response);
}

void UserInterface::on_pushButton_7_clicked()
{
    nlohmann::json response;
    cpp_utils::rpc_call("biolab-017",9001,"unlock_brakes",{"192.168.3.101","franka","frankaRSI"},response);

}

void UserInterface::on_pushButton_8_clicked()
{
    nlohmann::json response;
    cpp_utils::rpc_call("biolab-017",9001,"shutdown",{"192.168.3.101","franka","frankaRSI"},response);
}

void UserInterface::on_lineEdit_returnPressed()
{
    on_pushButton_3_clicked();
}

void UserInterface::on_comboBox_activated(const QString &arg1)
{
    graph_buffer_y.clear();
    graph_buffer_x.clear();


    StateSerialization forNumbers;
    double numbers = forNumbers.getData(arg1.toStdString());

    ui->comboBox_2->clear();
    for(int i=0; i < numbers; i++){
        ui->comboBox_2->addItem(QString::number(i));
    }

}


void UserInterface::on_comboBox_2_activated(int index)
{
    graph_buffer_y.clear();
    graph_buffer_x.clear();

}

void updateTabelWidget(QTableWidget* pointer, QString name){
    StateSerialization forNumbers;
    double numbers = forNumbers.getData(name.toStdString());

    if(numbers==16){
        pointer->setRowCount(4);
        pointer->setColumnCount(4);
    }else{
        pointer->setRowCount(numbers);
        pointer->setColumnCount(1);
    }
}



void UserInterface::on_comboBox_3_activated(const QString &arg1)
{
    updateTabelWidget(ui->tableWidget,arg1);
}



void UserInterface::on_comboBox_4_activated(const QString &arg1)
{
    updateTabelWidget(ui->tableWidget_2,arg1);
}

void UserInterface::on_comboBox_5_activated(const QString &arg1)
{
    updateTabelWidget(ui->tableWidget_3,arg1);
}

void UserInterface::on_comboBox_6_activated(const QString &arg1)
{
    updateTabelWidget(ui->tableWidget_4,arg1);
}

void UserInterface::on_pushButton_9_clicked()
{
    cv::Mat position = videostream_0->positons_[0];
    int id = videostream_0->ids_[0];

    qDebug() << id << endl;

    Eigen::Matrix<double, 4, 4> C_T_M;
    cv::cv2eigen(position, C_T_M);

    Eigen::Matrix<double, 4, 4> EE_T_C;
    EE_T_C = Eigen::Array44d::Zero();

    EE_T_C(1,0) = 1;
    EE_T_C(0,1) = -1;
    EE_T_C(2,2) = 1;

    EE_T_C(0,3) = 0.05;
    EE_T_C(1,3) = -0.03;
    EE_T_C(3,3) = 1;


    Eigen::Matrix<double, 4, 4> O_T_EE = Eigen::Map<Eigen::Matrix<double, 4, 4> >(last_state_.state_->O_T_EE.data());

    //    Eigen::Matrix<double, 4, 4> F_T_EE = Eigen::Map<Eigen::Matrix<double, 4, 4> >(last_state_.state_->F_T_EE.data());


    //    Eigen::Vector3d ea = (O_T_EE).block<3,3>(0,0).eulerAngles(0, 1, 2);

    //    std::cout << "Rotation X,Y,Z:" << std::endl;
    //    std::cout << ea << std::endl;

    std::cout << "O_T_EE:" << std::endl;
    std::cout << O_T_EE << std::endl;

    std::cout << "EE_T_C:" << std::endl;
    std::cout << EE_T_C << std::endl;

    std::cout << "C_T_M:" << std::endl;
    std::cout << C_T_M << std::endl;

    std::cout << "O_T_C:" << std::endl;
    std::cout <<  O_T_EE * EE_T_C << std::endl;

    std::cout << "O_T_M: " << std::endl;
    std::cout <<  O_T_EE * EE_T_C * C_T_M << std::endl;

    std::cout << "EE_T_M: " << std::endl;
    std::cout <<  EE_T_C * C_T_M << std::endl;

    std::cout << "END" << std::endl;

    QString command = "AbsPose";

    for(int i = 0; i < 16; i++){
        command+= " ";
        command+= QString::number((O_T_EE * EE_T_C * C_T_M)(i%4,i/4)+0.0000000001);
    }

    emit tcp_abs_pose(command);

    return;

}

void UserInterface::on_pushButton_10_clicked()
{
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);
    cv::Ptr<cv::aruco::CharucoBoard> board = cv::aruco::CharucoBoard::create(5, 7, 0.04, 0.02, dictionary);
    cv::Mat boardImage;
    board->draw( cv::Size(800, 1000), boardImage, 10, 1 );
    imshow("CharucoBoard", boardImage);
}

void UserInterface::on_pushButton_11_clicked()
{
    int marker_size = 1000;

    cv::Mat markerImage;
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);
    cv::aruco::drawMarker(dictionary, ui->lineEdit_3->text().toInt(), marker_size, markerImage, 1);

    cv::Mat row = cv::Mat(50,markerImage.size[1], CV_8U,255);
    markerImage.push_back(row);

    putText(markerImage, ui->lineEdit_3->text().toStdString(), cv::Point(30,marker_size+40),
            cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 0.8, cv::Scalar(0,0,0), 2,cv::ADAPTIVE_THRESH_MEAN_C);

    imshow(("ArUco Marker: " + ui->lineEdit_3->text()).toStdString(), markerImage);
}

void UserInterface::on_pushButton_12_clicked()
{

    std::string folderName = "images";
    std::string folderCreateCommand = "mkdir -p " + folderName;

    system(folderCreateCommand.c_str());

    cv::imwrite("images/"+std::to_string(save_image_counter_)+".jpg",videostream_0->last_image_);
    save_image_counter_++;
}

void UserInterface::on_pushButton_13_clicked()
{
    std::vector<cv::Mat> allCharucoCorners;
    std::vector<cv::Mat> allCharucoIds;

    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);
    cv::Ptr<cv::aruco::CharucoBoard> board = cv::aruco::CharucoBoard::create(5, 7, 0.04, 0.02, dictionary);

    for(int i = 0; i<save_image_counter_; i++){

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


    for(int i = 0; i<save_image_counter_; i++){
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
        qDebug() << " Fail to open " << "camera_calibration_parameter.yaml" << endl;
        exit(EXIT_FAILURE);
    }

    fs << "cameraMatrix" << cameraMatrix;
    fs << "distCoeffs" << distCoeffs;
    fs.release();


}



void UserInterface::on_pushButton_14_clicked()
{
    cv::Mat frame = videostream_0->last_image_;

    std::tuple<std::vector<int>,std::vector<Eigen::Matrix<double, 4, 4>>> output;
    output = marker_.get_marker(frame);

    ui->listWidget->clear();
    qDebug() << std::get<0>(output).size() << endl;
    for(int i = 0; i < std::get<0>(output).size(); i++){
        new QListWidgetItem(QString::number(std::get<0>(output)[i]), ui->listWidget);



    }

}

void UserInterface::on_pushButton_15_clicked()
{
    cv::Mat frame = videostream_0->last_image_;

    std::tuple<std::vector<int>,std::vector<Eigen::Matrix<double, 4, 4>>> output;
    output = marker_.get_marker(frame);

    for(int i = 0; i < std::get<0>(output).size(); i++){
        if(std::get<0>(output)[i] == ui->lineEdit_4->text().toInt())
        {
            YAML::Node config = ConfigHandler::getConfig("poses.yaml");
            Eigen::Matrix<double, 4, 4> matrix = std::get<1>(output)[i];

            std::vector<double> vec(matrix.data(), matrix.data() + matrix.rows() * matrix.cols());

            config[ui->lineEdit_10->text().toStdString()]["Start"] =vec;
            ConfigHandler::updateConfig(config,"poses.yaml");
        }

    }
}


void UserInterface::on_lineEdit_7_returnPressed()
{
    int port = ui->lineEdit_7->text().toInt();

    videostream_0->stop();
    videostream_0->set_port(port);
    videostream_0->start();

    YAML::Node config = ConfigHandler::getConfig();
    config["camera_0_port"] = port;

    ConfigHandler::updateConfig(config);
}

void UserInterface::on_lineEdit_8_returnPressed()
{
    int port = ui->lineEdit_8->text().toInt();

    videostream_1->stop();
    videostream_1->set_port(port);
    videostream_1->start();

    YAML::Node config = ConfigHandler::getConfig();
    config["camera_1_port"] = port;

    ConfigHandler::updateConfig(config);
}

void UserInterface::on_lineEdit_2_returnPressed(){

}

void UserInterface::on_pushButton_18_clicked()
{

    cv::Mat frame = videostream_0->last_image_;

    std::tuple<std::vector<int>,std::vector<Eigen::Matrix<double, 4, 4>>> output;
    output = marker_.get_marker(frame);

    ui->listWidget->clear();

    QString command;
    for(int i = 0; i < std::get<0>(output).size(); i++){
        qDebug()  << std::get<0>(output)[i] << " " << ui->lineEdit_9->text().toInt() << endl;
        if(std::get<0>(output)[i] == ui->lineEdit_9->text().toInt())
        {
            Eigen::Matrix<double, 4, 4> matrix = std::get<1>(output)[i];

//            Eigen::Matrix<double, 3, 3> rotation = matrix.block(0,0,3,3);

//            rotation = rotation*Eigen::Quaterniond(0,1,0,0)*Eigen::Quaterniond(0.7071068,0,0,-0.7071068);

//            matrix.block(0,0,3,3) = rotation;

            matrix = matrix.inverse().eval();
            matrix(2,3) += 0.1;
            matrix = matrix.inverse().eval();

            command = "AbsPose";


            Eigen::Matrix<double, 3, 3> rotation = matrix.block(0,0,3,3);
            std::cout << "Print: rotation.eulerAngles(0,1,2)  " << std::endl;
            std::cout << rotation.eulerAngles(0,1,2) << std::endl;

            for(int j = 0; j < 16; j++){
                command+= " ";
                command+= QString::number(matrix(j%4,j/4)+0.0000000001);
            }
        }

    }

    if(command.size()==0){
        qDebug() << "Marker not found!" << endl;
    }else{

        qDebug() << command << endl;

        emit tcp_abs_pose(command);
    }

}

void UserInterface::on_pushButton_16_clicked()
{
    YAML::Node config = ConfigHandler::getConfig("poses.yaml");
    std::vector<double> start_position = config[ui->lineEdit_10->text().toStdString()]["Start"].as<std::vector<double>>();

    Eigen::Matrix<double, 4, 4> start_position_eigen(start_position.data());

    Eigen::Matrix<double, 4, 4> O_T_EE = Eigen::Map<Eigen::Matrix<double, 4, 4> >(last_state_.state_->O_T_EE.data());

    Eigen::Matrix<double, 4, 4> rel_position = start_position_eigen.inverse()*O_T_EE;

    std::vector<double> vec(rel_position.data(), rel_position.data() + rel_position.rows() * rel_position.cols());

    config[ui->lineEdit_10->text().toStdString()][ui->lineEdit_5->text().toStdString()] =vec;

    ConfigHandler::updateConfig(config,"poses.yaml");

}

void UserInterface::on_pushButton_19_clicked()
{
    cv::Mat frame = videostream_0->last_image_;

    std::tuple<std::vector<int>,std::vector<Eigen::Matrix<double, 4, 4>>> output;
    output = marker_.get_marker(frame);

    Eigen::Matrix<double, 4, 4> matrix;

    for(int i = 0; i < std::get<0>(output).size(); i++){
        if(std::get<0>(output)[i] == ui->lineEdit_4->text().toInt())
        {
            YAML::Node config = ConfigHandler::getConfig("poses.yaml");
            matrix = std::get<1>(output)[i];

            std::vector<double> vec(matrix.data(), matrix.data() + matrix.rows() * matrix.cols());

            config[ui->lineEdit_10->text().toStdString()]["Start"] =vec;
            ConfigHandler::updateConfig(config,"poses.yaml");
        }

    }
}

void UserInterface::on_pushButton_20_clicked()
{
    YAML::Node config = ConfigHandler::getConfig("poses.yaml");
    std::vector<double> start_position = config[ui->lineEdit_10->text().toStdString()]["Start"].as<std::vector<double>>();
    Eigen::Matrix<double, 4, 4> start_position_eigen(start_position.data());
    std::vector<double> relative_position = config[ui->lineEdit_10->text().toStdString()][ui->lineEdit_5->text().toStdString()].as<std::vector<double>>();
    Eigen::Matrix<double, 4, 4> relative_position_eigen(relative_position.data());

    Eigen::Matrix<double, 4, 4> matrix = start_position_eigen*relative_position_eigen;

    QString command = "AbsPose";

    for(int j = 0; j < 16; j++){
        command+= " ";
        command+= QString::number(matrix(j%4,j/4)+0.0000000001);
    }

    emit tcp_abs_pose(command);
}

void UserInterface::on_pushButton_17_clicked()
{
    YAML::Node config = ConfigHandler::getConfig("poses.yaml");

    Eigen::Matrix<double, 4, 4> O_T_EE = Eigen::Map<Eigen::Matrix<double, 4, 4> >(last_state_.state_->O_T_EE.data());

    std::vector<double> vec(O_T_EE.data(), O_T_EE.data() + O_T_EE.rows() * O_T_EE.cols());

    config["Absolut Pose"][ui->lineEdit_6->text().toStdString()] =vec;
    ConfigHandler::updateConfig(config,"poses.yaml");
}

void UserInterface::on_pushButton_21_clicked()
{
    YAML::Node config = ConfigHandler::getConfig("poses.yaml");

    std::vector<double> start_position = config["Absolut Pose"][ui->lineEdit_6->text().toStdString()].as<std::vector<double>>();
    Eigen::Matrix<double, 4, 4> start_position_eigen(start_position.data());

    QString command = "AbsPose";

    for(int j = 0; j < 16; j++){
        command+= " ";
        command+= QString::number(start_position_eigen(j%4,j/4)+0.0000000001);
    }

    emit tcp_abs_pose(command);
}
