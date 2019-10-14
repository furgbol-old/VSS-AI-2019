// ® Copyright FURGBot 2019


#include "io/tcp_receiver.h"

#include <iostream>


namespace vss_furgbol {
namespace io {

TCPReceiver::TCPReceiver(world_model::WorldModel *world_model) : 
    world_model_(world_model), running_(true) {}

TCPReceiver::~TCPReceiver() {}

void TCPReceiver::init() {
    setConfigurations();

    state_receiver_ = new vss::StateReceiver();
    state_receiver_->createSocket(); 

    printConfigurations();
    
    exec();
    
    end();
}

void TCPReceiver::exec() {
    while (running_) {
        state_ = state_receiver_->receiveState(vss::FieldTransformationType::None);
        //std::cout << state_ << std::endl;

        {
            std::lock_guard<std::mutex> lock(mutex_);
            world_model_->ball = state_.ball;
            for (int i = 0; i < 3; i++) world_model_->team_blue[i] = state_.teamYellow[i];
            for (int i = 0; i < 3; i++) world_model_->team_yellow[i] = state_.teamBlue[i];
        }

        //std::cout << std::endl << *world_model_ << std::endl;
    }
}

void TCPReceiver::end() {
    std::cout << "[STATUS]: Closing TCP..." << std::endl;
    state_receiver_->closeSocket();
}

void TCPReceiver::setConfigurations() { std::cout << "[STATUS]: Configuring TCP..." << std::endl; }

void TCPReceiver::printConfigurations() { std::cout << "[STATUS]: TCP configuration done!" << std::endl; }

} // namespace io
} // namespace vss_furgbol 