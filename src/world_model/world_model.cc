// ® Copyright FURGBot 2019


#include "world_model/world_model.h"


namespace vss_furgbol {
namespace world_model {

WorldModel::WorldModel() : ball_() {
    Robot gk_blue;
    Robot cb_blue;
    Robot st_blue;

    Robot gk_yellow;
    Robot cb_yellow;
    Robot st_yellow;

    std::vector<Robot> team_blue = {gk_blue, cb_blue, st_blue};
    std::vector<Robot> team_yellow = {gk_yellow, cb_yellow, st_yellow};
    robots_ = {team_blue, team_yellow};
}

WorldModel::~WorldModel() {}

} // namespace world_model
} // namespace vss_furgbol