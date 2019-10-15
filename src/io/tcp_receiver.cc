// ® Copyright FURGBot 2019


#include "io/tcp_receiver.h"

#include <iostream>


namespace vss_furgbol {
namespace io {

TCPReceiver::TCPReceiver(bool *running, bool *changed, world_model::WorldModel *world_model) : 
    world_model_(world_model), running_(running), changed_(changed) {}

TCPReceiver::~TCPReceiver() {}

void TCPReceiver::init() {
    setConfigurations();

    try {
        state_receiver_ = new vss::StateReceiver();
        state_receiver_->createSocket();
    } catch (zmq::error_t &error) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            *running_ = false;
        }
    }

    if (*running_) printConfigurations();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        *changed_ = true;
    }
    
    exec();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        *changed_ = true;
    }
}

void TCPReceiver::exec() {
    bool previous_status = false;;

    while (true) {
        while (*running_) {
            if (previous_status == false) {
                previous_status = true;
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    *changed_ = true;
                }
            }

            state_ = state_receiver_->receiveState(vss::FieldTransformationType::None);
            {
                std::lock_guard<std::mutex> lock(mutex_);
                world_model_->ball = state_.ball;
                for (int i = 0; i < 3; i++) world_model_->team_blue[i] = state_.teamYellow[i];
                for (int i = 0; i < 3; i++) world_model_->team_yellow[i] = state_.teamBlue[i];
            }

            //std::cout << state_ << std::endl;
            //std::cout << std::endl << *world_model_ << std::endl;
        }

        if (!*running_) {
            end();
            
            if (previous_status == true) {
                previous_status = false;
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    *changed_ = true;
                }
            }

            break;
        }
    }
}

void TCPReceiver::end() {
    std::cout << "[STATUS]: Closing vision receiver..." << std::endl;
    try {
        state_receiver_->closeSocket();
    } catch (zmq::error_t &error) {}
}

void TCPReceiver::setConfigurations() { std::cout << "[STATUS]: Configuring vision receiver..." << std::endl; }

void TCPReceiver::printConfigurations() { std::cout << "[STATUS]: Vision receiver configuration done!" << std::endl; }

} // namespace io
} // namespace vss_furgbol 