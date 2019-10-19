// ® Copyright FURGBot 2019


#ifndef STRIKER_OPERATION_H
#define STRIKER_OPERATION_H


#include "operations/operation.h"

#include <memory>


namespace vss_furgbol {
namespace operations {

class StrikerOperation : public Operation {
    protected:
        void setTarget();

    public:
        StrikerOperation(vss::Ball *ball, world_model::Robot *robot, int side, communications::SerialRepository *serial_repo);
        ~StrikerOperation();

        void init();
};

} // namespace operations
} // namespace vss_furgbol


#endif // STRIKER_OPERATION_H