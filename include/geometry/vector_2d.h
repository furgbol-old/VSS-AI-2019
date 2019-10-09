// ® Copyright FURGBot 2019


#ifndef VECTOR_2D_H
#define VECTOR_2D_H


#include "Domain/Point.h"


namespace vss_furgbol {
namespace geometry {

class Vector2D {
    private:
        vss::Point origin_;
        vss::Point destiny_;
    
    public:
        Vector2D();
        Vector2D(vss::Point origin, vss::Point destiny);
        ~Vector2D();

        //Getters
        vss::Point getOrigin();
        vss::Point getDestiny();
        float getComponentX();
        float getComponentY();
        float getReferentY(float x);

        //Setters
        void setOrigin(vss::Point origin);
        void setDestiny(vss::Point destiny);

        //Operators
        void operator=(Vector2D vector);
};

} // namespace geometry
} // namespace vss_furgbol


#endif // VECTOR_2D_H