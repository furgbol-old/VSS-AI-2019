// ® Copyright FURGBot 2019


#include "system/system.h"
#include "world_model/world_model.h"


int main() {
    vss_furgbol::world_model::WorldModel world_model;

    vss_furgbol::system::System *system = new vss_furgbol::system::System(&world_model);
    system->init();

    return 0;
}