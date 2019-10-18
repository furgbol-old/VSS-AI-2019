// ® Copyright FURGBot 2019


#include "geometry/field_line.h"


namespace vss_furgbol {
namespace geometry {

FieldLine::FieldLine() {}

FieldLine::FieldLine(float x, float min_y, float max_y) {
    x_ = x;
    min_y_ = min_y;
    max_y_ = max_y;
}

FieldLine::~FieldLine() {}

float FieldLine::getX() { return x_; }

float FieldLine::getMinY() { return min_y_; }

float FieldLine::getMaxY() { return max_y_; }

void FieldLine::setX(float x) { x_ = x; }

void FieldLine::setMinY(float min_y) { min_y_ = min_y; }

void FieldLine::setMaxY(float max_y) { max_y_ = max_y; }

void FieldLine::operator=(FieldLine field) {
    x_ = field.x_;
    min_y_ = field.min_y_;
    max_y_ = field.max_y_; 
}

} // geometry
} // vss_furgbol