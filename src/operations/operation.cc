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

    linear_velocity_gain_ = json_file[which_player]["linear velocity gain"];
    angular_velocity_gain_ = json_file[which_player]["angular velocity gain"];
    linear_discrepancy_factor = json_file[which_player]["linear discrepancy factor"];
    angular_discrepancy_factor = json_file[which_player]["angular discrepancy factor"];
    linear_threshold_ = json_file[which_player]["linear threshold"];
    angular_threshold_ = json_file[which_player]["angular threshold"];
    max_ball_distance_ = json_file[which_player]["max ball distance"];
    max_linear_velocity_ = json_file[which_player]["max linear velocity"];
    max_angular_velocity_ = json_file[which_player]["max angular velocity"];
    min_linear_velocity_ = json_file[which_player]["min linear velocity"];
    min_angular_velocity_ = json_file[which_player]["min angular velocity"];
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
    // else if (outOfAngle(robot_angle)) fixesAngle(robot_angle);
    // else if (outOfTarget(robot_x, robot_y)) goToTarget(robot_x, robot_y);
    else stopRobot();
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

bool Operation::outOfAngle(float robot_angle) {
    if (target_.angle == 0) {
        if ((robot_angle >= (360 - angular_threshold_)) || (robot_angle <= (target_.angle + angular_threshold_)))
            return false;
        return true;
    } else {
        if ((robot_angle >= (target_.angle - angular_threshold_)) && (robot_angle <= (target_.angle + angular_threshold_)))
            return false;
        return true;
    }
}

bool Operation::outOfTarget(float robot_x, float robot_y) {
    return ((robot_x < (target_.x - linear_threshold_)) || (robot_x > (target_.x + linear_threshold_))
        || (robot_y < (target_.y - linear_threshold_)) || (robot_y > (target_.y + linear_threshold_)));
}

void Operation::setKick(float robot_y, float ball_y) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    robot_->linear_velocity = 0;
    robot_->angular_velocity = kick_velocity_;
    
    switch (side_) {
        case LEFT:
            if (ball_y > robot_y) robot_->angular_direction = CLOCKWISE;
            else robot_->angular_direction = COUNTERCLOCKWISE;
            break;
        case RIGHT:
            if (ball_y < robot_y) robot_->angular_direction = CLOCKWISE;
            else robot_->angular_direction = COUNTERCLOCKWISE;
            break;
    }
}

void Operation::fixesAngle(float robot_angle) {
    std::lock_guard<std::mutex> lock(mutex_);

    robot_->linear_velocity = 0;
    robot_->angular_velocity = calculateAngularVelocity(robot_angle);

    switch (getQuadrant(robot_angle)) {
        case FIRST:
            if (target_.angle == 180) robot_->angular_direction = COUNTERCLOCKWISE;
            else robot_->angular_direction = CLOCKWISE;
            break;
        case SECOND:
            if (target_.angle == 0) robot_->angular_direction = CLOCKWISE;
            else robot_->angular_direction = COUNTERCLOCKWISE;
            break;
        case THIRD:
            if (target_.angle == 180) robot_->angular_direction = CLOCKWISE;
            else robot_->angular_direction = COUNTERCLOCKWISE;
            break;
        case FOURTH:
            if (target_.angle == 0) robot_->angular_direction = COUNTERCLOCKWISE;
            else robot_->angular_direction = CLOCKWISE;
            break;
    }
}

void Operation::goToTarget(float robot_x, float robot_y) {
    std::lock_guard<std::mutex> lock(mutex_);

    robot_->angular_velocity = 0;
    robot_->linear_velocity = calculateLinearVelocity(robot_x, robot_y);

    if (out_of_place_ == NO) {
        if (robot_y < target_.y) robot_->linear_direction = FORWARD;
        else robot_->linear_direction = BACK;
    } else robot_->linear_direction = FORWARD;
}

void Operation::stopRobot() {
    std::lock_guard<std::mutex> lock(mutex_);
    robot_->angular_velocity = 0;
    robot_->linear_velocity = 0;
}

int Operation::calculateAngularVelocity(float robot_angle) {
    float angular_velocity;

    if (target_.angle == 0) {
        if (robot_angle > 180)
            angular_velocity = angular_velocity_gain_ * ((360 - robot_angle) / angular_discrepancy_factor);
        else
            angular_velocity = angular_velocity_gain_ * (robot_angle / angular_discrepancy_factor);
    } else if (target_.angle == 90) {
        if (robot_angle < 90)
            angular_velocity = angular_velocity_gain_ * (robot_angle / angular_discrepancy_factor);
        else if (robot_angle > 270)
            angular_velocity = angular_velocity_gain_ * ((450 - robot_angle) / angular_discrepancy_factor);
        else
            angular_velocity = angular_velocity_gain_ * ((robot_angle - 90) / angular_discrepancy_factor);
    } else if (target_.angle == 180)
        angular_velocity = angular_velocity_gain_ * (abs(robot_angle - 180) / angular_discrepancy_factor);

    if (angular_velocity < min_angular_velocity_) angular_velocity = min_angular_velocity_;
    else if (angular_velocity > max_angular_velocity_) angular_velocity = max_angular_velocity_;

    return ((int)angular_velocity);
}

int Operation::calculateLinearVelocity(float robot_x, float robot_y) {
    float linear_velocity;

    if (out_of_place_ == NO)
        linear_velocity = linear_velocity_gain_ * (abs(robot_y - target_.x) / linear_discrepancy_factor);
    else
        linear_velocity = linear_velocity_gain_ * (abs(robot_x - target_.x) / linear_discrepancy_factor);

    if (linear_velocity < min_linear_velocity_) linear_velocity = min_linear_velocity_;
    else if (linear_velocity > max_linear_velocity_) linear_velocity = max_linear_velocity_;

    return ((int)linear_velocity);
}

int Operation::getQuadrant(float robot_angle) {
    if ((robot_angle > 0) && (robot_angle <= 90)) return FOURTH;
    else if ((robot_angle > 90) && (robot_angle <= 180)) return THIRD;
    else if ((robot_angle > 180) && (robot_angle <= 270)) return SECOND;
    else if ((robot_angle > 270) && (robot_angle < 360)) return FIRST;
}

} // namespace operations
} // namespace vss_furgbol