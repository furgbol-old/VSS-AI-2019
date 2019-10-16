// ® Copyright FURGBot 2019


#include "labels/labels.h"
#include "system/system.h"

#include "json.hpp"

#include <fstream>


namespace vss_furgbol {
namespace system {

System::System(world_model::WorldModel *world_model) : world_model_(world_model), configured_(true) {}

System::~System() {}

void System::init() {
    std::cout << "[STATUS]: Configuring system..." << std::endl;
    configure();

    if (configured_) {
        std::cout << "[STATUS]: System configured!" << std::endl;
        std::cout << std::endl << std::endl << "---> Press enter to continue.";
        getchar();
        clearScreen();

        tcp_is_running_ = false;
        gk_is_running_ = false;
        cb_is_running_ = false;
        st_is_running_ = false;
        serial_is_running_ = false;

        startTCPReceiver();

        exec();
    } else std::cout << "[SYSTEM ERROR]: System configuration failed!" << std::endl;

    end();
}

void System::startTCPReceiver() {
    switch (execution_mode_) {
        case REAL:
            std::cout << "[SYSTEM WARNING]: Verify if VSS-Vision is running! Otherwise, you system will stall." << std::endl;
            break;
        case SIMULATION:
            std::cout << "[SYSTEM WARNING]: Verify if VSS-Simultor and VSS-Vie is running! Otherwise, you system will stall." << std::endl;
    }

    if (tcp_is_running_) {
        switch (execution_mode_) {
            case REAL:
                std::cout << "[SYSTEM ERROR]: Vision receiver is already running!" << std::endl;
                break;
            case SIMULATION:
                std::cout << "[SYSTEM ERROR]: Simulation receiver is already running!" << std::endl;
                break;
        }
    } else {
        tcp_is_running_ = true;
        tcp_changed_ = false;
        tcp_receiver_ = new io::Receiver(&tcp_is_running_, &tcp_changed_, world_model_);
        tcp_thread_ = std::thread(&io::Receiver::init, tcp_receiver_);
        while (!tcp_changed_);
    }
}

void System::startGKOperator() {
    if (gk_is_running_) std::cout << "[SYSTEM ERROR]: Goalkeeper operator is already running!" << std::endl;
    else {
        if (tcp_is_running_) {
            gk_is_running_ = true;
            gk_changed_ = false;
            if (team_color_ == BLUE)
                gk_operator_ = new operation::GKOperation(&gk_is_running_, &gk_changed_, &world_model_->ball, &world_model_->team_blue[GK], side_, max_robots_velocity_);
            else if (team_color_ == YELLOW)
                gk_operator_ = new operation::GKOperation(&gk_is_running_, &gk_changed_, &world_model_->ball, &world_model_->team_yellow[GK], side_, max_robots_velocity_);
            gk_thread_ = std::thread(&operation::GKOperation::init, gk_operator_);
            while (!gk_changed_);
        } else {
            switch (execution_mode_) {
                case REAL:
                    std::cout << "[SYSTEM ERROR]: Cannot start goalkeeper operator! Vision receiver must be running!" << std::endl;
                    break;
                case SIMULATION:
                    std::cout << "[SYSTEM ERROR]: Cannot start goalkeeper operator! Simulation receiver must be running!" << std::endl;
                    break;
            }
        }
    }
}

void System::startCBOperator() {
    if (cb_is_running_) std::cout << "[SYSTEM ERROR]: Centerback operator is already running!" << std::endl;
    else {
        if (tcp_is_running_) {
            cb_is_running_ = true;
            cb_changed_ = false;
            if (team_color_ == BLUE)
                cb_operator_ = new operation::CBOperation(&cb_is_running_, &cb_changed_, &world_model_->ball, &world_model_->team_blue[CB], side_, max_robots_velocity_);
            else if (team_color_ == YELLOW)
                cb_operator_ = new operation::CBOperation(&cb_is_running_, &cb_changed_, &world_model_->ball, &world_model_->team_yellow[CB], side_, max_robots_velocity_);
            cb_thread_ = std::thread(&operation::CBOperation::init, cb_operator_);
            while (!cb_changed_);
        } else {
            switch (execution_mode_) {
                case REAL:
                    std::cout << "[SYSTEM ERROR]: Cannot start centerback operator! Vision receiver must be running!" << std::endl;
                    break;
                case SIMULATION:
                    std::cout << "[SYSTEM ERROR]: Cannot start centerback operator! Simulation receiver must be running!" << std::endl;
                    break;
            }
        }
    }
}

void System::startSTOperator() {
    if (st_is_running_) std::cout << "[SYSTEM ERROR]: Striker operator is already running!" << std::endl;
    else {
        if (tcp_is_running_) {
            st_is_running_ = true;
            st_changed_ = false;
            if (team_color_ == BLUE)
                st_operator_ = new operation::STOperation(&st_is_running_, &st_changed_, &world_model_->ball, &world_model_->team_blue[ST], side_, max_robots_velocity_);
            else if (team_color_ == YELLOW)
                st_operator_ = new operation::STOperation(&st_is_running_, &st_changed_, &world_model_->ball, &world_model_->team_yellow[ST], side_, max_robots_velocity_);
            st_thread_ = std::thread(&operation::STOperation::init, st_operator_);
            while (!st_changed_);
        } else {
            switch (execution_mode_) {
                case REAL:
                    std::cout << "[SYSTEM ERROR]: Cannot start striker operator! Vision receiver must be running!" << std::endl;
                    break;
                case SIMULATION:
                    std::cout << "[SYSTEM ERROR]: Cannot start striker operator! Simulation receiver must be running!" << std::endl;
                    break;
            }
        }
    }
}

void System::startSerialSender() {
    if (serial_is_running_) std::cout << "[SYSTEM ERROR]: Serial sender is already running!" << std::endl;
    else {
        if (gk_is_running_ || cb_is_running_ || st_is_running_) {
            serial_is_running_ = true;
            serial_changed_ = false;
            serial_sender_ = new io::Sender(&serial_is_running_, &serial_changed_, &gk_is_running_, &cb_is_running_, &st_is_running_, max_robots_velocity_, execution_mode_, team_color_, gk_operator_->sending_queue, cb_operator_->sending_queue, st_operator_->sending_queue);
            serial_thread_ = std::thread(&io::Sender::init, serial_sender_);
            while (!serial_changed_);
        } else {
            std::cout << "[SYSTEM ERROR]: Cannot start serial communication. At least one of the operators must be running!" << std::endl;
        }
    }
}

void System::stopTCPReceiver() {
    if (tcp_is_running_) {
        {
            std::lock_guard<std::mutex> lock(tcp_mutex_);
            tcp_is_running_ = false;
            tcp_changed_ = false;
        }
        while (!tcp_changed_);
        if (gk_is_running_) stopGKOperator();
        if (cb_is_running_) stopCBOperator();
        if (st_is_running_) stopSTOperator();
        if (serial_is_running_) stopSerialSender();
        tcp_thread_.join();
    } else {
        switch (execution_mode_) {
            case REAL:
                std::cout << "[SYSTEM ERROR]: Vision receiver is already not running!" << std::endl;
                break;
            case SIMULATION:
                std::cout << "[SYSTEM ERROR]: Simulation receiver is already not running!" << std::endl;
                break;
        }
    }
}

void System::stopGKOperator() {
    if (gk_is_running_) {
        {
            std::lock_guard<std::mutex> lock(gk_mutex_);
            gk_is_running_ = false;
            gk_changed_ = false;
        }
        while (!gk_changed_);
        if (!cb_is_running_ && !st_is_running_) stopSerialSender();
        gk_thread_.join();
    } else std::cout << "[SYSTEM ERROR]: Goalkeeper operator is already not running!" << std::endl;
}

void System::stopCBOperator() {
    if (cb_is_running_) {
        {
            std::lock_guard<std::mutex> lock(cb_mutex_);
            cb_is_running_ = false;
            cb_changed_ = false;
        }
        while (!cb_changed_);
        if (!gk_is_running_ && !st_is_running_) stopSerialSender();
        cb_thread_.join();
    } else std::cout << "[SYSTEM ERROR]: Centerback operator is already not running!" << std::endl;
}

void System::stopSTOperator() {
    if (st_is_running_) {
        {
            std::lock_guard<std::mutex> lock(st_mutex_);
            st_is_running_ = false;
            st_changed_ = false;
        }
        while (!st_changed_);
        if (!gk_is_running_ && !cb_is_running_) stopSerialSender();
        st_thread_.join();
    } else std::cout << "[SYSTEM ERROR]: Striker operator is already not running!" << std::endl;
}

void System::stopSerialSender() {
    if (serial_is_running_) {
        {
            std::lock_guard<std::mutex> lock(serial_mutex_);
            serial_is_running_ = false;
            serial_changed_ = false;
        }
        while (!serial_changed_);
        serial_thread_.join();
    } else std::cout << "[SYSTEM ERROR]: Serial sender is already not running!" << std::endl;
}

void System::configure() {
    std::ifstream ifstream("config/system.json");
    nlohmann::json json_file;
    ifstream >> json_file;

    max_robots_velocity_ = json_file["max robots velocity"];

    if (json_file["team color"] == "blue") team_color_ = BLUE;
    else if (json_file["team color"] == "yellow") team_color_ = YELLOW;
    else {
        std::cout << "[SYSTEM ERROR]: JSON: Unknown team color!" << std::endl;
        configured_ = false;
    }
    if (configured_) std::cout << "-> Team color: " << std::string(json_file["team color"]) << std::endl;

    if (json_file["side"] == "left") side_ = LEFT;
    else if (json_file["side"] == "right") side_ = RIGHT;
    else {
        std::cout << "[SYSTEM ERROR]: JSON: Unknown side!" << std::endl;
        configured_ = false;
    }
    if (configured_) std::cout << "-> Side: " << std::string(json_file["side"]) << std::endl;

    if (json_file["execution mode"] == "real") execution_mode_ = REAL;
    else if (json_file["execution mode"] == "simulation") execution_mode_ = SIMULATION;
    else {
        std::cout << "[SYSTEM ERROR]: JSON: Unknown execution mode!" << std::endl;
        configured_ = false;
    }
    if (configured_) std::cout << "-> Execution mode: " << std::string(json_file["execution mode"]) << std::endl;

    std::cout << "Max robots velocity: " << max_robots_velocity_ << std::endl;
}

void System::exec() {
    int option;

    do {
        std::cout << std::endl << std::endl;
        std::cout << "\t ------ Furgbol VSS System ----- " << std::endl;
        std::cout << "[1] - Start";
        switch (execution_mode_) {
            case REAL:
                std::cout << " Vision ";
                break;
            case SIMULATION:
                std::cout << " Simulation ";
                break;
        }
        std::cout << "Receiver" << std::endl;
        std::cout << "[2] - Start All Operators" << std::endl;
        std::cout << "[3] - Start Goalkeeper Operator" << std::endl;
        std::cout << "[4] - Start Centerback Operator" << std::endl;
        std::cout << "[5] - Start Striker Operator" << std::endl;
        std::cout << "[6] - Start Robots Movimentation (Serial Sender)" << std::endl;
        std::cout << "[7] - Pause";
        switch (execution_mode_) {
            case REAL:
                std::cout << " Vision ";
                break;
            case SIMULATION:
                std::cout << " Simulation ";
                break;
        }
        std::cout << "Receiver" << std::endl;
        std::cout << "[8] - Pause All Operators" << std::endl;
        std::cout << "[9] - Pause Goalkeeper Operator" << std::endl;
        std::cout << "[10] - Pause Centerback Operator" << std::endl;
        std::cout << "[11] - Pause Striker Operator" << std::endl;
        std::cout << "[12] - Pause Robots Movimentation (Serial Sender)" << std::endl;
        std::cout << "[0] - Close System" << std::endl;
        std::cout << "---> ";
        std::cin >> option;
        
        clearScreen();

        switch (option) {
            case 0:
                if (tcp_is_running_) stopTCPReceiver();
                if (gk_is_running_) stopGKOperator();
                if (cb_is_running_) stopCBOperator();
                if (st_is_running_) stopSTOperator();
                if (serial_is_running_) stopSerialSender();
                break;
            case 1:
                startTCPReceiver();
                break;
            case 2:
                startGKOperator();
                startCBOperator();
                startSTOperator();
                break;
            case 3:
                startGKOperator();
                startSerialSender();
                break;
            case 4:
                startCBOperator();
                break;
            case 5:
                startSTOperator();
                break;
            case 6:
                startSerialSender();
                break;
            case 7:
                stopTCPReceiver();
                break;
            case 8:
                stopGKOperator();
                stopCBOperator();
                stopSTOperator();
                break;
            case 9:
                stopGKOperator();
                break;
            case 10:
                stopCBOperator();
                break;
            case 11:
                stopSTOperator();
                break;
            case 12:
                stopSerialSender();
                break;
            default:
                std::cout << "[SYSTEM ERROR]: Please, select a valid option!" << std::endl;
                break;
        }
    } while (option != 0);
}

void System::end() { std::cout << "[STATUS]: Closing system..." << std::endl; }

void System::clearScreen() { std::cout << "\033[2J\033[1;1H"; }

} // namespace system
} // namespace vss_furgbol