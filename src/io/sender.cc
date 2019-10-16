// ® Copyright FURGBot 2019


#include "io/sender.h"

#include "json.hpp"

#include <fstream>
#include <iostream>


namespace vss_furgbol {
namespace io {

Sender::Sender(bool *running, bool *changed, bool *gk_is_running, bool *cb_is_running, bool *st_is_running, int max_velocity, int execution_mode, int team_color, std::queue<std::vector<uint8_t>> &gk_sending_queue, std::queue<std::vector<uint8_t>> &cb_sending_queue, std::queue<std::vector<uint8_t>> &st_sending_queue) :
    running_(running), changed_(changed), execution_mode_(execution_mode), team_color_(team_color),
    gk_sending_queue_(gk_sending_queue), cb_sending_queue_(cb_sending_queue), st_sending_queue_(st_sending_queue),
    which_queue_(GK), max_velocity_(max_velocity), gk_is_running_(gk_is_running), cb_is_running_(cb_is_running),
    st_is_running_(st_is_running) {}

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
            serial_writer_->write(buffer_to_send_);
            break;
        case SIMULATION:
            if (buffer_to_send_.empty()) command_.commands.push_back(vss::WheelsCommand(0, 0));
            else {
                float linear_velocity = (50 * (float)buffer_to_send_[LINEAR_VELOCITY] / max_velocity_) * (buffer_to_send_[LINEAR_DIRECTION] - 2);
                float angular_velocity = (50 * (float)buffer_to_send_[ANGULAR_VELOCITY] / max_velocity_) * (buffer_to_send_[ANGULAR_DIRECTION] - 2);
                float right_velocity = ((linear_velocity / 0.03) + ((angular_velocity * 0.04) / 0.03));
                float left_velocity = ((linear_velocity / 0.03) - ((angular_velocity * 0.04) / 0.03));
                command_.commands.push_back(vss::WheelsCommand(right_velocity, left_velocity));
            }
            break;
    }
}

} // namespace io
} // namespace vss_furgbol