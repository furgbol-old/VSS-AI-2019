// ® Copyright FURGBot 2019


#include "world_model/world_model.h"

#include <thread>


int main() {
    vss_furgbol::world_model::WorldModel *world_model = new vss_furgbol::world_model::WorldModel();
    std::thread world_model_thread = std::thread(&vss_furgbol::world_model::WorldModel::init, world_model);
    world_model_thread.join();
    return 0;
}