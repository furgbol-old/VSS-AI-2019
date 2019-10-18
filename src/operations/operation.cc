// ® Copyright FURGBot 2019


#include "labels/labels.h"
#include "operations/operation.h"

#include "json.hpp"

#include <fstream>


namespace vss_furgbol {
namespace operations {

Operation::Operation() {}

Operation::~Operation() {}

void Operation::init() {
    run();
    end();
}

void Operation::configure(std::string which_player) {
    std::ifstream ifstream("config/operations.json");
    nlohmann::json json_file;
    ifstream >> json_file;

    velocity_gain_ = json_file[which_player]["velocity gain"];
    linear_threshold_ = json_file[which_player]["linear threshold"];
    angular_threshold_ = json_file[which_player]["angular threshold"];
    max_ball_distance_ = json_file[which_player]["max ball distance"];
    max_linear_velocity_ = json_file[which_player]["max linear velocity"];
    max_angular_velocity_ = json_file[which_player]["max angular velocity"];
    kick_velocity_ = json_file[which_player]["kick velocity"];

    switch (side_) {
        case LEFT:
            field_line_.setX(json_file[which_player]["field line"]["x"]);
            break;
        case RIGHT:
            field_line_.setX(170 - (float)json_file[which_player]["field line"]["x"]);;
            break;
    }
    field_line_.setMinY(json_file[which_player]["field line"]["min y"]);
    field_line_.setMaxY(json_file[which_player]["field line"]["max y"]);

    std::lock_guard<std::mutex> lock(mutex_);
    robot_->id = json_file[which_player]["id"];
}

void Operation::end() {}

void Operation::verifyPosition() {
    float robot_x;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        robot_x = robot_->x;
    }
    float line_x = field_line_.getX();

    if ((robot_x >= line_x - linear_threshold_) && (robot_x <= line_x + linear_threshold_)) out_of_place_ = NO;
    else if (robot_x > line_x + linear_threshold_) out_of_place_ = AHEAD;
    else if (robot_x < line_x - linear_threshold_) out_of_place_ = BEHIND;
}

void Operation::setMotion() {
    float robot_x, robot_y, robot_angle, ball_x, ball_y;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        robot_x = robot_->x;
        robot_y = robot_->y;
        robot_angle = robot_->angle;
        ball_x = ball_->x;
        ball_y = ball_->y; 
    }

    if (canKick(robot_x, robot_y, ball_x, ball_y)) setKick(robot_y, ball_y);
    else {
        std::lock_guard<std::mutex> lock(mutex_);
        robot_->linear_velocity = 0;
        robot_->angular_velocity = 0;
    }
}

void Operation::serialize() {
    std::vector<uint8_t> buffer = std::vector<uint8_t>(5, 0);
    std::lock_guard<std::mutex> lock(mutex_);
    
    buffer[ROBOT_ID] = (uint8_t)robot_->id;
    buffer[LINEAR_VELOCITY] = (uint8_t)robot_->linear_velocity;
    buffer[ANGULAR_VELOCITY] = (uint8_t)robot_->angular_velocity;
    buffer[LINEAR_DIRECTION] = (uint8_t)robot_->linear_direction;
    buffer[ANGULAR_DIRECTION] = (uint8_t)robot_->angular_direction;
    
    if (serial_repo_->getStatus(GK) == true) serial_repo_->setPackage(GK, buffer);
}

bool Operation::canKick(float robot_x, float robot_y, float ball_x, float ball_y) {
    switch (side_) {
        case LEFT:
            return ((ball_x > robot_x) && ((ball_x - robot_x) <= max_ball_distance_) && (abs(ball_y - robot_y) <= max_ball_distance_));
        case RIGHT:
            return ((ball_x > robot_x) && ((ball_x - robot_x) <= max_ball_distance_) && (abs(ball_y - robot_y) <= max_ball_distance_));
    }
}

void Operation::setKick(float robot_y, float ball_y) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    robot_->linear_velocity = 0;
    robot_->angular_velocity = kick_velocity_;
    
    switch (side_) {
        case LEFT:
            if (ball_y > robot_y) robot_->angular_direction = POSITIVE;
            else robot_->angular_direction = NEGATIVE;
            break;
        case RIGHT:
            if (ball_y < robot_y) robot_->angular_direction = POSITIVE;
            else robot_->angular_direction = NEGATIVE;
            break;
    }
}

} // namespace operations
} // namespace vss_furgbol