// ® Copyright FURGBot 2019


#include "system/system.h"

#include "json.hpp"

#include <fstream>


namespace vss_furgbol {
namespace system {

System::System() {}

System::System(world_model::WorldModel *world_model) : world_model_(world_model), 
    serial_mutex_(), configured_(true) {}

System::~System() {}

void System::init() {
    clearScreen();

    std::cout << "[STATUS]: Configuring system..." << std::endl;
    std::cout << "AQUI" << std::endl;
    std::ifstream _ifstream("config/system.json");
    nlohmann::json json_file;
    _ifstream >> json_file;

    if (json_file["team_color"] == "blue") team_color_ = BLUE;
    else if (json_file["team_color"] == "yellow") team_color_ = YELLOW;
    else {
        std::cout << "[SYSTEM ERROR]: Unknown team color." << std::endl;
        configured_ = false;
    }

    if (json_file["execution_mode"] == "real") execution_mode_ = REAL;
    else if (json_file["execution_mode"] == "simulation") execution_mode_ = SIMULATION;
    else {
        std::cout << "[SYSTEM ERROR]: Unknown execution mode." << std::endl;
        configured_ = false;
    }

    if (configured_) {
        std::cout << "[STATUS]: System configuration done!" << std::endl;

        std::cout << "-> Configurations:" << std::endl;
        std::cout << "Team color: " << std::string(json_file["team_color"]) << std::endl;
        std::cout << "Execution mode: " << std::string(json_file["execution_mode"]) << std::endl;
        std::cout << std::endl;

        gk_is_running_ = true;
        gk_is_paused_ = true;
        gk_status_changed_ = false;
        gk_operator_ = new operation::GKOperation(&world_model_->robots_[team_color_][GK], &world_model_->ball_, &gk_is_running_, &gk_status_changed_);
        gk_thread_ = std::thread(&operation::GKOperation::init, gk_operator_);
        while (!gk_status_changed_);

        cb_is_running_ = true;
        cb_status_changed_ = false;
        cb_operator_ = new operation::CBOperation(&world_model_->robots_[team_color_][CB], &world_model_->ball_, &cb_is_running_, &cb_status_changed_);
        cb_thread_ = std::thread(&operation::CBOperation::init, cb_operator_);
        while (!cb_status_changed_);

        st_is_running_ = true;
        st_status_changed_ = false;
        st_operator_ = new operation::STOperation(&world_model_->robots_[team_color_][ST], &world_model_->ball_, &st_is_running_, &st_status_changed_);
        st_thread_ = std::thread(&operation::STOperation::init, st_operator_);
        while (!st_status_changed_);

        tcp_is_running_ = true;
        tcp_is_paused_ = true;
        tcp_status_changed_ = false;
        tcp_receiver_ = new io::TCPReceiver(world_model_, &tcp_is_running_, &tcp_is_paused_, &tcp_status_changed_);
        tcp_thread_ = std::thread(&io::TCPReceiver::init, tcp_receiver_);
        while (!tcp_status_changed_);
        if (!tcp_is_running_) tcp_thread_.join();

        serial_is_running_ = true;
        serial_is_paused_ = true;
        serial_status_changed_ = false;
        serial_sender_ = new io::SerialSender(execution_mode_, &serial_is_running_, &serial_is_paused_, &serial_status_changed_, gk_operator_->getSendingQueue(), cb_operator_->getSendingQueue(), st_operator_->getSendingQueue());
        serial_thread_ = std::thread(&io::SerialSender::init, serial_sender_);
        while (!serial_status_changed_);
        if (!serial_is_running_) serial_thread_.join();

        std::cout << std::endl << "**Press enter to continue.";
        std::getchar();
        clearScreen();

        exec();
    } else std::cout << "[STATUS]: System configuration failed!" << std::endl;
}

void System::exec() {
    int option, count;

    do {
        std::cout << std::endl << std::endl << "\t------[MENU]------" << std::endl;
        std::cout << "[1] - Start system" << std::endl;
        std::cout << "[2] - Start vision" << std::endl;
        std::cout << "[3] - Start goalkeeper" << std::endl;
        std::cout << "[4] - Start centerback" << std::endl;
        std::cout << "[5] - Start striker" << std::endl;
        std::cout << "[6] - Pause system" << std::endl;
        std::cout << "[7] - Pause vision" << std::endl;
        std::cout << "[8] - Pause goalkeeper" << std::endl;
        std::cout << "[9] - Pause centerback" << std::endl;
        std::cout << "[10] - Pause striker" << std::endl;
        std::cout << "[0] - Close system" << std::endl;
        std::cout << "\t-> ";
        std::cin >> option;

        clearScreen();

        switch (option) {
            case 0:
                if (serial_is_running_) {
                    {
                        std::lock_guard<std::mutex> lock(serial_mutex_);
                        serial_status_changed_ = false;
                    }
                    {
                        std::lock_guard<std::mutex> lock(serial_mutex_);
                        serial_is_running_ = false;
                    }
                    while (!serial_status_changed_);
                    serial_thread_.join();
                }
                if (tcp_is_running_) {
                    {
                        std::lock_guard<std::mutex> lock(tcp_mutex_);
                        tcp_status_changed_ = false;
                    }
                    {
                        std::lock_guard<std::mutex> lock(tcp_mutex_);
                        tcp_is_running_ = false;
                    }
                    while (!tcp_status_changed_);
                    tcp_thread_.join();
                }
                {
                    std::lock_guard<std::mutex> lock(gk_mutex_);
                    gk_status_changed_ = false;
                }
                {
                    std::lock_guard<std::mutex> lock(gk_mutex_);
                    gk_is_running_ = false;
                }
                while (!gk_status_changed_);
                {
                    std::lock_guard<std::mutex> lock(cb_mutex_);
                    cb_status_changed_ = false;
                }
                {
                    std::lock_guard<std::mutex> lock(cb_mutex_);
                    cb_is_running_ = false;
                }
                while (!cb_status_changed_);
                {
                    std::lock_guard<std::mutex> lock(st_mutex_);
                    st_status_changed_ = false;
                }
                {
                    std::lock_guard<std::mutex> lock(st_mutex_);
                    st_is_running_ = false;
                }
                while (!st_status_changed_);
                end();
                break;
            case 1:
                if (!serial_is_running_) {
                    {
                        std::lock_guard<std::mutex> lock(serial_mutex_);
                        serial_is_running_ = true;
                    }
                    {
                        std::lock_guard<std::mutex> lock(serial_mutex_);
                        serial_is_paused_ = false;
                    }
                    {
                        std::lock_guard<std::mutex> lock(serial_mutex_);
                        serial_status_changed_ = false;
                    }
                    serial_thread_ = std::thread(&io::SerialSender::init, serial_sender_);
                    while (!serial_status_changed_);
                    if (!serial_is_running_) {
                        serial_thread_.join();
                        {
                            std::lock_guard<std::mutex> lock(serial_mutex_);
                            serial_is_paused_ = true;
                        }
                    }
                } else if (serial_is_paused_) {
                    {
                        std::lock_guard<std::mutex> lock(serial_mutex_);
                        serial_status_changed_ = false;
                    }
                    {
                        std::lock_guard<std::mutex> lock(serial_mutex_);
                        serial_is_paused_ = false;
                    }
                    while (!serial_status_changed_);
                }
                break;
            case 2:
                if (!tcp_is_running_) {
                    {
                        std::lock_guard<std::mutex> lock(tcp_mutex_);
                        tcp_is_running_ = true;
                    }
                    {
                        std::lock_guard<std::mutex> lock(tcp_mutex_);
                        tcp_is_paused_ = false;
                    }
                    {
                        std::lock_guard<std::mutex> lock(tcp_mutex_);
                        tcp_status_changed_ = false;
                    }
                    tcp_thread_ = std::thread(&io::TCPReceiver::init, tcp_receiver_);
                    while (!tcp_status_changed_);
                    if (!tcp_is_running_) {
                        tcp_thread_.join();
                        {
                            std::lock_guard<std::mutex> lock(tcp_mutex_);
                            tcp_is_paused_ = true;
                        }
                    }
                } else if (tcp_is_paused_) {
                    {
                        std::lock_guard<std::mutex> lock(tcp_mutex_);
                        tcp_status_changed_ = false;
                    }
                    {
                        std::lock_guard<std::mutex> lock(tcp_mutex_);
                        tcp_is_paused_ = false;
                    }
                    while (!tcp_status_changed_);
                }
                break;
            case 6:
                if (!serial_is_paused_) {
                    {
                        std::lock_guard<std::mutex> lock(serial_mutex_);
                        serial_status_changed_ = false;
                    }
                    {
                        std::lock_guard<std::mutex> lock(serial_mutex_);
                        serial_is_paused_ = true;
                    }
                    while (!serial_status_changed_);
                }
                break;
            case 7:
                if (!tcp_is_paused_) {
                    {
                        std::lock_guard<std::mutex> lock(tcp_mutex_);
                        tcp_status_changed_ = false;
                    }
                    {
                        std::lock_guard<std::mutex> lock(tcp_mutex_);
                        tcp_is_paused_ = true;
                    }
                    while (!tcp_status_changed_);
                }
                break;
            default:
                std::cout << "**Please, insert a valid option!" << std::endl;
                break;
        }
    } while (option != 0);
}

void System::end() {
    std::cout << "[STATUS]: Closing system..." << std::endl;
    gk_thread_.join();
    cb_thread_.join();
    st_thread_.join();
}

void System::clearScreen() { std::cout << "\033[2J\033[1;1H"; }

} // namespace system
} // namespace vss_furgbol