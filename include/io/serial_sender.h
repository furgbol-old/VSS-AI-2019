// ® Copyright FURGBot 2019


#ifndef SERIAL_SENDER_H
#define SERIAL_SENDER_H


#include "Communications/CommandSender.h"

#include <boost/asio.hpp>
#include <chrono>
#include <cinttypes>
#include <mutex>
#include <queue>
#include <vector>


namespace vss_furgbol {
namespace io {

class SerialSender {
    private:
        boost::asio::streambuf buf_;
        boost::asio::io_service io_service_;
        boost::asio::streambuf::const_buffers_type buffer_;
        boost::asio::serial_port port_;

        vss::CommandSender *command_sender_;
        vss::Command command_;

        int mode_;
        int team_color_;
        int which_queue_;

        bool running_;

        std::string port_name_;
        int frequency_;
        float period_;
        std::chrono::duration<float> sending_frequency_;

        std::queue<std::vector<uint8_t>> *gk_sending_queue_;
        std::queue<std::vector<uint8_t>> *cb_sending_queue_;
        std::queue<std::vector<uint8_t>> *st_sending_queue_;

        std::mutex mutex_;

        int linear_velocity_;
        int angular_velocity_;
        int linear_direction_;
        int angular_direction_;

        float velocity_right_;
        float velocity_left_;

        void configure();
        void printConfigurations();

        void exec();
        void end();

        void send(std::vector<unsigned char> buffer);

        void calculateVelocity();
        void errorCorrector();
        
    public:
        SerialSender(int execution_mode, int team_color_, std::queue<std::vector<uint8_t>> *gk_sending_queue, std::queue<std::vector<uint8_t>> *cb_sending_queue, std::queue<std::vector<uint8_t>> *st_sending_queue);
        ~SerialSender();

        void init();
};

} // namespace io
} // namespace vss_furgbol


#endif // SERIAL_SENDER_H