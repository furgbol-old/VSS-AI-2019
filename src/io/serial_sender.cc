// ® Copyright FURGBot 2019


#include "labels/labels.h"
#include "operation/operation.h"
#include "io/serial_sender.h"

#include "json.hpp"

#include <fstream>
#include <iostream>


namespace vss_furgbol {
namespace io {

SerialSender::SerialSender(int execution_mode, std::queue<std::vector<uint8_t>> *gk_sending_queue, std::queue<std::vector<uint8_t>> *cb_sending_queue, std::queue<std::vector<uint8_t>> *st_sending_queue) :
    gk_sending_queue_(gk_sending_queue), cb_sending_queue_(cb_sending_queue), st_sending_queue_(st_sending_queue),
    mode_(execution_mode), io_service_(), port_(io_service_), buffer_(buf_.data()), running_(true),
    which_queue_(GK) {}

SerialSender::~SerialSender() {}

void SerialSender::init() {
    configure();

    if (mode_ == REAL) {
        try {
            port_.open(port_name_);
            port_.set_option(boost::asio::serial_port_base::baud_rate(115200));
            port_.set_option(boost::asio::serial_port_base::character_size(8));
        } catch (boost::system::system_error error) {
            std::cout << "[SERIAL COMMUNICATOR ERROR]: " << error.what() << std::endl;
            running_ = false;
        }
    }

    exec();
    end();
}

void SerialSender::configure() {
    std::cout << std::endl << "[STATUS]: Configuring serial..." << std::endl;
    std::ifstream _ifstream("config/serial.json");
    nlohmann::json json_file;
    _ifstream >> json_file;

    port_name_ = json_file["port name"];
    frequency_ = json_file["sending frequency"];
    period_ = 1/(float)frequency_;
}

void SerialSender::printConfigurations() {
    std::cout << "[STATUS]: Serial configuration done!" << std::endl;

    std::cout << "-> Configurations:" << std::endl;
    std::cout << "Serial port: " << port_name_ << std::endl;
    std::cout << "Serial sending frequency: " << frequency_ << "hz" << std::endl;
    std::cout << "Time between serial messages: " << period_ << "s" << std::endl;
    std::cout << std::endl;
}


void SerialSender::exec() {
    std::vector<uint8_t> buffer;
    std::chrono::system_clock::time_point compair_time = std::chrono::high_resolution_clock::now();

    do {
        if ((std::chrono::high_resolution_clock::now() - compair_time) >= sending_frequency_) {
            switch (which_queue_) {
                case GK:
                    if (!gk_sending_queue_->empty()) {
                        {
                            std::lock_guard<std::mutex> lock(mutex_);
                            buffer = gk_sending_queue_->front();
                        }
                    }
                    //std::cout << "Goalkeeper!" << std::endl;
                    break;
                case CB:
                    if (!cb_sending_queue_->empty()) {
                        {
                            std::lock_guard<std::mutex> lock(mutex_);
                            buffer = cb_sending_queue_->front();
                        }
                    }
                    //std::cout << "Centerback!" << std::endl;
                    break;
                case ST:
                    if (!st_sending_queue_->empty()) {
                        {
                            std::lock_guard<std::mutex> lock(mutex_);
                            buffer = st_sending_queue_->front();
                        }
                    }
                    //std::cout << "Striker!" << std::endl;
                    break;
            }
            send(buffer);
            which_queue_++;
            if (which_queue_ > ST) which_queue_ = GK;
            compair_time = std::chrono::high_resolution_clock::now();
        }
    } while (running_);

    /*while (running_) {
        if (!gk_sending_queue_->empty()) {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                buffer = gk_sending_queue_->front();
            }
 
            std::cout << std::endl << "Buffer:" << std::endl;
            std::cout << "\tRobot ID: " << (int)buffer[operation::ROBOT_ID] << std::endl;
            std::cout << "\tLinear Velocity: " << (int)buffer[operation::LINEAR_VELOCITY] << std::endl;
            std::cout << "\tAngular Velocity: " << (int)buffer[operation::ANGULAR_VELOCITY] << std::endl;
            std::cout << "\tLinear Direction: " << (int)buffer[operation::LINEAR_DIRECTION] << std::endl;
            std::cout << "\tAngular Direction: " << (int)buffer[operation::ANGULAR_DIRECTION] << std::endl;
            send(buffer);
            compair_time = std::chrono::high_resolution_clock::now();
        }
    }*/
}

void SerialSender::end() {}

void SerialSender::send(std::vector<unsigned char> buffer) { 
    if (buffer[operation::ROBOT_ID] >= 128)
        port_.write_some(boost::asio::buffer(buffer, buffer.size()));
}

} // namespace io
} // namespace vss_furgbol