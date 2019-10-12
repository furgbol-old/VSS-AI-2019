// ® Copyright FURGBot 2019


#include "world_model/world_model.h"
#include "system/system.h"

#include <thread>


int main() {
    vss_furgbol::world_model::WorldModel world_model;

    vss_furgbol::system::System *system = new vss_furgbol::system::System(&world_model);
    std::thread system_thread = std::thread(&vss_furgbol::system::System::init, system);
    system_thread.join();

    return 0;
}