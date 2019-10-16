// ® Copyright FURGBot 2019


#ifndef SENDER_H
#define SENDER_H


#include "labels/labels.h"

#include "Communications/CommandSender.h"
#include "furgbol-core/io/serial_writer.h"

#include <chrono>
#include <cinttypes>
#include <mutex>
#include <queue>
#include <vector>


namespace vss_furgbol {
namespace io {

class Sender {
    private:
        bool *running_;
        bool *changed_;

        std::mutex mutex_;

        int execution_mode_;
        int team_color_;
        int max_velocity_;

        vss::CommandSender *command_sender_;
        furgbol::io::SerialWriter *serial_writer_;

        vss::Command command_;

        std::string port_name_;
        int package_size_;

        std::queue<std::vector<uint8_t>> gk_sending_queue_;
        std::queue<std::vector<uint8_t>> cb_sending_queue_;
        std::queue<std::vector<uint8_t>> st_sending_queue_;

        bool *gk_is_running_;
        bool *cb_is_running_;
        bool *st_is_running_;
        int which_queue_;

        std::vector<unsigned char> buffer_to_send_;

        void configure();
        void printConfigurations();

        void send();

        void calculateWheelsVelocity();

        void exec();
        void end();
        
    public:
        Sender(bool *running, bool *changed, bool *gk_is_running, bool *cb_is_running, bool *st_is_running, int max_velocity, int execution_mode, int team_color_, std::queue<std::vector<uint8_t>> &gk_sending_queue, std::queue<std::vector<uint8_t>> &cb_sending_queue, std::queue<std::vector<uint8_t>> &st_sending_queue);
        ~Sender();

        void init();
};

} // namespace io
} // namespace vss_furgbol


#endif // SENDER_H