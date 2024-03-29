// ® Copyright FURGBot 2019


#ifndef OPERATION_H
#define OPERATION_H


#include "communications/serial_repository.h"
#include "world_model/robot.h"

#include "Domain/Ball.h"

#include <memory>
#include <mutex>
#include <string>
#include <vector>


namespace vss_furgbol {
namespace operations {

class Operation {
    protected:
        vss::Ball *ball_;
        world_model::Robot *robot_;

        int side_;

        communications::SerialRepository *serial_repo_;

        //Velocity control
        int linear_velocity_gain_;
        int angular_velocity_gain_;
        int linear_discrepancy_factor;
        int angular_discrepancy_factor;

        //Distance thresholds
        int linear_threshold_;
        int angular_threshold_;
        float max_ball_distance_;

        //Velocity thresholds
        int max_linear_velocity_;
        int max_angular_velocity_;
        int min_linear_velocity_;
        int min_angular_velocity_;
        int kick_velocity_;

        geometry::FieldLine field_line_;

        vss::Pose target_;
        int out_of_place_;

        std::mutex mutex_;

        virtual void configure(std::string which_player);

        void run();
        void end();

        void verifyPosition();
        virtual void setTarget() = 0;
        void setMotion();
        void serialize();

        bool canKick(float robot_x, float robot_y, float ball_x, float ball_y);
        bool outOfAngle(float robot_angle);
        bool outOfTarget(float robot_x, float robot_y);

        void setKick(float robot_y, float ball_y);
        void fixesAngle(float robot_angle);
        void goToTarget(float robot_x, float robot_y);
        void stopRobot();

        int calculateAngularVelocity(float robot_angle);
        int calculateLinearVelocity(float robot_x, float robot_y);
        int getQuadrant(float robot_angle);

    public:
        Operation();
        ~Operation();

        virtual void init();
};

} // namespace operations
} // namespace vss_furgbol


#endif // OPERATION_H