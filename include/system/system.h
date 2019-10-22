// ® Copyright FURGBot 2019


#ifndef SYSTEM_H
#define SYSTEM_H


#include "communications/receiver.h"
#include "communications/serial_repository.h"
#include "communications/sender.h"
#include "operations/goalkeeper_operation.h"
#include "world_model/world_model.h"

#include <memory>
#include <mutex>
#include <thread>


namespace vss_furgbol {
namespace system {

class System {
    private:
        world_model::WorldModel *world_model_;
        communications::SerialRepository *serial_repo_;

        std::shared_ptr<communications::Receiver> receiver_;
        std::thread receiver_thread_;

        std::shared_ptr<communications::Sender> sender_;
        std::thread sender_thread_;

        std::shared_ptr<operations::GoalkeeperOperation> gk_operator_;
        std::thread gk_thread_;

        std::mutex mutex_;

        int side_;
        int team_color_;

        void configure();

        void startSender();
        void startGoalkeeperOperation();
        void startReceiver();

        world_model::Robot* getRobot(int which_player);

        void run();
        void end();

    public:
        System(world_model::WorldModel *world_model);
        ~System();

        void init();
};

} // namespace system
} // namespace vss_furgbol


#endif // SYSTEM_H