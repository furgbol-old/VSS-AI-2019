// ® Copyright FURGBot 2019


#ifndef SYSTEM_H
#define SYSTEM_H


#include "io/sender.h"
#include "io/receiver.h"
#include "operation/cb_operation.h"
#include "operation/gk_operation.h"
#include "operation/st_operation.h"
#include "world_model/world_model.h"

#include <mutex>
#include <thread>


namespace vss_furgbol {
namespace system {

class System {
    private:
        int team_color_;
        int side_;
        int execution_mode_;
        int max_robots_velocity_;
        bool configured_;

        world_model::WorldModel *world_model_;

        io::Sender *serial_sender_;
        std::thread serial_thread_;
        std::mutex serial_mutex_;
        bool serial_is_running_;
        bool serial_changed_;

        io::Receiver *tcp_receiver_;
        std::thread tcp_thread_;
        std::mutex tcp_mutex_;
        bool tcp_is_running_;
        bool tcp_changed_;

        operation::GKOperation *gk_operator_;
        std::thread gk_thread_;
        std::mutex gk_mutex_;
        bool gk_is_running_;
        bool gk_changed_;

        operation::CBOperation *cb_operator_;
        std::thread cb_thread_;
        std::mutex cb_mutex_;
        bool cb_is_running_;
        bool cb_changed_;

        operation::STOperation *st_operator_;
        std::thread st_thread_;
        std::mutex st_mutex_;
        bool st_is_running_;
        bool st_changed_;

        void startGKOperator();
        void startCBOperator();
        void startSTOperator();
        void startTCPReceiver();
        void startSerialSender();

        void stopGKOperator();
        void stopCBOperator();
        void stopSTOperator();
        void stopTCPReceiver();
        void stopSerialSender();

        void configure();

        void exec();
        void end();

        void clearScreen();
    
    public:
        System(world_model::WorldModel *world_model);
        ~System();

        void init();
};

} // namespace system
} // namespace vss_furgbol


#endif // SYSTEM_H