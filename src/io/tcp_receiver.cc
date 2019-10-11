// ® Copyright FURGBot 2019


#include "io/tcp_receiver.h"

#include <iostream>


namespace vss_furgbol {
namespace io {

TCPReceiver::TCPReceiver() {}

TCPReceiver::TCPReceiver(world_model::WorldModel *world_model, bool *running, bool *paused, bool *status_changed) : 
    world_model_(world_model), running_(running), status_changed_(status_changed), paused_(paused) {}

TCPReceiver::~TCPReceiver() {}

void TCPReceiver::init() {
    setConfigurations();

    try {
        state_receiver_ = new vss::StateReceiver();
        state_receiver_->createSocket(); 
    } catch (zmq::error_t& error) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            *running_ = false;
        }    
    }

    if (*running_) printConfigurations();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        *status_changed_ = true;
    }
    
    exec();
    
    end();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        *status_changed_ = true;
    }
}

void TCPReceiver::exec() {
    bool previous_status;

    while (1) {
        previous_status = true;
        while (*running_ && !*paused_) {
            if (previous_status != *paused_) {
                previous_status = *paused_;
                std::cout << "[STATUS]: TCP working." << std::endl;
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    *status_changed_ = true;
                }
            }
            state_ = state_receiver_->receiveState(vss::FieldTransformationType::None);
            world_model_->ball_ = state_.ball;
            world_model_->robots_[BLUE][GK] = state_.teamBlue[GK];
            world_model_->robots_[BLUE][CB] = state_.teamBlue[CB];
            world_model_->robots_[BLUE][ST] = state_.teamBlue[ST];
            world_model_->robots_[YELLOW][GK] = state_.teamYellow[GK];
            world_model_->robots_[YELLOW][CB] = state_.teamYellow[CB];
            world_model_->robots_[YELLOW][ST] = state_.teamYellow[ST];
        }

        if ((*paused_) && (previous_status != *paused_)) {
            std::cout << "[STATUS]: TCP paused." << std::endl;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                *status_changed_ = true;
            }
        }

        if (!*running_) {
            end();
            {
                std::lock_guard<std::mutex> lock(mutex_);
                *status_changed_ = true;
            }
            break;
        }
    }
}

void TCPReceiver::end() {
    std::cout << "[STATUS]: Closing TCP..." << std::endl;
    if (*running_) state_receiver_->closeSocket();
}

void TCPReceiver::setConfigurations() { std::cout << "[STATUS]: Configuring TCP..." << std::endl; }

void TCPReceiver::printConfigurations() { std::cout << "[STATUS]: TCP configuration done!" << std::endl; }

} // namespace io
} // namespace vss_furgbol