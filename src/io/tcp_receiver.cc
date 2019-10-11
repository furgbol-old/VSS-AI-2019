// ® Copyright FURGBot 2019


#include "io/tcp_receiver.h"

#include <iostream>


namespace vss_furgbol {
namespace io {

TCPReceiver::TCPReceiver() {}

TCPReceiver::TCPReceiver(world_model::WorldModel *world_model, bool *running, bool *status_changed) : 
    world_model_(world_model), running_(running), status_changed_(status_changed) {}

TCPReceiver::~TCPReceiver() {}

void TCPReceiver::init() {
    setConfigurations();
    printConfigurations();

    state_receiver_ = new vss::StateReceiver();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        *status_changed_ = true;
    }
    
    exec();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        *status_changed_ = false;
    }
    end();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        *status_changed_ = true;
    }
}

void TCPReceiver::exec() {
    while (*running_); /*{
        state_ = state_receiver_->receiveState(vss::FieldTransformationType::None);
        world_model_->ball_ = state_.ball;
        world_model_->robots_[BLUE][GK] = state_.teamBlue[GK];
        world_model_->robots_[BLUE][CB] = state_.teamBlue[CB];
        world_model_->robots_[BLUE][ST] = state_.teamBlue[ST];
        world_model_->robots_[YELLOW][GK] = state_.teamYellow[GK];
        world_model_->robots_[YELLOW][CB] = state_.teamYellow[CB];
        world_model_->robots_[YELLOW][ST] = state_.teamYellow[ST];
    }*/
}

void TCPReceiver::end() { std::cout << "[STATUS]: Closing TCP..." << std::endl; }

void TCPReceiver::setConfigurations() { std::cout << "[STATUS]: Configuring TCP..." << std::endl; }

void TCPReceiver::printConfigurations() { std::cout << "[STATUS]: TCP configuration done!" << std::endl; }

} // namespace io
} // namespace vss_furgbol