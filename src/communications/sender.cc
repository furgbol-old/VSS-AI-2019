// ® Copyright FURGBot 2019


#include "labels/labels.h"
#include "communications/sender.h"

#include "json.hpp"

#include <fstream>
#include <iostream>


namespace vss_furgbol {
namespace communications {

Sender::Sender(communications::SerialRepository *serial_repo) : serial_repo_(serial_repo) {}

Sender::~Sender() {}

void Sender::init() {
    configure();

    serial_writer_ = std::make_shared<furgbol::io::SerialWriter>(serial_port_name_, serial_package_size_);
    serial_writer_->init(
        SerialPort::BAUD_500000,
        SerialPort::CHAR_SIZE_8,
        SerialPort::PARITY_DEFAULT,
        SerialPort::STOP_BITS_1,
        SerialPort::FLOW_CONTROL_DEFAULT
    );
    which_player_ = GK;

    run();
    end();
}

void Sender::configure() {
    std::ifstream ifstream("config/communications.json");
    nlohmann::json json_file;
    ifstream >> json_file;

    serial_port_name_ = json_file["sender"]["port name"];
    serial_package_size_ = json_file["sender"]["package size"];
}

void Sender::run() {
    std::vector<uint8_t> buffer;
    while (true) {
        if (serial_repo_->getStatus(which_player_) == true) {
            buffer = serial_repo_->getPackage(which_player_);
            if (buffer[LINEAR_VELOCITY] > 0 || buffer[ANGULAR_VELOCITY] > 0) serial_writer_->write(buffer);
        }
        //which_player_++;
        //if (which_player_ > ST) which_player_ = GK;
    }
}

void Sender::end() {}

} // namespace communications
} // namespace vss_furgbol