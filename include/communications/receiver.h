// ® Copyright FURGBot 2019


#ifndef RECEIVER_H
#define RECEIVER_H


#include "world_model/world_model.h"

#include "Communications/StateReceiver.h"

#include <memory>
#include <mutex>
#include <vector>


namespace vss_furgbol {
namespace communications {

class Receiver {
    private:
        vss::State state_;
        std::shared_ptr<vss::IStateReceiver> state_receiver_;

        std::vector<std::vector<std::vector<float>>> robot_one_queues_;
        std::vector<std::vector<std::vector<float>>> robot_two_queues_;
        std::vector<std::vector<std::vector<float>>> robot_three_queues_;
        std::vector<float> ball_x_queue_;
        std::vector<float> ball_y_queue_;
        int max_queue_size_;

        std::vector<std::vector<std::vector<float>>> robot_one_ordered_queues_;
        std::vector<std::vector<std::vector<float>>> robot_two_ordered_queues_;
        std::vector<std::vector<std::vector<float>>> robot_three_ordered_queues_;
        std::vector<float> ball_x_ordered_queue_;
        std::vector<float> ball_y_ordered_queue_;
        int position_to_get_;

        std::vector<float> angle_queue_;
        std::vector<float> ordered_angle_queue_;

        world_model::WorldModel *world_model_;

        bool *running_;
        bool *changed_;

        std::mutex mutex_;

        void updateQueues();
        void sortQueues();
        void updateWorldModel();

        void configure();
        void printConfigurations();

        void run();
        void end();

    public:
        Receiver(world_model::WorldModel *world_model);
        ~Receiver();

        void init();
};

} // namespace communications
} // namespace vss_furgbol


#endif // RECEIVER_H