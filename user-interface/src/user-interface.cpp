#include "user-interface.h"
#include "ui_userinterface.h"

#include "serialization.h"

UserInterface::UserInterface(QWidget *parent, QString  command)
    : QMainWindow(parent)
    , ui(new Ui::UserInterface)
    , last_time(std::chrono::duration_cast< std::chrono::microseconds >(
                    std::chrono::system_clock::now().time_since_epoch()))
    , marker_(&last_state_)
    , skill_handler_interface_()
{

    ui->setupUi(this);

    // Loading YAML config
    YAML::Node config = ConfigHandler::getConfig();

    // Set up plot in gui
    ui->widget->addGraph();

    // Set up two displays
    ui->graphicsView->setScene(new QGraphicsScene(this));
    ui->graphicsView->scene()->addItem(&pixmap_0);

    ui->graphicsView_2->setScene(new QGraphicsScene(this));
    ui->graphicsView_2->scene()->addItem(&pixmap_1);

    // Set up two videostream
    ui->lineEdit_7->setText(QString::fromStdString(config["nuc"]["camera_0"]["port"].as<std::string>()));
    videostream_0 = new Videostream(config["nuc"]["camera_0"]["port"].as<int>(),0);
    videostream_0->start();

    ui->lineEdit_8->setText(QString::fromStdString(config["nuc"]["camera_1"]["port"].as<std::string>()));
    videostream_1 = new Videostream(config["nuc"]["camera_1"]["port"].as<int>(),1);
    videostream_1->start();

    // Connect videostreams to displays
    connect(videostream_0,&Videostream::sendQPixmap_0,this,[=]( QPixmap item) { pixmap_0.setPixmap(item);});
    connect(videostream_1,&Videostream::sendQPixmap_1,this,[=]( QPixmap item) { pixmap_1.setPixmap(item);});

    // Update detected marker
    qRegisterMetaType<std::vector<int> >("std::vector<int>");
    connect(videostream_0,&Videostream::update_marker_ids,this,&UserInterface::update_marker_list);

    // Filling combo boxes with selection possibilites
    for (auto name : StateSerialization::getNames()) {
        ui->comboBox->addItem(QString::fromStdString(name));
        ui->comboBox_3->addItem(QString::fromStdString(name));
        ui->comboBox_4->addItem(QString::fromStdString(name));;
    }

    // Setup TCP connection
    std::string input = config["nuc"]["hostname"].as<std::string>()+":"+config["nuc"]["tcp_port"].as<std::string>();
    ui->lineEdit_2->setText(QString::fromStdString(input));

    tcp_command_ = new Tcp_command();
    tcp_thread_ = new QThread;
    tcp_command_->moveToThread(tcp_thread_);
    tcp_thread_->start();

    connect(this,SIGNAL(init_tcp()),tcp_command_,SLOT(init()));
    emit init_tcp();

    connect(this,SIGNAL(tcp_custome_command(QString)),tcp_command_,SLOT(write_command(QString)));
    connect(this,SIGNAL(tcp_adresse_changed(QString)),tcp_command_,SLOT(new_adresse(QString)));
    emit tcp_adresse_changed(ui->lineEdit_2->text());

    connect(ui->pushButton_4,&QPushButton::clicked,tcp_command_,[=]() { tcp_command_->write_command("GoToInit");});
    connect(ui->pushButton_5,&QPushButton::clicked,tcp_command_,[=]() { tcp_command_->write_command("StopAll");});
    connect(ui->pushButton_3,&QPushButton::clicked,tcp_command_,[=]() { tcp_command_->write_command(ui->lineEdit->displayText());});
    connect(ui->lineEdit,&QLineEdit::returnPressed,tcp_command_,[=]() { tcp_command_->write_command(ui->lineEdit->displayText());});

    // Set up UDP connection for receiving robot states
    socket_udp_ = new QUdpSocket(this);
    socket_udp_->bind(QHostAddress::AnyIPv4, config["client"]["udp_port"].as<int>());
    connect(socket_udp_, &QUdpSocket::readyRead, this, &UserInterface::processPendingUDPDatagrams);

    // Update Statusbar
    connect(this,&UserInterface::current_robot_state,this,&UserInterface::update_statusbar);

    // Update Matrix
    connect(ui->comboBox_3 , static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated), this ,[&](QString arg1) { updateTabelWidget(ui->tableWidget,arg1);});
    connect(ui->comboBox_4 , static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated), this ,[&](QString arg1) { updateTabelWidget(ui->tableWidget_2,arg1);});

    ui->comboBox_3->setCurrentIndex(0);
    ui->comboBox_4->setCurrentText("q");
    emit ui->comboBox_4->activated("q");
    connect(this,&UserInterface::current_robot_state,this,&UserInterface::update_tablewidgets);

    // Update Plot
    connect(this,&UserInterface::current_robot_state,this,&UserInterface::update_plot);

    // Group Widget
    skill_handler_interface_.setup_skillhandler(videostream_0);
    connect(ui->lineEdit_13,&QLineEdit::textChanged,&skill_handler_interface_,&SkillHandlerInterface::setNewGroupName);
    connect(ui->pushButton,&QPushButton::clicked,&skill_handler_interface_,&SkillHandlerInterface::addGroup);
    connect(ui->pushButton_2,&QPushButton::clicked,&skill_handler_interface_,&SkillHandlerInterface::removeGroup);
    connect(ui->listWidget_3,&QListWidget::itemClicked,&skill_handler_interface_,&SkillHandlerInterface::setSelectedGroup);
    connect(ui->listWidget_3,&QListWidget::itemClicked,&skill_handler_interface_,&SkillHandlerInterface::openSkills);


    connect(&skill_handler_interface_,&SkillHandlerInterface::clearGroupWidget, ui->listWidget_3,&QListWidget::clear);
    connect(&skill_handler_interface_,&SkillHandlerInterface::addGroupToWidget, ui->listWidget_3,static_cast<void (QListWidget::*)(QListWidgetItem *)>(&QListWidget::addItem));
    connect(&skill_handler_interface_,&SkillHandlerInterface::setGroupRow, ui->listWidget_3,static_cast<void (QListWidget::*)(int)>(&QListWidget::setCurrentRow));

    // Skill Widget
    connect(ui->lineEdit_11,&QLineEdit::textChanged,&skill_handler_interface_,&SkillHandlerInterface::setNewSkillName);
    connect(ui->pushButton_14,&QPushButton::clicked,&skill_handler_interface_,&SkillHandlerInterface::addSkill);
    connect(ui->pushButton_22,&QPushButton::clicked,&skill_handler_interface_,&SkillHandlerInterface::removeSkill);
    connect(ui->listWidget_4,&QListWidget::itemClicked,&skill_handler_interface_,&SkillHandlerInterface::setSelectedSkill);
    connect(ui->listWidget_4,&QListWidget::itemClicked,&skill_handler_interface_,&SkillHandlerInterface::openSequence);

    connect(&skill_handler_interface_,&SkillHandlerInterface::clearSkillWidget, ui->listWidget_4,&QListWidget::clear);
    connect(&skill_handler_interface_,&SkillHandlerInterface::addSkillToWidget, ui->listWidget_4,static_cast<void (QListWidget::*)(QListWidgetItem *)>(&QListWidget::addItem));
    connect(&skill_handler_interface_,&SkillHandlerInterface::setSkillRow, ui->listWidget_4,static_cast<void (QListWidget::*)(int)>(&QListWidget::setCurrentRow));

    // Sequence Widget
    connect(&skill_handler_interface_,&SkillHandlerInterface::clearSequenceElementWidget, ui->listWidget_2,&QListWidget::clear);
    connect(&skill_handler_interface_,&SkillHandlerInterface::addSequenceElementToWidget, ui->listWidget_2,static_cast<void (QListWidget::*)(QListWidgetItem *)>(&QListWidget::addItem));
    connect(&skill_handler_interface_,&SkillHandlerInterface::setSequenceElementRow, ui->listWidget_2,static_cast<void (QListWidget::*)(int)>(&QListWidget::setCurrentRow));
    connect(ui->listWidget_2,&QListWidget::itemClicked,&skill_handler_interface_,&SkillHandlerInterface::setSelectedSequenceElement);
    connect(ui->pushButton_23,&QPushButton::clicked,&skill_handler_interface_,&SkillHandlerInterface::removeSequenceElement);
    connect(ui->pushButton_25,&QPushButton::clicked,&skill_handler_interface_,&SkillHandlerInterface::moveSequenceElementUp);
    connect(ui->pushButton_24,&QPushButton::clicked,&skill_handler_interface_,&SkillHandlerInterface::moveSequenceElementDown);

    connect(ui->pushButton_34,&QPushButton::clicked,&skill_handler_interface_,&SkillHandlerInterface::playSequence);
    connect(&skill_handler_interface_,&SkillHandlerInterface::command,tcp_command_,[=](QString command) { tcp_command_->write_command(command);});


    // Primitives
    connect(&skill_handler_interface_,&SkillHandlerInterface::addMovementPrimitiveToWidget, ui->comboBox_5,static_cast<void (QComboBox::*)(const QStringList&)>(&QComboBox::addItems));
    connect(this,&UserInterface::current_robot_state,&skill_handler_interface_, &SkillHandlerInterface::setLastState);


    // Relative Pose
    connect(ui->listWidget_6,&QListWidget::itemClicked,this,[=](QListWidgetItem *item) {ui->label_52->setText(item->text());});
    connect(ui->listWidget_6,&QListWidget::itemClicked,&skill_handler_interface_, &SkillHandlerInterface::setActualMarker);
    connect(ui->pushButton_29,&QPushButton::clicked,&skill_handler_interface_,&SkillHandlerInterface::savePose);
    connect(ui->lineEdit_26,&QLineEdit::textChanged,&skill_handler_interface_,&SkillHandlerInterface::saveDuration);

    connect(ui->pushButton_30,&QPushButton::clicked,&skill_handler_interface_,&SkillHandlerInterface::createRelativePose);

    // Joint Pose
    connect(ui->pushButton_31,&QPushButton::clicked,&skill_handler_interface_,&SkillHandlerInterface::savePose);
    connect(ui->lineEdit_27,&QLineEdit::textChanged,&skill_handler_interface_,&SkillHandlerInterface::saveDuration);
    connect(ui->pushButton_32,&QPushButton::clicked,&skill_handler_interface_,&SkillHandlerInterface::createJointPosition);


    // Relative Movement
    connect(ui->pushButton_37,&QPushButton::clicked,&skill_handler_interface_,[&]() { skill_handler_interface_.createRelMovement(ui->lineEdit_29->text().toDouble(),
                                                                                                                                 ui->lineEdit_30->text().toDouble(),
                                                                                                                                 ui->lineEdit_31->text().toDouble(),
                                                                                                                                 ui->lineEdit_32->text().toDouble(),
                                                                                                                                 ui->lineEdit_33->text().toDouble(),
                                                                                                                                 ui->lineEdit_34->text().toDouble(),
                                                                                                                                 ui->lineEdit_28->text().toDouble(),
                                                                                                                                 ui->radioButton_4->isChecked());});

    // Gripper
    connect(ui->pushButton_39,&QPushButton::clicked,&skill_handler_interface_,[&]() { skill_handler_interface_.gripper_action(ui->lineEdit_35->text().toDouble(),
                                                                                                                              ui->lineEdit_36->text().toDouble(),
                                                                                                                              ui->lineEdit_37->text().toDouble(),
                                                                                                                              ui->lineEdit_38->text().toDouble());});

    connect(ui->pushButton_40,&QPushButton::clicked,&skill_handler_interface_,[&]() { skill_handler_interface_.gripper_homing();});

    // Contact Collision Behaviour
    connect(ui->pushButton_48,&QPushButton::clicked,&skill_handler_interface_,[&]() { skill_handler_interface_.set_colcon_behaviour(ui->lineEdit_53->text().toDouble(),
                                                                                                                                    ui->lineEdit_58->text().toDouble());});

    // Set Cartesian Impedance
    connect(ui->pushButton_49,&QPushButton::clicked,&skill_handler_interface_,[&]() { skill_handler_interface_.set_cartesian_impedance(ui->lineEdit_67->text().toDouble(),
                                                                                                                                       ui->lineEdit_66->text().toDouble(),
                                                                                                                                       ui->lineEdit_52->text().toDouble(),
                                                                                                                                       ui->lineEdit_64->text().toDouble(),
                                                                                                                                       ui->lineEdit_65->text().toDouble(),
                                                                                                                                       ui->lineEdit_68->text().toDouble());});

    connect(ui->pushButton_50,&QPushButton::clicked,&skill_handler_interface_,[&]() { skill_handler_interface_.set_default_behaviour();});


    connect(ui->pushButton_38,&QPushButton::clicked,&skill_handler_interface_,[&]() { skill_handler_interface_.add_end();});

    connect(ui->pushButton_41,&QPushButton::clicked,&skill_handler_interface_,[&]() { skill_handler_interface_.go_to_abs_pose(ui->lineEdit_39->text().toDouble(),
                                                                                                                              ui->lineEdit_42->text().toDouble(),
                                                                                                                              ui->lineEdit_41->text().toDouble(),
                                                                                                                              ui->lineEdit_40->text().toDouble(),
                                                                                                                              ui->lineEdit_44->text().toDouble(),
                                                                                                                              ui->lineEdit_45->text().toDouble(),
                                                                                                                              ui->lineEdit_43->text().toDouble());});




    skill_handler_interface_.init();

    // Calibration
    connect(ui->pushButton_10,&QPushButton::clicked,CameraCalibration::create_board);
    connect(ui->pushButton_12,&QPushButton::clicked,[=]() {CameraCalibration::save_photo(videostream_0->last_image_);});
    connect(ui->pushButton_13,&QPushButton::clicked,CameraCalibration::calibrate_camera);
    connect(ui->pushButton_11,&QPushButton::clicked,[&]() {Marker::create_aruco_marker(ui->lineEdit_3->text().toInt());});

}


UserInterface::~UserInterface()
{
    delete ui;
}



void UserInterface::processPendingUDPDatagrams()
{
    QHostAddress sender;
    u_int16_t port;
    while (socket_udp_->hasPendingDatagrams())
    {
        //// Get Information ////

        // Read datagram
        QByteArray datagram;
        datagram.resize(socket_udp_->pendingDatagramSize());
        socket_udp_->readDatagram(datagram.data(),datagram.size(),&sender,&port);

        // Datagram to streambuf
        boost::asio::streambuf buf;
        std::ostream os(&buf);
        os.write(datagram.data(), datagram.size());

        // Deserialize streambuf
        {
            std::istream os(&buf);
            boost::archive::binary_iarchive in_archive(os);
            in_archive >> last_state_;
        }

        last_state_.sender_ip = sender.toString().toStdString();
        last_state_.sender_port = port;

        emit current_robot_state(&last_state_);

    }
}

// set left camera
void UserInterface::on_lineEdit_7_returnPressed()
{
    int port = ui->lineEdit_7->text().toInt();

    videostream_0->set_port(port);

    YAML::Node config = ConfigHandler::getConfig();
    config["nuc"]["camera_0"]["port"] = port;

    ConfigHandler::updateConfig(config);
}

// set right camera
void UserInterface::on_lineEdit_8_returnPressed()
{
    int port = ui->lineEdit_8->text().toInt();

    videostream_1->set_port(port);

    YAML::Node config = ConfigHandler::getConfig();
    config["nuc"]["camera_1"]["port"] = port;

    ConfigHandler::updateConfig(config);
}


// Update marker list
void UserInterface::update_marker_list(std::vector<int> ids)
{
    ui->listWidget->clear();
    ui->listWidget_6->clear();

    for(auto& element : ids){
        new QListWidgetItem(QString::number(element), ui->listWidget);
        new QListWidgetItem(QString::number(element), ui->listWidget_6);
    }
}

void UserInterface::update_statusbar(StateSerialization *state)
{
    //// Show Information ////

    // Print sender adresse
    ui->label_3->setText(QString::fromStdString(state->sender_ip)+" : "+QString::number(state->sender_port));

    // Print sent time
    ui->label_4->setText(std::asctime(std::localtime(&state->current_time)));

    // Calculate and show frequency
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

    ui->label_12->setText(QString::number(1/(mean*0.000001)).remove(4,QString::number(1/(mean*0.000001)).length()-4));

    // Print robot mode
    QString mode;
    switch(state->robot_mode_converted){
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

// Set tablewidget based on chosen name in droplist and update valuesin tablewidget
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

void UserInterface::update_tablewidgets(StateSerialization *state)
{
    // Print matrixes
    setTableWidgetContent(ui->tableWidget,ui->comboBox_3->currentText(),state);
    setTableWidgetContent(ui->tableWidget_2,ui->comboBox_4->currentText(),state);
}

void UserInterface::update_plot(StateSerialization *state)
{
    // Update plot
    graph_buffer_y.push_back(state->getData(ui->comboBox->currentText().toStdString(),ui->comboBox_2->currentIndex()));
    graph_buffer_x.push_back(counter++);
    if(graph_buffer_y.size()>5000){
        graph_buffer_y.pop_front();
        graph_buffer_x.pop_front();
    }
    ui->widget->graph(0)->setData(graph_buffer_x,graph_buffer_y);
    ui->widget->yAxis->setRange(*std::min_element(graph_buffer_y.constBegin(), graph_buffer_y.constEnd()), *std::max_element(graph_buffer_y.constBegin(), graph_buffer_y.constEnd()));
    ui->widget->xAxis->setRange(*std::min_element(graph_buffer_x.constBegin(), graph_buffer_x.constEnd()), *std::max_element(graph_buffer_x.constBegin(), graph_buffer_x.constEnd()));
    ui->widget->replot();
}

// TCP Commands

// Update tcp adresse
void UserInterface::on_lineEdit_2_textChanged(const QString &arg1)
{
    YAML::Node config = ConfigHandler::getConfig();
    std::vector<std::string> tcp_connection;
    std::string input = arg1.toStdString();
    boost::split(tcp_connection,input, boost::is_any_of(":"));
    config["nuc"]["hostname"] = tcp_connection[0];
    config["nuc"]["tcp_port"] = tcp_connection[1];
    ConfigHandler::updateConfig(config);

    emit tcp_adresse_changed(arg1);
}



// go to marker by id
void UserInterface::on_pushButton_18_clicked()
{

    cv::Mat frame = videostream_0->last_image_;

    std::tuple<std::vector<int>,std::vector<Eigen::Matrix<double, 4, 4>>> output;
    output = marker_.get_marker(frame);

    ui->listWidget->clear();

    QString command;
    for(unsigned long i = 0; i < std::get<0>(output).size(); i++){
        qDebug()  << std::get<0>(output)[i] << " " << ui->lineEdit_9->text().toInt() << endl;
        if(std::get<0>(output)[i] == ui->lineEdit_9->text().toInt())
        {
            Eigen::Matrix<double, 4, 4> matrix = std::get<1>(output)[i];

            matrix = matrix.inverse().eval();
            matrix(2,3) += 0.1;
            matrix = matrix.inverse().eval();

            command = "AbsPose";

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

        emit tcp_custome_command(command);
    }

}

// Franka Desk Calls
void UserInterface::on_pushButton_6_clicked()
{
    YAML::Node config = ConfigHandler::getConfig("config.yaml");;
    nlohmann::json response;
    cpp_utils::rpc_call(config["nuc"]["hostname"].as<std::string>(),9001,"lock_brakes",{config["robot"]["ip"].as<std::string>(),config["robot"]["desk_username"].as<std::string>(),config["robot"]["desk_password"].as<std::string>()},response);
}

void UserInterface::on_pushButton_7_clicked()
{
    YAML::Node config = ConfigHandler::getConfig("config.yaml");
    nlohmann::json response;
    cpp_utils::rpc_call(config["nuc"]["hostname"].as<std::string>(),9001,"unlock_brakes",{config["robot"]["ip"].as<std::string>(),config["robot"]["desk_username"].as<std::string>(),config["robot"]["desk_password"].as<std::string>()},response);
}

void UserInterface::on_pushButton_8_clicked()
{
    YAML::Node config = ConfigHandler::getConfig("config.yaml");
    nlohmann::json response;
    cpp_utils::rpc_call(config["nuc"]["hostname"].as<std::string>(),9001,"shutdown",{config["robot"]["ip"].as<std::string>(),config["robot"]["desk_username"].as<std::string>(),config["robot"]["desk_password"].as<std::string>()},response);
}

// set up matrices
void UserInterface::updateTabelWidget(QTableWidget* pointer, QString name){
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






// Set up plot name
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

// Set up plot id
void UserInterface::on_comboBox_2_activated(int index)
{
    UNUSED(index);
    graph_buffer_y.clear();
    graph_buffer_x.clear();

}


// Set init marker
void UserInterface::on_pushButton_15_clicked()
{
    cv::Mat frame = videostream_0->last_image_;

    std::tuple<std::vector<int>,std::vector<Eigen::Matrix<double, 4, 4>>> output;
    output = marker_.get_marker(frame);

    for(unsigned long i = 0; i < std::get<0>(output).size(); i++){
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




// save relative position
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

// "set start" pose for relative motion
void UserInterface::on_pushButton_19_clicked()
{
    cv::Mat frame = videostream_0->last_image_;

    std::tuple<std::vector<int>,std::vector<Eigen::Matrix<double, 4, 4>>> output;
    output = marker_.get_marker(frame);

    Eigen::Matrix<double, 4, 4> matrix;

    for(unsigned long i = 0; i < std::get<0>(output).size(); i++){
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

// go to saved relative pose
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

    emit tcp_custome_command(command);
}


// Go to aboslute pose
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

    emit tcp_custome_command(command);
}

// Save actual absolute pose
void UserInterface::on_pushButton_17_clicked()
{
    YAML::Node config = ConfigHandler::getConfig("poses.yaml");

    Eigen::Matrix<double, 4, 4> O_T_EE = Eigen::Map<Eigen::Matrix<double, 4, 4> >(last_state_.state_->O_T_EE.data());

    std::vector<double> vec(O_T_EE.data(), O_T_EE.data() + O_T_EE.rows() * O_T_EE.cols());

    config["Absolut Pose"][ui->lineEdit_6->text().toStdString()] =vec;
    ConfigHandler::updateConfig(config,"poses.yaml");
}
