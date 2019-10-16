// ® Copyright FURGBot 2019


#ifndef RECEIVER_H
#define RECEIVER_H


#include "world_model/world_model.h"

#include "Communications/StateReceiver.h"

#include <mutex>


namespace vss_furgbol {
namespace io {

class Receiver {
    private:
        vss::State state_;
        vss::IStateReceiver *state_receiver_;

        world_model::WorldModel *world_model_;

        bool *running_;
        bool *changed_;

        std::mutex mutex_;

        void setConfigurations();
        void printConfigurations();

        void exec();
        void end();

    public:
        Receiver(bool *running, bool *changed, world_model::WorldModel *world_model);
        ~Receiver();

        void init();
};

} // namespace io
} // namespace vss_furgbol


#endif // RECEIVER_H