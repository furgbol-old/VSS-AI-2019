// ® Copyright FURGBot 2019


#ifndef SERIAL_SENDER_H
#define SERIAL_SENDER_H


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

        int mode_;
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

        void configure();
        void printConfigurations();

        void exec();
        void end();

        void send(std::vector<unsigned char> buffer);
        
    public:
        SerialSender(int execution_mode, std::queue<std::vector<uint8_t>> *gk_sending_queue, std::queue<std::vector<uint8_t>> *cb_sending_queue, std::queue<std::vector<uint8_t>> *st_sending_queue);
        ~SerialSender();

        void init();
};

} // namespace io
} // namespace vss_furgbol


#endif // SERIAL_SENDER_H