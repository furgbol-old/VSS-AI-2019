// ® Copyright FURGBot 2019


#ifndef SYSTEM_H
#define SYSTEM_H


#include "io/serial_sender.h"
#include "world_model/world_model.h"
#include "operation/gk_operation.h"
#include "operation/cb_operation.h"
#include "operation/st_operation.h"

#include <thread>


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
        bool configured_;

        world_model::WorldModel *world_model_;

        operation::GKOperation *gk_operator_;
        std::thread gk_thread_;
        std::mutex gk_mutex_;
        bool gk_is_running_;
        bool gk_status_changed_;

        operation::CBOperation *cb_operator_;
        std::thread cb_thread_;
        std::mutex cb_mutex_;
        bool cb_is_running_;
        bool cb_status_changed_;

        operation::STOperation *st_operator_;
        std::thread st_thread_;
        std::mutex st_mutex_;
        bool st_is_running_;
        bool st_status_changed_;
        
        io::SerialSender *serial_sender_;
        std::thread serial_thread_;
        std::mutex serial_mutex_;
        bool serial_is_running_;
        bool serial_is_paused_;
        bool serial_status_changed_;

        void exec();
        void end();

        void clearScreen();

    public:
        System();
        System(world_model::WorldModel *world_model);
        ~System();

        void init();
};

}
} // namespace furgbol


#endif // SYSTEM_H