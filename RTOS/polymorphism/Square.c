#include "stdlib.h"
#include "Square.h"

double
square_Area(Square *square)
{
    return square->x * square->x;
}

ShapeInterface *SquareAsShape = &(ShapeInterface) {
    .Area = (double (*)(void *)) square_Area
};

Square *
square_Create(double sideLength)
{
    Square *square = (Square *) malloc(sizeof(Square));
    square->x = sideLength;
    return square;
}