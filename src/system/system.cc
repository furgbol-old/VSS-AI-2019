// ® Copyright FURGBot 2019


#include "labels/labels.h"
#include "system/system.h"

#include "json.hpp"

#include <fstream>


namespace vss_furgbol {
namespace system {

System::System(world_model::WorldModel *world_model) : world_model_(world_model), configured_(true) {}

System::~System() {}

void System::init() {
    std::cout << "[STATUS]: Configuring system..." << std::endl;
    configure();

    if (configured_) {
        std::cout << "[STATUS]: System configured!" << std::endl;

        createTCPReceiver();
        createGKOperator();
        createCBOperator();
        createSTOperator();
        createSerialSender();

        exec();
    } else std::cout << "[STATUS]: System configuration failed!" << std::endl;

    end();
}

void System::createTCPReceiver() {
    tcp_receiver_ = new io::TCPReceiver(world_model_);
    tcp_thread_ = std::thread(&io::TCPReceiver::init, tcp_receiver_);
}

void System::createGKOperator() {
    if (team_color_ == BLUE)
        gk_operator_ = new operation::GKOperation(&world_model_->ball, &world_model_->team_blue[GK], side_);
    else if (team_color_ == YELLOW)
        gk_operator_ = new operation::GKOperation(&world_model_->ball, &world_model_->team_yellow[GK], side_);
    gk_thread_ = std::thread(&operation::GKOperation::init, gk_operator_);
}

void System::createCBOperator() {
    if (team_color_ == BLUE)
        cb_operator_ = new operation::CBOperation(&world_model_->ball, &world_model_->team_blue[CB], side_);
    else if (team_color_ == YELLOW)
        cb_operator_ = new operation::CBOperation(&world_model_->ball, &world_model_->team_yellow[CB], side_);
    cb_thread_ = std::thread(&operation::CBOperation::init, cb_operator_);
}

void System::createSTOperator() {
    if (team_color_ == BLUE)
        st_operator_ = new operation::STOperation(&world_model_->ball, &world_model_->team_blue[ST], side_);
    else if (team_color_ == YELLOW)
        st_operator_ = new operation::STOperation(&world_model_->ball, &world_model_->team_yellow[ST], side_);
    st_thread_ = std::thread(&operation::STOperation::init, st_operator_);
}

void System::createSerialSender() {
    serial_sender_ = new io::SerialSender(execution_mode_, &gk_operator_->sending_queue, &cb_operator_->sending_queue, &st_operator_->sending_queue);
    serial_thread_ = std::thread(&io::SerialSender::init, serial_sender_);
}

void System::configure() {
    std::ifstream ifstream("config/system.json");
    nlohmann::json json_file;
    ifstream >> json_file;

    if (json_file["team color"] == "blue") team_color_ = BLUE;
    else if (json_file["team color"] == "yellow") team_color_ = YELLOW;
    else {
        std::cout << "[SYSTEM]: JSON: Unknown team color!" << std::endl;
        configured_ = false;
    }
    if (configured_) std::cout << "-> Team color: " << std::string(json_file["team color"]) << std::endl;

    if (json_file["side"] == "left") side_ = LEFT;
    else if (json_file["side"] == "right") side_ = RIGHT;
    else {
        std::cout << "[SYSTEM]: JSON: Unknown side!" << std::endl;
        configured_ = false;
    }
    if (configured_) std::cout << "-> Side: " << std::string(json_file["side"]) << std::endl;

    if (json_file["execution mode"] == "real") execution_mode_ = REAL;
    else if (json_file["execution mode"] == "simulation") execution_mode_ = SIMULATION;
    else {
        std::cout << "[SYSTEM]: JSON: Unknown execution mode!" << std::endl;
        configured_ = false;
    }
    if (configured_) std::cout << "-> Execution mode: " << std::string(json_file["execution mode"]) << std::endl;
}

void System::exec() {
    while (true) {
        /*{
            std::lock_guard<std::mutex> lock(tcp_mutex_);
            std::cout << *world_model_ << std::endl;
        }*/
    }
}

void System::end() {
    //std::cout << "[STATUS]: Closing system..." << std::endl;
    tcp_thread_.join();
    gk_thread_.join();
    cb_thread_.join();
    st_thread_.join();
    serial_thread_.join();
}

} // namespace system
} // namespace vss_furgbol