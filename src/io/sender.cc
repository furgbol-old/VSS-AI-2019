// ® Copyright FURGBot 2019


#include "io/sender.h"

#include "json.hpp"

#include <fstream>
#include <iostream>


namespace vss_furgbol {
namespace io {

Sender::Sender(bool *running, bool *changed, int execution_mode, int team_color, std::queue<std::vector<uint8_t>> &gk_sending_queue, std::queue<std::vector<uint8_t>> &cb_sending_queue, std::queue<std::vector<uint8_t>> &st_sending_queue) :
    running_(running), changed_(changed), execution_mode_(execution_mode), team_color_(team_color),
    gk_sending_queue_(gk_sending_queue), cb_sending_queue_(cb_sending_queue), st_sending_queue_(st_sending_queue),
    which_queue_(GK) {}

Sender::~Sender() {}

void Sender::init() {
    configure();

    switch (execution_mode_) {
        case REAL:
            try {
                serial_writer_ = new furgbol::io::SerialWriter(port_name_, package_size_);
                serial_writer_->init(
                    SerialPort::BAUD_500000,
                    SerialPort::CHAR_SIZE_8,
                    SerialPort::PARITY_DEFAULT,
                    SerialPort::STOP_BITS_1,
                    SerialPort::FLOW_CONTROL_DEFAULT
                );
            } catch(std::runtime_error &error) {
                std::cout << "[SENDER ERROR]: Cannot open serial port." << std::endl;
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    *running_ = false;
                }
            }
            break;
        case SIMULATION:
            command_sender_ = new vss::CommandSender();
            try {
                if (team_color_ == BLUE) command_sender_->createSocket(vss::TeamType::Blue);
                else if (team_color_ == YELLOW) command_sender_->createSocket(vss::TeamType::Yellow);
            } catch (zmq::error_t &error) {
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    *running_ = false;
                }
            }
            break;
    }

    if (*running_) {
        printConfigurations();
        {
            std::lock_guard<std::mutex> lock(mutex_);
            *changed_ = true;
        }
        exec();
    }

    end();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        *changed_ = true;
    }
}

void Sender::configure() {
    std::cout << "[STATUS]: Configuring sender..." << std::endl;

    std::ifstream ifstream("config/serial.json");
    nlohmann::json json_file;
    ifstream >> json_file;

    port_name_ = json_file["port name"];
    package_size_ = json_file["package size"];
}

void Sender::printConfigurations() {
    std::cout << "[STATUS]: Sender configuration done!" << std::endl;
    
    std::cout << "-> Configurations:" << std::endl;
    std::cout << "\tPort name: " << port_name_ << std::endl;
    std::cout << "\tPackage size: " << package_size_ << std::endl;
}

void Sender::send() {
    switch (execution_mode_) {
        case REAL:
            serial_writer_->write
    }
}

} // namespace io
} // namespace vss_furgbol