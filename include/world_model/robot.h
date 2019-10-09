// ® Copyright FURGBot 2019


#ifndef ROBOT_H
#define ROBOT_H


#include "Domain/Robot.h"


namespace vss_furgbol {
namespace world_model {

class Robot : public vss::Robot {
    private:
        int id_;

    public:
        Robot();
        Robot(int id);
        ~Robot();

        //getters
        int getId();

        //setters
        void setId(int id);

        //operators
        void operator=(Robot robot);
};

} // namespace world_model
} // namespace vss_furgbol


#endif // ROBOT_H