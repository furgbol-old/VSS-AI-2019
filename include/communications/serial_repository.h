// ® Copyright FURGBot 2019


#ifndef SERIAL_REPOSITORY_H
#define SERIAL_REPOSITORY_H


#include <cinttypes>
#include <mutex>
#include <vector>


namespace vss_furgbol {
namespace communications {

class SerialRepository {
    private:
        std::mutex mutex_;

        std::vector<bool> is_active_;
        std::vector<std::vector<uint8_t>> packages_to_send_;
    public:
        SerialRepository();
        ~SerialRepository();

        bool getStatus(int player);
        std::vector<uint8_t> getPackage(int player);

        void setStatus(int player, bool status);
        void setPackage(int player, std::vector<uint8_t> package);
};

} // namespace communications
} // namespace vss_furgbol


#endif // SERIAL_REPOSITORY_H