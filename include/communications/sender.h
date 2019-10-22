// ® Copyright FURGBot 2019


#ifndef SENDER_H
#define SENDER_H


#include "communications/serial_repository.h"

#include "furgbol-core/io/serial_writer.h"

#include <memory>
#include <mutex>
#include <string>


namespace vss_furgbol {
namespace communications {

class Sender {
    private:
        communications::SerialRepository *serial_repo_;

        std::shared_ptr<furgbol::io::SerialWriter> serial_writer_;

        int which_player_;

        std::string serial_port_name_;
        int serial_package_size_;

        void configure();

        void run();
        void end();

    public:
        Sender(communications::SerialRepository *serial_repo);
        ~Sender();

        void init();
};

} // namespace communications
} // namespace vss_furgbol


#endif // SENDER_H