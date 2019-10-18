// ® Copyright FURGBot 2019


#ifndef WORLD_MODEL_H
#define WORLD_MODEL_H


#include "world_model/robot.h"

#include "Domain/Ball.h"

#include <vector>


namespace vss_furgbol {
namespace world_model {

class WorldModel {
    public:
        WorldModel();
        ~WorldModel();

        vss::Ball ball;
        std::vector<Robot> team_blue;
        std::vector<Robot> team_yellow;

        friend std::ostream& operator<<(std::ostream& os, const WorldModel& world_model);
};

} // namespace world_model
} // namespace vss_furgbol


#endif // WORLD_MODEL_H