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
        WorldModel(vss::Ball ball, std::vector<std::vector<Robot>> robots);
        ~WorldModel();

        vss::Ball ball_;
        std::vector<std::vector<Robot>> robots_;
};

} // namespace worl_model
} // namespace vss_furgbol


#endif // WORLD_MODEL_H