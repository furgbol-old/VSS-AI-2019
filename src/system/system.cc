// ® Copyright FURGBot 2019


#include "labels/labels.h"
#include "system/system.h"

#include "json.hpp"

#include <fstream>
#include <iostream>
#include <vector>


namespace vss_furgbol {
namespace system {

System::System(world_model::WorldModel *world_model) {
    serial_repo_ = new communications::SerialRepository();
    world_model_ = world_model;
}

System::~System() {
    delete serial_repo_;
    delete world_model_;
}

void System::init() {
    configure();

    startReceiver();
    startSender();
    startGoalkeeperOperation();

    run();
    end();
}

void System::configure() {
    std::ifstream ifstream("config/system.json");
    nlohmann::json json_file;
    ifstream >> json_file;

    if (json_file["environment"]["side"] == "right") side_ = RIGHT;
    else side_ = LEFT;

    if (json_file["environment"]["team color"] == "blue") team_color_ = BLUE;
    else team_color_ = YELLOW;
    
    serial_repo_->setStatus(GK, json_file["players status"]["goalkeeper"]);
    serial_repo_->setStatus(CB, json_file["players status"]["centerback"]);
    serial_repo_->setStatus(ST, json_file["players status"]["striker"]);
}

void System::startSender() {
    sender_ = std::make_shared<communications::Sender>(serial_repo_);
    sender_thread_ = std::thread(&communications::Sender::init, sender_);
}

void System::startGoalkeeperOperation() {
    serial_repo_->setStatus(GK, true);
    gk_operator_ = std::make_shared<operations::GoalkeeperOperation>(
        &world_model_->ball,
        getRobot(GK),
        side_,
        serial_repo_
    );
    gk_thread_ = std::thread(&operations::GoalkeeperOperation::init, gk_operator_);
}

void System::startReceiver() {
    receiver_ = std::make_shared<communications::Receiver>(world_model_);
    receiver_thread_ = std::thread(&communications::Receiver::init, receiver_);
}

world_model::Robot* System::getRobot(int which_player) {
    std::lock_guard<std::mutex> lock(mutex_);
    switch (team_color_) {
        case BLUE:
            return &world_model_->team_blue[which_player];
        case YELLOW:
            return &world_model_->team_yellow[which_player];
    }
}

void System::run() { while (true); }

void System::end() {}

} // namespace system
} // namespace vss_furgbol