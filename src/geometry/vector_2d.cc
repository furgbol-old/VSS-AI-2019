// ® Copyright FURGBot 2019


#include "geometry/vector_2d.h"

#include <math.h>


namespace vss_furgbol {
namespace geometry {

Vector2D::Vector2D() {}

Vector2D::Vector2D(vss::Point origin, vss::Point destiny) {
    origin_ = origin;
    destiny_ = destiny;
}

Vector2D::~Vector2D() {}

vss::Point Vector2D::getOrigin() { return origin_; }

vss::Point Vector2D::getDestiny() { return destiny_; }

float Vector2D::getComponentX() { return destiny_.x - origin_.x; }

float Vector2D::getComponentY() { return destiny_.y - origin_.y; }

float Vector2D::getReferentY(float x) { return (((x * getComponentY()) / destiny_.x) + origin_.y); }

void Vector2D::setOrigin(vss::Point origin) { origin_ = origin; }

void Vector2D::setDestiny(vss::Point destiny) { destiny_ = destiny; }

void Vector2D::operator=(Vector2D vector) {
    origin_ = vector.origin_;
    destiny_ = vector.destiny_;
}

} // namespace geometry
} // namespace vss_furgbol