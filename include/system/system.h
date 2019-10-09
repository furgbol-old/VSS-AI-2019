// ® Copyright FURGBot 2019


#ifndef SYSTEM_H
#define SYSTEM_H


#include "io/serial_sender.h"
#include "world_model/world_model.h"


namespace vss_furgbol {
namespace system {

enum TeamLabels {
    ENEMY, FRIENDLY
};

enum TeamColorLabels {
    BLUE, YELLOW
};

enum RoleLabels {
    GK, CB, ST //GoalKeeper, CenterBack, STriker
};

class System {
    private:
        int team_color_;

        world_model::WorldModel *world_model_;
        
        io::SerialSender *serial_sender_;
        std::thread serial_sender_thread_;
        std::mutex serial_sender_mutex_;
        bool serial_is_running_;
        bool serial_is_paused_;
        bool serial_status_changed_;

        void exec();
        void end();

        void clearScreen();

    public:
        System();
        ~System();

        void init();
};

}
} // namespace furgbol


#endif // SYSTEM_H