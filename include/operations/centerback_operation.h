// ® Copyright FURGBot 2019


#ifndef CENTERBACK_OPERATION_H
#define CENTERBACK_OPERATION_H


#include "operations/operation.h"

#include <memory>


namespace vss_furgbol {
namespace operations {

class CenterbackOperation : public Operation {
    protected:
        void setTarget();

    public:
        CenterbackOperation(vss::Ball *ball, world_model::Robot *robot, int side, communications::SerialRepository *serial_repo);
        ~CenterbackOperation();

        void init();
};

} // namespace operations
} // namespace vss_furgbol


#endif // CENTERBACK_OPERATION_H