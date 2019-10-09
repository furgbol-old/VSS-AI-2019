// ® Copyright FURGBot 2019


#ifndef FIELD_LINE_H
#define FIELD_LINE_H


namespace vss_furgbol {
namespace geometry {

class FieldLine {
    private:
        float x_;
        float min_y_;
        float max_y_;

    public:
        FieldLine();
        FieldLine(float x, float min_y, float max_y);
        ~FieldLine();

        //Getters
        float getX();
        float getMinY();
        float getMaxY();

        //Setters
        void setX(float x);
        void setMinY(float min_y);
        void setMaxY(float max_y);

        //Operators
        void operator=(FieldLine field);
};

} // namespace geometry
} // namespace vss_furgbol


#endif // FIELD_LINE_H