// ® Copyright FURGBot 2019


#include "operation/st_operation.h"

#include "json.hpp"

#include <fstream>


namespace vss_furgbol {
namespace operation {

STOperation::STOperation() : sending_queue_() {}

STOperation::STOperation(world_model::Robot *robot, vss::Ball *ball, bool *running, bool *status_changed) : robot_(robot), ball_(ball),
    sending_queue_(), status_changed_(status_changed), running_(running) {}

STOperation::~STOperation() {}

void STOperation::init() {
    setConfigurations();
    printConfigurations();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        *status_changed_ = true;
    }

    exec();

    *status_changed_ = false;
    end();
}

void STOperation::end() {
    std::cout << "[STATUS]: Closing operator..." << std::endl;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        *status_changed_ = true;
    }    
}

void STOperation::exec() {
    while (*running_) {
        verifyPosition();
        setTarget();
        setMotion();
        serialize();
    }
}

void STOperation::verifyPosition() {
    float robot_x = robot_->x;
    float line_x = field_line_.getX();

    if ((robot_x >= line_x - vision_error_) && (robot_x <= line_x + vision_error_)) out_of_place_ = NO;
    else if (robot_x > line_x + vision_error_) out_of_place_ = AHEAD;
    else if (robot_x < line_x - vision_error_) out_of_place_ = BEHIND;
}

void STOperation::setMotion() {
    if (((ball_->x - robot_->x) > 0) && ((ball_->x - robot_->x) < max_ball_distance_) && (abs(ball_->y - robot_->y) < max_ball_distance_)) {
        linear_velocity_ = 0;
        angular_velocity_ = max_velocity_;
        if (ball_->y > robot_->y) angular_direction_ = POSITIVE;
        else angular_direction_ = NEGATIVE;
    } else {
        if ((robot_->angle >= target_angle_ - vision_error_) && (robot_->angle <= target_angle_ + vision_error_)) {
            if (target_angle_ != 0) {
                target_angle_ = 0;
                linear_velocity_ = 0;
                angular_velocity_ = 0;
            } else { 
                if (((robot_->x <= target_.x - vision_error_) || (robot_->x >= target_.x + vision_error_)) || ((robot_->y <= target_.y - vision_error_) || (robot_->y >= target_.y + vision_error_))) {
                    angular_velocity_ = 0;
                    if (out_of_place_ == NO) {
                        linear_velocity_ = (int)(velocity_k_ * abs(robot_->y - target_.y));
                        if (linear_velocity_ > max_velocity_) linear_velocity_ = max_velocity_;
                        if (robot_->y < target_.y) linear_direction_ = NEGATIVE;
                        else if (robot_->y > target_.y) linear_direction_ = POSITIVE;
                    } else {
                        linear_velocity_ = (int)(velocity_k_ * abs(robot_->x - target_.x));
                        if (linear_velocity_ > max_velocity_) linear_velocity_ = max_velocity_;
                        if (out_of_place_ == BEHIND) linear_direction_ = POSITIVE;
                        else if (out_of_place_ == AHEAD) linear_direction_ = NEGATIVE;
                    }
                } else linear_velocity_ = 0;
            }
        } else {
            linear_velocity_ = 0;
            angular_velocity_ = velocity_k_ * abs(robot_->angle - target_angle_);
            if (angular_velocity_ > max_velocity_) angular_velocity_ = max_velocity_;
            if (robot_->angle < target_angle_) angular_direction_ = NEGATIVE;
            if (robot_->angle > target_angle_) angular_direction_ = POSITIVE;
        }
    }
}

void STOperation::serialize() {
    buffer_to_send_ = std::vector<uint8_t>(5, 0);
    buffer_to_send_[ROBOT_ID] = robot_->getId() + 128;
    buffer_to_send_[LINEAR_VELOCITY] = linear_velocity_;
    buffer_to_send_[ANGULAR_VELOCITY] = angular_velocity_;
    buffer_to_send_[LINEAR_DIRECTION] = linear_direction_;
    buffer_to_send_[ANGULAR_DIRECTION] = angular_direction_;

    if (sending_queue_.size() > max_queue_size_) sending_queue_.pop();
    sending_queue_.push(buffer_to_send_);
}

std::queue<std::vector<uint8_t>> STOperation::getSendingQueue() { return sending_queue_; }

void STOperation::setConfigurations() {
    std::cout << "[STATUS]: Configuring striker operator..." << std::endl;

    std::ifstream _ifstream("config/operation.json");
    nlohmann::json json_file;
    _ifstream >> json_file;

    max_queue_size_ = json_file["max_queue_size"];
    velocity_k_ = json_file["velocity_gain"];
    vision_error_ = json_file["vision_error"];

    robot_->setId(json_file["robots"]["striker"]["id"]);
    field_line_.setX(json_file["robots"]["striker"]["field_line"]["x"]);
    field_line_.setMinY(json_file["robots"]["striker"]["field_line"]["min_y"]);
    field_line_.setMaxY(json_file["robots"]["striker"]["field_line"]["max_y"]);
    max_ball_distance_ = json_file["robots"]["striker"]["max_ball_distance"];
    max_velocity_ = json_file["robots"]["striker"]["max_velocity"];
}

void STOperation::printConfigurations() {
    std::cout << "[STATUS]: Striker operator configuration done!" << std::endl;

    std::cout << "-> Configurations:" << std::endl;
    std::cout << "Max queue size: " << max_queue_size_ << std::endl;
    std::cout << "Velocity gain: " << velocity_k_ << std::endl;
    std::cout << "Vision error: " << vision_error_ << std::endl;
    std::cout << "Robot:" << std::endl;
    std::cout << "\tID: " << robot_->getId() << std::endl;
    std::cout << "\tMax ball distante: " << max_ball_distance_ << std::endl;
    std::cout << "\tMax velocity: " << max_velocity_ << std::endl;
    std::cout << "\tField line:" << std::endl;
    std::cout << "\t\tX: " << field_line_.getX() << std::endl;
    std::cout << "\t\tMin Y: " << field_line_.getMinY() << std::endl;
    std::cout << "\t\tMax Y: " << field_line_.getMaxY() << std::endl;
    std::cout << std::endl;
}

void STOperation::setTarget() {
    if (out_of_place_ == NO) {
        target_angle_ = 90;
        target_.x = robot_->x;
        target_.y = geometry::Vector2D(
            vss::Point(ball_->x, ball_->y),
            vss::Point(160, 65)
        ).getReferentY(robot_->x);
        if (target_.y < field_line_.getMinY()) target_.y = field_line_.getMinY();
        else if (target_.y > field_line_.getMaxY()) target_.y = field_line_.getMaxY();
    } else {
        target_.x = field_line_.getX();
        target_.y = robot_->y;
        if (out_of_place_ == BEHIND) target_angle_ = 0;
        else if (out_of_place_ == AHEAD) target_angle_ = 180;
    }
}

} // namespace operation
} // namespace vss_furgbol