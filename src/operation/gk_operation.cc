// ® Copyright FURGBot 2019


#include "operation/gk_operation.h"

#include "json.hpp"

#include <fstream>


namespace vss_furgbol {
namespace operation {

GKOperation::GKOperation(bool *running, bool *changed, vss::Ball *ball, world_model::Robot *robot, int side) :
    ball_(ball), robot_(robot), side_(side), sending_queue(), running_(running), changed_(changed) {}

GKOperation::~GKOperation() {}

void GKOperation::init() {
    configure();
    printConfigurations();
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

void GKOperation::configure() {
    std::cout << "[STATUS]: Configuring goalkeeper operator..." << std::endl;

    std::ifstream ifstream("config/operation.json");
    nlohmann::json json_file;
    ifstream >> json_file;

    max_queue_size_ = json_file["max queue size"];
    velocity_gain_ = json_file["velocity gain"];
    error_threshold_ = json_file["error threshold"];

    robot_->id = json_file["robots"]["goalkeeper"]["id"];
    max_velocity_ = json_file["robots"]["goalkeeper"]["max velocity"];
    max_ball_distance_ = json_file["robots"]["goalkeeper"]["max ball distance"];

    switch (side_) {
        case LEFT:
            field_line_.setX(json_file["robots"]["goalkeeper"]["field line"]["x"]);
            break;
        case RIGHT:
            field_line_.setX(170 - (float)json_file["robots"]["goalkeeper"]["field line"]["x"]);;
            break;
    }
    field_line_.setMinY(json_file["robots"]["goalkeeper"]["field line"]["min y"]);
    field_line_.setMaxY(json_file["robots"]["goalkeeper"]["field line"]["max y"]);
}

void GKOperation::printConfigurations() {
    std::cout << "[STATUS]: Goalkeeper operator configuration done!" << std::endl;

    std::cout << "-> Configurations:" << std::endl;
    std::cout << "Max queue size: " << max_queue_size_ << std::endl;
    std::cout << "Velocity gain: " << velocity_gain_ << std::endl;
    std::cout << "Vision error: " << error_threshold_ << std::endl;
    std::cout << "Robot:" << std::endl;
    std::cout << "\tID: " << robot_->id << std::endl;
    std::cout << "\tMax ball distante: " << max_ball_distance_ << std::endl;
    std::cout << "\tMax velocity: " << max_velocity_ << std::endl;
    std::cout << "\tField line:" << std::endl;
    std::cout << "\t\tX: " << field_line_.getX() << std::endl;
    std::cout << "\t\tMin Y: " << field_line_.getMinY() << std::endl;
    std::cout << "\t\tMax Y: " << field_line_.getMaxY() << std::endl;
    std::cout << std::endl;
}

void GKOperation::exec() {
    bool previous_status = false;
    while (true) {
        while (*running_) {
            if (previous_status == false) {
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    *changed_ = true;
                }
            }
            verifyPosition();
            setTarget();
            setMotion();
            serialize();

            /*{
                std::lock_guard<std::mutex> lock(mutex_);
                std::cout << *ball_ << std::endl;
                std::cout << *robot_ << std::endl;
            }*/
            // switch (out_of_place_) {
            //     case NO:
            //         std::cout << "In the right place!" << std::endl;
            //         break;
            //     case AHEAD:
            //         std::cout << "Ahead!" << std::endl;
            //         break;
            //     case BEHIND:
            //         std::cout << "Behind!" << std::endl;
            //         break;
            // }
            //std::cout << "Target Position: (" << target_.x << ", " << target_.y << ")" << std::endl;
            //std::cout << "Target Angle: " << target_angle_ << std::endl;
        }

        if (!*running_) {
            end();
            if (previous_status == true) {
                previous_status = false;
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    *changed_ = true;
                }
            }
            break;
        }
    }
}

void GKOperation::end() {
    std::cout << "[STATUS]: Closing goalkeeper operation..." << std::endl;
}

void GKOperation::verifyPosition() {
    float robot_x;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        robot_x = robot_->x;
    }
    float line_x = field_line_.getX();
    //std::cout << "Robot X: " << robot_x << std::endl;
    //std::cout << "Line X: " << line_x << std::endl;

    if ((robot_x >= line_x - error_threshold_) && (robot_x <= line_x + error_threshold_)) out_of_place_ = NO;
    else if (robot_x > line_x + error_threshold_) out_of_place_ = AHEAD;
    else if (robot_x < line_x - error_threshold_) out_of_place_ = BEHIND;
}


void GKOperation::setTarget() {
    float robot_x, robot_y, ball_x, ball_y;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        robot_x = robot_->x;
        robot_y = robot_->y;
        ball_x = ball_->x;
        ball_y = ball_->y; 
    }

    if (out_of_place_ == NO) {
        target_angle_ = 90;
        target_.x = robot_x;
        target_.y = ball_y;
        if (target_.y < field_line_.getMinY()) target_.y = field_line_.getMinY();
        else if (target_.y > field_line_.getMaxY()) target_.y = field_line_.getMaxY();
    } else {
        target_.x = field_line_.getX();
        target_.y = robot_y;
        if (out_of_place_ == BEHIND) target_angle_ = 0;
        else if (out_of_place_ == AHEAD) target_angle_ = 180;
    }
}

void GKOperation::setMotion() {
    float robot_x, robot_y, robot_angle, ball_x, ball_y;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        robot_x = robot_->x;
        robot_y = robot_->y;
        robot_angle = robot_->angle;
        ball_x = ball_->x;
        ball_y = ball_->y; 
    }

    linear_velocity_ = 0;
    angular_velocity_ = 0;
    linear_direction_ = 0;
    angular_direction_ = 0;

    if (canKick(robot_x, robot_y, ball_x, ball_y)) {
        //std::cout << "Robot can kick";
        setKick(robot_y, ball_y);
    } else if (outOfAngle(robot_angle)) {
        //std::cout << "Robot is out of angle! Have to turn";
        fixesAngle(robot_angle);
    } else if ((target_angle_ != 90) && (out_of_place_ == NO)) {
        //std::cout << "Robot have to get back to 90 degrees." << std::endl;
        target_angle_ = 90;
        linear_velocity_ = 0;
        angular_velocity_ = 0;
    } else if (outOfTarget(robot_x, robot_y, ball_x, ball_y)) {
        goToTarget(robot_x, robot_y, ball_x, ball_y);
        //std::cout << "Robot is out of target. Have to run";
    } else {
        //std::cout << "Robot is on the right place." << std::endl;
    }
}

void GKOperation::serialize() {
    buffer_to_send_ = std::vector<uint8_t>(5, 0);

    {
        std::lock_guard<std::mutex> lock(mutex_);
        buffer_to_send_[ROBOT_ID] = (uint8_t)robot_->id;
    }
    buffer_to_send_[LINEAR_VELOCITY] = (uint8_t)linear_velocity_;
    buffer_to_send_[ANGULAR_VELOCITY] = (uint8_t)angular_velocity_;
    buffer_to_send_[LINEAR_DIRECTION] = (uint8_t)linear_direction_;
    buffer_to_send_[ANGULAR_DIRECTION] = (uint8_t)angular_direction_;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (sending_queue.size() == max_queue_size_) sending_queue.pop();
        sending_queue.push(buffer_to_send_);
    }

    // std::cout << "Have to send: {" << std::endl;
    // std::cout << "\tRobot ID: " << (int)sending_queue.front()[ROBOT_ID] << std::endl;
    // std::cout << "\tLinear Velocity:" << (int)sending_queue.front()[LINEAR_VELOCITY] << std::endl;
    // std::cout << "\tAngular Velocity: " << (int)sending_queue.front()[ANGULAR_VELOCITY] << std::endl;
    // std::cout << "\tLinear Direction: " << (int)sending_queue.front()[LINEAR_DIRECTION] << std::endl;
    // std::cout << "\tAngular Direction: " << (int)sending_queue.front()[ANGULAR_DIRECTION] << std::endl;
    // std::cout << "}" << std::endl;
}

bool GKOperation::canKick(float robot_x, float robot_y, float ball_x, float ball_y) {
    switch (side_) {
        case LEFT:
            return ((ball_x > robot_x) && ((ball_x - robot_x) <= max_ball_distance_) && (abs(ball_y - robot_y) <= max_ball_distance_));
        case RIGHT:
            return ((ball_x > robot_x) && ((ball_x - robot_x) <= max_ball_distance_) && (abs(ball_y - robot_y) <= max_ball_distance_));
    }
}

bool GKOperation::outOfAngle(float robot_angle) {
    if (target_angle_ == 0) {
        if ((robot_angle < (360 - error_threshold_)) || (robot_angle > (target_angle_ + error_threshold_)))
            return true;
        else
            return false;
        
    } else
        return ((robot_angle < (target_angle_ - error_threshold_)) || ((robot_angle > (target_angle_ + error_threshold_))));
}

bool GKOperation::outOfTarget(float robot_x, float robot_y, float ball_x, float ball_y) {
    return ((robot_x < (target_.x - error_threshold_)) || (robot_x > (target_.x + error_threshold_)) || 
            (robot_y < (target_.y - error_threshold_)) || (robot_y > (target_.y + error_threshold_)));
}

void GKOperation::setKick(float robot_y, float ball_y) {
    linear_velocity_ = 0;
    angular_velocity_ = max_velocity_;
    
    switch (side_) {
        case LEFT:
            if (ball_y > robot_y) {
                angular_direction_ = POSITIVE;
                //std::cout << " in counterclockwise." << std::endl;
            } else {
                angular_direction_ = NEGATIVE;
                //std::cout << " in clockwise." << std::endl;
            }
            break;
        case RIGHT:
            if (ball_y < robot_y) {
                angular_direction_ = POSITIVE;
                //std::cout << " in counterclockwise." << std::endl;
            } else {
                angular_direction_ = NEGATIVE;
                //std::cout << " in clockwise." << std::endl;
            }
            break;
    }
}

void GKOperation::fixesAngle(float robot_angle) {
    linear_velocity_ = 0;
    angular_velocity_ = velocity_gain_ * abs(robot_angle - target_angle_);
    if (angular_velocity_ > max_velocity_) angular_velocity_ = max_velocity_;

    //std::cout << " with angular velocity = " << angular_velocity_;
    if (robot_angle < target_angle_) {
        angular_direction_ = NEGATIVE;
        //std::cout << " in clockwise." << std::endl;
    } else if (robot_angle > target_angle_) {
        angular_direction_ = POSITIVE;
        //std::cout << " in counterclockwise." << std::endl;
    }
}

void GKOperation::goToTarget(float robot_x, float robot_y, float ball_x, float ball_y) {
    angular_velocity_ = 0;
    if (out_of_place_ == NO) {
        linear_velocity_ = (int)(velocity_gain_ * abs(robot_y - target_.y));
        if (linear_velocity_ > max_velocity_) linear_velocity_ = max_velocity_;
        if (robot_y > target_.y) linear_direction_ = NEGATIVE;
        else if (robot_y < target_.y) linear_direction_ = POSITIVE;
    } else {
        linear_velocity_ = (int)(velocity_gain_ * abs(robot_x - target_.x));
        if (linear_velocity_ > max_velocity_) linear_velocity_ = max_velocity_;
        linear_direction_ = POSITIVE;
    }

    // std::cout << " with linear velocity = " << linear_velocity_;
    // if (linear_direction_ == POSITIVE) std::cout << " forward." << std::endl;
    // if (linear_direction_ == NEGATIVE) std::cout << " back." << std::endl;
}

} // namespace operation
} // namespace vss_furgbol