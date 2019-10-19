// ® Copyright FURGBot 2019


#include "world_model/world_model.h"


namespace vss_furgbol {
namespace world_model {

WorldModel::WorldModel() {
    ball = vss::Ball();
    team_blue = std::vector<Robot>(3, Robot());
    team_yellow = std::vector<Robot>(3, Robot());
}

WorldModel::~WorldModel() {}

std::ostream& operator<<(std::ostream& os, const WorldModel& world_model) {
    std::cout << "World Model:" << std::endl;
    std::cout << "\t" << "Ball: (" << world_model.ball.x << ", " << world_model.ball.y << ")" << std::endl;
    std::cout << "\t" << "Team Blue:" << std::endl;
    for (int i = 0; i < 3; i++) {
        std::cout << "\t\t" << "Robot " << i + 1 << ":" << std::endl;
        std::cout << "\t\t\t" << "ID: " << world_model.team_blue[i].id << std::endl;
        std::cout << "\t\t\t" << "Position: (" << world_model.team_blue[i].x << ", " << world_model.team_blue[i].y << std::endl;
        std::cout << "\t\t\t" << "Angle: " << world_model.team_blue[i].angle << std::endl;
    }
    std::cout << "\t" << "Team Yellow:" << std::endl;
    for (int i = 0; i < 3; i++) {
        std::cout << "\t\t" << "Robot " << i + 1 << ":" << std::endl;
        std::cout << "\t\t\t" << "ID: " << world_model.team_yellow[i].id << std::endl;
        std::cout << "\t\t\t" << "Position: (" << world_model.team_yellow[i].x << ", " << world_model.team_yellow[i].y << std::endl;
        std::cout << "\t\t\t" << "Angle: " << world_model.team_yellow[i].angle << std::endl;
    }
}

} // namespace world_model
} // namespace vss_furgbol