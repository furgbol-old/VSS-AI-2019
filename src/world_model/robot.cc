// ® Copyright FURGBot 2019


#include "world_model/robot.h"


namespace vss_furgbol {
namespace world_model {

Robot::Robot() {
    id = 0;
    x = 0;
    y = 0;
    angle = 0;
    speedAngle = 0;
    speedX = 0;
    speedY = 0;
}

Robot::~Robot() {}

void Robot::operator=(Robot robot) {
    id = robot.id;
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

std::ostream& operator<<(std::ostream& os, const Robot& robot) {
    std::cout << "Robot: " << std::endl;
    std::cout << "\t" << "ID: " << robot.id << std::endl;
    std::cout << "\t" << "Position: (" << robot.x << ", " << robot.y << ")" << std::endl;
    std::cout << "\t" << "Angle: " << robot.angle << std::endl;
}

} // namespace world_model
} // namespace vss_furgbol