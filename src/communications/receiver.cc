// ® Copyright FURGBot 2019


#include "communications/receiver.h"
#include "labels/labels.h"

#include "json.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>


namespace vss_furgbol {
namespace communications {

Receiver::Receiver(world_model::WorldModel *world_model) : world_model_(world_model) {}

Receiver::~Receiver() {}

void Receiver::init() {
    configure();

    try {
        state_receiver_ = std::make_shared<vss::StateReceiver>();
        state_receiver_->createSocket();
    } catch (zmq::error_t &error) {}
    
    run();
    end();
}

void Receiver::run() {
    while (true) {
        updateQueues();
        sortQueues();
        updateWorldModel();
    }
}

void Receiver::end() {
    try {
        state_receiver_->closeSocket();
    } catch (zmq::error_t &error) {}
}

void Receiver::configure() {
    std::ifstream ifstream("config/communications.json");
    nlohmann::json json_file;
    ifstream >> json_file;

    max_queue_size_ = json_file["receiver"]["max queue size"];

    std::vector<float> base_queue_one;
    std::vector<std::vector<float>> base_queue_two;
    for (int i = 0; i < max_queue_size_; i++) base_queue_one.push_back(0);
    for (int i = 0; i < 3; i++) base_queue_two.push_back(base_queue_one);
    for (int i = 0; i < 2; i++) {
        robot_one_queues_.push_back(base_queue_two);
        robot_two_queues_.push_back(base_queue_two);
        robot_three_queues_.push_back(base_queue_two);
        robot_one_ordered_queues_.push_back(base_queue_two);
        robot_two_ordered_queues_.push_back(base_queue_two);
        robot_three_ordered_queues_.push_back(base_queue_two);
    }
    ball_x_queue_ = base_queue_one;
    ball_y_queue_ = base_queue_one;

    angle_queue_ = base_queue_one;

    position_to_get_ = (max_queue_size_ / 2) + 1;
}

void Receiver::updateQueues() {
    robot_one_queues_[BLUE][ANGLE].erase(robot_one_queues_[BLUE][ANGLE].begin());
    robot_two_queues_[BLUE][ANGLE].erase(robot_two_queues_[BLUE][ANGLE].begin());
    robot_three_queues_[BLUE][ANGLE].erase(robot_three_queues_[BLUE][ANGLE].begin());
    robot_one_queues_[YELLOW][ANGLE].erase(robot_one_queues_[YELLOW][ANGLE].begin());
    robot_two_queues_[YELLOW][ANGLE].erase(robot_two_queues_[YELLOW][ANGLE].begin());
    robot_three_queues_[YELLOW][ANGLE].erase(robot_three_queues_[YELLOW][ANGLE].begin());

    state_ = state_receiver_->receiveState(vss::FieldTransformationType::None);

    robot_one_queues_[BLUE][ANGLE].push_back(state_.teamYellow[0].angle);
    robot_two_queues_[BLUE][ANGLE].push_back(state_.teamYellow[1].angle);
    robot_three_queues_[BLUE][ANGLE].push_back(state_.teamYellow[2].angle);
    robot_one_queues_[YELLOW][ANGLE].push_back(state_.teamBlue[0].angle);
    robot_two_queues_[YELLOW][ANGLE].push_back(state_.teamBlue[1].angle);
    robot_three_queues_[YELLOW][ANGLE].push_back(state_.teamBlue[2].angle);
}

void Receiver::sortQueues() {
    robot_one_ordered_queues_ = robot_one_queues_;
    robot_two_ordered_queues_ = robot_two_queues_;
    robot_three_ordered_queues_ = robot_three_queues_;

    std::sort(robot_one_queues_[BLUE][ANGLE].begin(), robot_one_queues_[BLUE][ANGLE].end());
    std::sort(robot_two_queues_[BLUE][ANGLE].begin(), robot_two_queues_[BLUE][ANGLE].end());
    std::sort(robot_three_queues_[BLUE][ANGLE].begin(), robot_three_queues_[BLUE][ANGLE].end());
    std::sort(robot_one_queues_[YELLOW][ANGLE].begin(), robot_one_queues_[YELLOW][ANGLE].end());
    std::sort(robot_two_queues_[YELLOW][ANGLE].begin(), robot_two_queues_[YELLOW][ANGLE].end());
    std::sort(robot_three_queues_[YELLOW][ANGLE].begin(), robot_three_queues_[YELLOW][ANGLE].end());
}

void Receiver::updateWorldModel() {
    state_.teamBlue[0].angle = robot_one_queues_[YELLOW][ANGLE][position_to_get_];
    state_.teamBlue[1].angle = robot_two_queues_[YELLOW][ANGLE][position_to_get_];
    state_.teamBlue[2].angle = robot_three_queues_[YELLOW][ANGLE][position_to_get_];
    state_.teamYellow[0].angle = robot_one_queues_[BLUE][ANGLE][position_to_get_];
    state_.teamYellow[1].angle = robot_two_queues_[BLUE][ANGLE][position_to_get_];
    state_.teamYellow[2].angle = robot_three_queues_[BLUE][ANGLE][position_to_get_];

    std::lock_guard<std::mutex> lock(mutex_);
    world_model_->ball = state_.ball;
    for (int i = 0; i < 3; i++) world_model_->team_blue[i] = state_.teamYellow[i];
    for (int i = 0; i < 3; i++) world_model_->team_yellow[i] = state_.teamBlue[i];
}

} // namespace communications
} // namespace vss_furgbol 