// ® Copyright FURGBot 2019


#ifndef OPERATION_H
#define OPERATION_H


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