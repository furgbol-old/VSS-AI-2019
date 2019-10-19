// ® Copyright FURGBot 2019


#include "io/serial_sender.h"

#include "json.hpp"

#include <fstream>
#include <iostream>


namespace vss_furgbol {
namespace io {

SerialSender::SerialSender() : io_service_(), port_(io_service_), buffer_(buf_.data()),
    mutex_() {}

SerialSender::SerialSender(bool *running, bool *paused, bool *status_changed, std::queue<std::vector<uint8_t>> gk_sending_queue, std::queue<std::vector<uint8_t>> cb_sending_queue, std::queue<std::vector<uint8_t>> st_sending_queue)
    : io_service_(), port_(io_service_), buffer_(buf_.data()), running_(running), paused_(paused),
    which_queue_(GK), status_changed_(status_changed), mutex_() {}

SerialSender::SerialSender(bool *running, bool *paused, bool *status_changed) : 
    io_service_(), port_(io_service_), buffer_(buf_.data()), running_(running), paused_(paused),
    mutex_(), status_changed_(status_changed) {}

SerialSender::~SerialSender() {}

void SerialSender::init() {
    setConfigurations();

    try {
        port_.open(port_name_);
        port_.set_option(boost::asio::serial_port_base::baud_rate(115200));
        port_.set_option(boost::asio::serial_port_base::character_size(8));
    } catch (boost::system::system_error error) {
        std::cout << "[SERIAL COMMUNICATOR ERROR]: " << std::endl << std::endl;
        *running_ = false;
    }

    if (*running_) printConfigurations();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        *status_changed_ = true;
    }

    exec();
}

void SerialSender::exec() {
    bool previous_status;

    while (1) {
        previous_status = true;
        while ((*running_) && (!*paused_)) {
            if (previous_status != *paused_) {
                previous_status = *paused_;
                std::cout << "[STATUS]: System working." << std::endl;
                *status_changed_ = true;
            }
            //send(which_queue_);
            which_queue_++;
            if (which_queue_ > ST) which_queue_ = GK;
        }

        if ((*paused_) && (previous_status != *paused_)) {
            std::cout << "[STATUS]: System paused." << std::endl;
            *status_changed_ = true;
        }

        if (!*running_) {
            end();
            *status_changed_ = true;
            break;
        }
    }
}

void SerialSender::end() {
    std::cout << "[STATUS]: Closing serial..." << std::endl;
    port_.close(); 
}

void SerialSender::setConfigurations() {
    std::cout << "[STATUS]: Configuring serial..." << std::endl;
    std::ifstream _ifstream("config/serial.json");
    nlohmann::json json_file;
    _ifstream >> json_file;

    port_name_ = json_file["port_name"];
    frequency_ = json_file["sending_frequency"];
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

void SerialSender::send(int which_queue) {
    switch (which_queue) {
        case GK:
            if (!gk_sending_queue_.empty()) {
                port_.write_some(boost::asio::buffer(gk_sending_queue_.front(), gk_sending_queue_.front().size()));
                gk_sending_queue_.pop();
            }
            break;
        case CB:
            if (!cb_sending_queue_.empty()) {
                port_.write_some(boost::asio::buffer(cb_sending_queue_.front(), cb_sending_queue_.front().size()));
                cb_sending_queue_.pop();
            }
            break;
        case ST:
            if (!st_sending_queue_.empty()) {
                port_.write_some(boost::asio::buffer(st_sending_queue_.front(), st_sending_queue_.front().size()));
                st_sending_queue_.pop();
            }
            break;
    }
}

std::string SerialSender::getPortName() { return port_name_; }

int SerialSender::getFrequency() { return frequency_; }

float SerialSender::getPeriod() { return period_; }

} // namespace io
} // namespace vss_furgbol