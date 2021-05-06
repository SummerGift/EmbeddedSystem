#include "Shape.h"
#include "stdlib.h"

typedef struct {
    double x;
} Square;

double
square_Area(Square *square);
ShapeInterface *SquareAsShape;
Square *
square_Create(double sideLength);