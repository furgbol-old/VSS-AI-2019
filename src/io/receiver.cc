// ® Copyright FURGBot 2019


#include "io/receiver.h"

#include "json.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>


namespace vss_furgbol {
namespace io {

Receiver::Receiver(bool *running, bool *changed, world_model::WorldModel *world_model) : 
    world_model_(world_model), running_(running), changed_(changed) {}

Receiver::~Receiver() {}

void Receiver::init() {
    configure();

    try {
        state_receiver_ = new vss::StateReceiver();
        state_receiver_->createSocket();
    } catch (zmq::error_t &error) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            *running_ = false;
        }
    }

    if (*running_) printConfigurations();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        *changed_ = true;
    }
    
    exec();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        *changed_ = true;
    }
}

void Receiver::exec() {
    bool previous_status = false;;
    while (*running_) {
        updateQueues();
        sortQueues();
        updateWorldModel();

        //std::cout << state_ << std::endl;
        //std::cout << std::endl << *world_model_ << std::endl;
    }
}

void Receiver::end() {
    std::cout << "[STATUS]: Closing vision receiver..." << std::endl;
    try {
        state_receiver_->closeSocket();
    } catch (zmq::error_t &error) {}
}

void Receiver::configure() {
    std::cout << "[STATUS]: Configuring vision receiver..." << std::endl;

    std::ifstream ifstream("config/receiver.json");
    nlohmann::json json_file;
    ifstream >> json_file;

    max_queue_size_ = json_file["max queue size"];

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

void Receiver::printConfigurations() {
    std::cout << "[STATUS]: Vision receiver configuration done!" << std::endl;

    std::cout << "-> Configurations:" << std::endl;
    std::cout << "\tMax queue size: " << max_queue_size_ << std::endl;
}

void Receiver::updateQueues() {
    // ball_x_queue_.erase(ball_x_queue_.begin());
    // ball_y_queue_.erase(ball_y_queue_.begin());
    // robot_one_queues_[BLUE][X].erase(robot_one_queues_[BLUE][X].begin());
    // robot_one_queues_[BLUE][Y].erase(robot_one_queues_[BLUE][Y].begin());
    robot_one_queues_[BLUE][ANGLE].erase(robot_one_queues_[BLUE][ANGLE].begin());
    // robot_two_queues_[BLUE][X].erase(robot_two_queues_[BLUE][X].begin());
    // robot_two_queues_[BLUE][Y].erase(robot_two_queues_[BLUE][Y].begin());
    robot_two_queues_[BLUE][ANGLE].erase(robot_two_queues_[BLUE][ANGLE].begin());
    // robot_three_queues_[BLUE][X].erase(robot_three_queues_[BLUE][X].begin());
    // robot_three_queues_[BLUE][Y].erase(robot_three_queues_[BLUE][Y].begin());
    robot_three_queues_[BLUE][ANGLE].erase(robot_three_queues_[BLUE][ANGLE].begin());
    // robot_one_queues_[YELLOW][X].erase(robot_one_queues_[YELLOW][X].begin());
    // robot_one_queues_[YELLOW][Y].erase(robot_one_queues_[YELLOW][Y].begin());
    robot_one_queues_[YELLOW][ANGLE].erase(robot_one_queues_[YELLOW][ANGLE].begin());
    // robot_two_queues_[YELLOW][X].erase(robot_two_queues_[YELLOW][X].begin());
    // robot_two_queues_[YELLOW][Y].erase(robot_two_queues_[YELLOW][Y].begin());
    robot_two_queues_[YELLOW][ANGLE].erase(robot_two_queues_[YELLOW][ANGLE].begin());
    // robot_three_queues_[YELLOW][X].erase(robot_three_queues_[YELLOW][X].begin());
    // robot_three_queues_[YELLOW][Y].erase(robot_three_queues_[YELLOW][Y].begin());
    robot_three_queues_[YELLOW][ANGLE].erase(robot_three_queues_[YELLOW][ANGLE].begin());

    state_ = state_receiver_->receiveState(vss::FieldTransformationType::None);

    // ball_x_queue_.push_back(state_.ball.x);
    // ball_y_queue_.push_back(state_.ball.y);
    // robot_one_queues_[BLUE][X].push_back(state_.teamYellow[0].x);
    // robot_one_queues_[BLUE][Y].push_back(state_.teamYellow[0].y);
    robot_one_queues_[BLUE][ANGLE].push_back(state_.teamYellow[0].angle);
    // robot_two_queues_[BLUE][X].push_back(state_.teamYellow[1].x);
    // robot_two_queues_[BLUE][Y].push_back(state_.teamYellow[1].y);
    robot_two_queues_[BLUE][ANGLE].push_back(state_.teamYellow[1].angle);
    // robot_three_queues_[BLUE][X].push_back(state_.teamYellow[2].x);
    // robot_three_queues_[BLUE][Y].push_back(state_.teamYellow[2].y);
    robot_three_queues_[BLUE][ANGLE].push_back(state_.teamYellow[2].angle);
    // robot_one_queues_[YELLOW][X].push_back(state_.teamBlue[0].x);
    // robot_one_queues_[YELLOW][Y].push_back(state_.teamBlue[0].y);
    robot_one_queues_[YELLOW][ANGLE].push_back(state_.teamBlue[0].angle);
    // robot_two_queues_[YELLOW][X].push_back(state_.teamBlue[1].x);
    // robot_two_queues_[YELLOW][Y].push_back(state_.teamBlue[1].y);
    robot_two_queues_[YELLOW][ANGLE].push_back(state_.teamBlue[1].angle);
    // robot_three_queues_[YELLOW][X].push_back(state_.teamBlue[2].x);
    // robot_three_queues_[YELLOW][Y].push_back(state_.teamBlue[2].y);
    robot_three_queues_[YELLOW][ANGLE].push_back(state_.teamBlue[2].angle);
}

void Receiver::sortQueues() {
    // ball_x_ordered_queue_ = ball_x_queue_;
    // ball_y_ordered_queue_ = ball_y_queue_;
    robot_one_ordered_queues_ = robot_one_queues_;
    robot_two_ordered_queues_ = robot_two_queues_;
    robot_three_ordered_queues_ = robot_three_queues_;

    // std::sort(ball_x_ordered_queue_.begin(), ball_x_ordered_queue_.end());
    // std::sort(ball_y_ordered_queue_.begin(), ball_y_ordered_queue_.end());
    // std::sort(robot_one_queues_[BLUE][X].begin(), robot_one_queues_[BLUE][X].end());
    // std::sort(robot_one_queues_[BLUE][Y].begin(), robot_one_queues_[BLUE][Y].end());
    std::sort(robot_one_queues_[BLUE][ANGLE].begin(), robot_one_queues_[BLUE][ANGLE].end());
    // std::sort(robot_two_queues_[BLUE][X].begin(), robot_two_queues_[BLUE][X].end());
    // std::sort(robot_two_queues_[BLUE][Y].begin(), robot_two_queues_[BLUE][Y].end());
    std::sort(robot_two_queues_[BLUE][ANGLE].begin(), robot_two_queues_[BLUE][ANGLE].end());
    // std::sort(robot_three_queues_[BLUE][X].begin(), robot_three_queues_[BLUE][X].end());
    // std::sort(robot_three_queues_[BLUE][Y].begin(), robot_three_queues_[BLUE][Y].end());
    std::sort(robot_three_queues_[BLUE][ANGLE].begin(), robot_three_queues_[BLUE][ANGLE].end());
    // std::sort(robot_one_queues_[YELLOW][X].begin(), robot_one_queues_[YELLOW][X].end());
    // std::sort(robot_one_queues_[YELLOW][Y].begin(), robot_one_queues_[YELLOW][Y].end());
    std::sort(robot_one_queues_[YELLOW][ANGLE].begin(), robot_one_queues_[YELLOW][ANGLE].end());
    // std::sort(robot_two_queues_[YELLOW][X].begin(), robot_two_queues_[YELLOW][X].end());
    // std::sort(robot_two_queues_[YELLOW][Y].begin(), robot_two_queues_[YELLOW][Y].end());
    std::sort(robot_two_queues_[YELLOW][ANGLE].begin(), robot_two_queues_[YELLOW][ANGLE].end());
    // std::sort(robot_three_queues_[YELLOW][X].begin(), robot_three_queues_[YELLOW][X].end());
    // std::sort(robot_three_queues_[YELLOW][Y].begin(), robot_three_queues_[YELLOW][Y].end());
    std::sort(robot_three_queues_[YELLOW][ANGLE].begin(), robot_three_queues_[YELLOW][ANGLE].end());
}

void Receiver::updateWorldModel() {
    state_.teamBlue[0].angle = robot_one_queues_[YELLOW][ANGLE][position_to_get_];
    state_.teamBlue[1].angle = robot_two_queues_[YELLOW][ANGLE][position_to_get_];
    state_.teamBlue[2].angle = robot_three_queues_[YELLOW][ANGLE][position_to_get_];
    state_.teamYellow[0].angle = robot_one_queues_[BLUE][ANGLE][position_to_get_];
    state_.teamYellow[1].angle = robot_two_queues_[BLUE][ANGLE][position_to_get_];
    state_.teamYellow[2].angle = robot_three_queues_[BLUE][ANGLE][position_to_get_];

    // world_model_->ball.x = ball_x_ordered_queue_[position_to_get_];
    // world_model_->ball.y = ball_y_ordered_queue_[position_to_get_];
    // world_model_->team_blue[0].x = robot_one_ordered_queues_[BLUE][X][position_to_get_];
    // world_model_->team_blue[0].y = robot_one_ordered_queues_[BLUE][Y][position_to_get_];
    // world_model_->team_blue[0].angle = robot_one_ordered_queues_[BLUE][ANGLE][position_to_get_];
    // world_model_->team_blue[1].x = robot_two_ordered_queues_[BLUE][X][position_to_get_];
    // world_model_->team_blue[1].y = robot_two_ordered_queues_[BLUE][Y][position_to_get_];
    // world_model_->team_blue[1].angle = robot_two_ordered_queues_[BLUE][ANGLE][position_to_get_];
    // world_model_->team_blue[2].x = robot_three_ordered_queues_[BLUE][X][position_to_get_];
    // world_model_->team_blue[2].y = robot_three_ordered_queues_[BLUE][Y][position_to_get_];
    // world_model_->team_blue[2].angle = robot_three_ordered_queues_[BLUE][ANGLE][position_to_get_];
    // world_model_->team_yellow[0].x = robot_one_ordered_queues_[YELLOW][X][position_to_get_];
    // world_model_->team_yellow[0].y = robot_one_ordered_queues_[YELLOW][Y][position_to_get_];
    // world_model_->team_yellow[0].angle = robot_one_ordered_queues_[YELLOW][ANGLE][position_to_get_];
    // world_model_->team_yellow[1].x = robot_two_ordered_queues_[YELLOW][X][position_to_get_];
    // world_model_->team_yellow[1].y = robot_two_ordered_queues_[YELLOW][Y][position_to_get_];
    // world_model_->team_yellow[1].angle = robot_two_ordered_queues_[YELLOW][ANGLE][position_to_get_];
    // world_model_->team_yellow[2].x = robot_three_ordered_queues_[YELLOW][X][position_to_get_];
    // world_model_->team_yellow[2].y = robot_three_ordered_queues_[YELLOW][Y][position_to_get_];
    // world_model_->team_yellow[2].angle = robot_three_ordered_queues_[YELLOW][ANGLE][position_to_get_];

    std::lock_guard<std::mutex> lock(mutex_);
    world_model_->ball = state_.ball;
    for (int i = 0; i < 3; i++) world_model_->team_blue[i] = state_.teamYellow[i];
    for (int i = 0; i < 3; i++) world_model_->team_yellow[i] = state_.teamBlue[i];
}

} // namespace io
} // namespace vss_furgbol 