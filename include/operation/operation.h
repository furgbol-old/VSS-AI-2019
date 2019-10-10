// ® Copyright FURGBot 2019


#ifndef OPERATION_H
#define OPERATION_H


#include "geometry/field_line.h"
#include "geometry/vector_2d.h"
#include "world_model/robot.h"

#include "Domain/Ball.h"

#include <cinttypes>
#include <mutex>
#include <queue>


namespace vss_furgbol {
namespace operation {

enum DirectionsLabels {
    NEGATIVE = 1, POSITIVE = 3
};

enum OutOfPlaceLabels {
    NO, BEHIND, AHEAD
};

enum BufferLabels {
    ROBOT_ID, LINEAR_VELOCITY, ANGULAR_VELOCITY, LINEAR_DIRECTION, ANGULAR_DIRECTION
};

} // namespace operation
} // namespace vss_furgbol


#endif // OPERATION_H