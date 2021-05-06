#include "Shape.h"

typedef struct
{
    double radius;
} Circle;

double circle_Area(Circle *circle);
ShapeInterface *CircleAsShape;
Circle * circle_Create(double radius);
