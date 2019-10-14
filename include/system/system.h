// ® Copyright FURGBot 2019


#ifndef SYSTEM_H
#define SYSTEM_H


#include "io/serial_sender.h"
#include "io/tcp_receiver.h"
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
        bool configured_;

        world_model::WorldModel *world_model_;

        io::SerialSender *serial_sender_;
        std::thread serial_thread_;
        std::mutex serial_mutex_;

        io::TCPReceiver *tcp_receiver_;
        std::thread tcp_thread_;
        std::mutex tcp_mutex_;

        operation::GKOperation *gk_operator_;
        std::thread gk_thread_;
        std::mutex gk_mutex_;

        operation::CBOperation *cb_operator_;
        std::thread cb_thread_;
        std::mutex cb_mutex_;

        operation::STOperation *st_operator_;
        std::thread st_thread_;
        std::mutex st_mutex_;

        void createGKOperator();
        void createCBOperator();
        void createSTOperator();
        void createTCPReceiver();
        void createSerialSender();

        void configure();

        void exec();
        void end();
    
    public:
        System(world_model::WorldModel *world_model);
        ~System();

        void init();
};

} // namespace system
} // namespace vss_furgbol


#endif // SYSTEM_H