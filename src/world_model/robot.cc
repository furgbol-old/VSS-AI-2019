// ® Copyright FURGBot 2019


#include "world_model/robot.h"


namespace vss_furgbol {
namespace world_model {

Robot::Robot() {}

Robot::Robot(int id) : id_(id) {}

Robot::~Robot() {}

int Robot::getId() { return id_; }

void Robot::setId(int id) { id_ = id; }

void Robot::operator=(Robot robot) {
    id_ = robot.id_;
    x = robot.x;
    y = robot.y;
    angle = robot.angle;
    speedX = robot.speedX;
    speedY = robot.speedY;
    speedAngle = robot.speedAngle;
}

void Robot::operator=(vss::Robot robot) {
    x = robot.x;
    y = robot.y;
    angle = robot.angle;
    speedX = robot.speedX;
    speedY = robot.speedY;
    speedAngle = robot.speedAngle;
}

} // namespace world_model
} // namespace vss_furgbol