// ® Copyright FURGBot 2019


#ifndef ST_OPERATION_H
#define ST_OPERATION_H


#include "operation/operation.h"


namespace vss_furgbol {
namespace operation {

class STOperation {
    private:
        world_model::Robot *robot_;
        vss::Ball *ball_;

        vss::Point target_;
        float target_angle_;
        int out_of_place_;

        geometry::FieldLine field_line_;
        int max_queue_size_;
        int velocity_k_;
        int vision_error_;
        float max_ball_distance_;
        int max_velocity_;

        int linear_velocity_;
        int angular_velocity_;
        int linear_direction_;
        int angular_direction_;

        bool *running_;
        bool *status_changed_;
        std::mutex mutex_;

        std::vector<uint8_t> buffer_to_send_;
        std::queue<std::vector<uint8_t>> sending_queue_;

        void setConfigurations();
        void printConfigurations();

        void verifyPosition();
        void setTarget();
        void setMotion();

        void exec();
        void end();

        void serialize();
         
    public:
        STOperation();
        STOperation(world_model::Robot *robot, vss::Ball *ball, bool *running, bool *status_changed);
        ~STOperation();

        void init();

        //Getters
        std::queue<std::vector<uint8_t>> getSendingQueue();
};

} // namespace operation
} // namespace vss_furgbol


#endif // CB_OPERATION_H