// ® Copyright FURGBot 2019


#include "geometry/vector_2d.h"
#include "labels/labels.h"
#include "operations/striker_operation.h"


namespace vss_furgbol {
namespace operations {

StrikerOperation::StrikerOperation(vss::Ball *ball, world_model::Robot *robot, int side, communications::SerialRepository *serial_repo) {
    ball_ = ball;
    robot_ = robot;
    side_ = side;
    serial_repo_ = serial_repo;
}

StrikerOperation::~StrikerOperation() {}

void StrikerOperation::init() {
    configure("striker");
    Operation::init();
}

void StrikerOperation::setTarget() {
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
        switch (side_) {
            case LEFT:
                target_.y = geometry::Vector2D(
                    vss::Point(ball_->x, ball_->y),
                    vss::Point(160, 65)
                ).getReferentY(robot_->x);
                break;
            case RIGHT:
                target_.y = geometry::Vector2D(
                    vss::Point(160, 65),
                    vss::Point(ball_->x, ball_->y)
                ).getReferentY(robot_->x);
                break;
        }
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