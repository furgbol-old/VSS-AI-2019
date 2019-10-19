// ® Copyright FURGBot 2019


#include "communications/serial_repository.h"

#include "json.hpp"

#include <fstream>


namespace vss_furgbol {
namespace communications {

SerialRepository::SerialRepository() {
    is_active_ = {false, false, false};
    packages_to_send_ = std::vector<std::vector<uint8_t>>(3, std::vector<uint8_t>(5, 0));
}

SerialRepository::~SerialRepository() {}

bool SerialRepository::getStatus(int player) {
    std::lock_guard<std::mutex> lock(mutex_);
    return is_active_[player];
}

std::vector<uint8_t> SerialRepository::getPackage(int player) {
    std::lock_guard<std::mutex> lock(mutex_);
    return packages_to_send_[player];
}

void SerialRepository::setStatus(int player, bool status) {
    std::lock_guard<std::mutex> lock(mutex_);
    is_active_[player] = status;
}

void SerialRepository::setPackage(int player, std::vector<uint8_t> package) {
    std::lock_guard<std::mutex> lock(mutex_);
    packages_to_send_[player] = package;
}

} // namespace communications
} // namespace vss_furgbol