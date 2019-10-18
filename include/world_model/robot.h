// ® Copyright FURGBot 2019


#ifndef ROBOT_H
#define ROBOT_H


#include "geometry/field_line.h"

#include "Domain/Robot.h"


namespace vss_furgbol {
namespace world_model {

class Robot : public vss::Robot {
    public:
        int id;
        int linear_velocity;
        int angular_velocity;
        int linear_direction;
        int angular_direction;

        Robot();
        ~Robot();

        void operator=(Robot robot);
        void operator=(vss::Robot robot);

        friend std::ostream& operator<<(std::ostream& os, const Robot& robot);
};

} // namespace world_model
} // namespace vss_furgbol


#endif // ROBOT_H