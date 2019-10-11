// ® Copyright FURGBot 2019


#ifndef TCP_RECEIVER_H
#define TCP_RECEIVER_H


#include "labels/labels.h"
#include "world_model/world_model.h"

#include "Communications/StateReceiver.h"

#include <mutex>


namespace vss_furgbol {
namespace io {

class TCPReceiver {
    private:
        vss::State state_;
        vss::IStateReceiver *state_receiver_;

        world_model::WorldModel *world_model_;

        bool *running_;
        bool *status_changed_;
        std::mutex mutex_;

        void setConfigurations();
        void printConfigurations();

        void exec();
        void end();

    public:
        TCPReceiver();
        TCPReceiver(world_model::WorldModel *world_model, bool *running, bool *status_changed);
        ~TCPReceiver();

        void init();
};

} // namespace io
} // namespace vss_furgbol


#endif // TCP_RECEIVER_H