// ® Copyright FURGBot 2019


#ifndef GOALKEEPER_OPERATION_H
#define GOALKEEPER_OPERATION_H


#include "operations/operation.h"

#include <memory>


namespace vss_furgbol {
namespace operations {

class GoalkeeperOperation : public Operation {
    protected:
        void setTarget();

    public:
        GoalkeeperOperation(vss::Ball *ball, world_model::Robot *robot, int side, communications::SerialRepository *serial_repo);
        ~GoalkeeperOperation();

        void init();
};

} // namespace operations
} // namespace vss_furgbol


#endif // GOALKEEPER_OPERATION_H