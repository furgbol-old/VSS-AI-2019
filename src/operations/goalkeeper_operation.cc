// ® Copyright FURGBot 2019


#include "labels/labels.h"
#include "operations/goalkeeper_operation.h"


namespace vss_furgbol {
namespace operations {

GoalkeeperOperation::GoalkeeperOperation(vss::Ball *ball, world_model::Robot *robot, int side, communications::SerialRepository *serial_repo) {
    ball_ = ball;
    robot_ = robot;
    side_ = side;
    serial_repo_ = serial_repo;
}

GoalkeeperOperation::~GoalkeeperOperation() {}

void GoalkeeperOperation::init() {
    configure("goalkeeper");
    Operation::init();
}

void GoalkeeperOperation::run() {
    while (true) {
        // verifyPosition();
        // setTarget();
        setMotion();
        serialize();
    }
}

void GoalkeeperOperation::setTarget() {
    float robot_x, robot_y, ball_x, ball_y;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        robot_x = robot_->x;
        robot_y = robot_->y;
        ball_x = ball_->x;
        ball_y = ball_->y; 
    }

    if (out_of_place_ == NO) {
        target_.angle = 90;
        target_.x = robot_x;
        target_.y = ball_y;
        if (target_.y < field_line_.getMinY()) target_.y = field_line_.getMinY();
        else if (target_.y > field_line_.getMaxY()) target_.y = field_line_.getMaxY();
    } else {
        target_.x = field_line_.getX();
        target_.y = robot_y;
        if (out_of_place_ == BEHIND) target_.angle = 0;
        else if (out_of_place_ == AHEAD) target_.angle = 180;
    }
}

} // namespace operations
} // namespace vss_furgbol