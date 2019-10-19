// ® Copyright FURGBot 2019


#ifndef LABELS_H
#define LABELS_H


namespace vss_furgbol {

enum AttributesLabels {
    X, Y, ANGLE
};

enum BufferLabels {
    ROBOT_ID, LINEAR_VELOCITY, ANGULAR_VELOCITY, LINEAR_DIRECTION, ANGULAR_DIRECTION
};

enum DirectionsLabels {
    NEGATIVE = 1, POSITIVE = 3
};

enum OutOfPlaceLabels {
    NO, BEHIND, AHEAD
};

enum PlayersLabels {
    GK, CB, ST // GoalKeeper, CenterBack, STriker
};

enum SideLabels {
    LEFT, RIGHT
};

enum TeamColorLabels {
    BLUE, YELLOW
};

}


#endif // LABELS_H