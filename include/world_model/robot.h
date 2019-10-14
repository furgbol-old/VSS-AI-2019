// ® Copyright FURGBot 2019


#include "Domain/Robot.h"


#ifndef ROBOT_H
#define ROBOT_H


namespace vss_furgbol {
namespace world_model {

class Robot : public vss::Robot {
    public:
        int id;

        Robot();
        ~Robot();

        void operator=(Robot robot);
        void operator=(vss::Robot robot);

        friend std::ostream& operator<<(std::ostream& os, const Robot& robot);
};

} // namespace world_model
} // namespace vss_furgbol


#endif // ROBOT_H