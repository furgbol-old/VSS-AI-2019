// ® Copyright FURGBot 2019


#ifndef CB_OPERATION_H
#define CB_OPERATION_H


#include "geometry/field_line.h"
#include "geometry/vector_2d.h"
#include "operation/operation.h"
#include "world_model/world_model.h"

#include <mutex>
#include <queue>


namespace vss_furgbol {
namespace operation {

class CBOperation {
    private:
        vss::Ball *ball_;
        world_model::Robot * robot_;

        int max_queue_size_;
        int velocity_gain_;
        int error_threshold_;

        int max_velocity_;
        float max_ball_distance_;
        geometry::FieldLine field_line_;

        int side_;

        bool *running_;
        bool *changed_;

        std::mutex mutex_;

        int out_of_place_;

        vss::Point target_;
        float target_angle_;

        int linear_velocity_;
        int angular_velocity_;
        int linear_direction_;
        int angular_direction_;

        std::vector<uint8_t> buffer_to_send_;

        void configure();
        void printConfigurations();

        void exec();
        void end();

        void verifyPosition();
        void setTarget();
        void setMotion();
        void serialize();

        bool canKick(float robot_x, float robot_y, float ball_x, float ball_y);
        bool outOfAngle(float robot_angle);
        bool outOfTarget(float robot_x, float robot_y, float ball_x, float ball_y);

        void setKick(float robot_y, float ball_y);
        void fixesAngle(float robot_angle);
        void goToTarget(float robot_x, float robot_y, float ball_x, float ball_y);

    public:
        std::queue<std::vector<uint8_t>> sending_queue;

        CBOperation(bool *running, bool *changed, vss::Ball *ball, world_model::Robot *robot, int side);
        ~CBOperation();

        void init();

};

} // namespace operation
} // namespace vss_furgbol


#endif // CB_OPERATION_H