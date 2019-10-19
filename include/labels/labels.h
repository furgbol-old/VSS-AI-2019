// ® Copyright FURGBot 2019


#ifndef LABELS_H
#define LABELS_H


namespace vss_furgbol {

enum AngularDirectionsLabels {
    COUNTERCLOCKWISE = 1, CLOCKWISE = 3
};

enum AttributesLabels {
    X, Y, ANGLE
};

enum BufferLabels {
    ROBOT_ID, LINEAR_VELOCITY, ANGULAR_VELOCITY, LINEAR_DIRECTION, ANGULAR_DIRECTION
};

enum LinearDirectionsLabels {
    BACK = 1, FORWARD = 3
};

enum OutOfPlaceLabels {
    NO, BEHIND, AHEAD
};

enum PlayersLabels {
    GK, CB, ST // GoalKeeper, CenterBack, STriker
};

enum QuadrantLabels {
    FIRST, SECOND, THIRD, FOURTH
};

enum SideLabels {
    LEFT, RIGHT
};

enum TeamColorLabels {
    BLUE, YELLOW
};

}


#endif // LABELS_H