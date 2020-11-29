#include <stdio.h>
#include "Circle.h"
#include "Square.h"

int
main(int argc, char **argv)
{
    // Create concrete types.
    Circle *circle = circle_Create(5.0);
    Square *square = square_Create(10.0);

    // Wire up the tables.
    Shape *circleShape = shape_Create(circle, CircleAsShape);
    Shape *squareShape = shape_Create(square, SquareAsShape);

    // Sanity check.
    printf("Equal circle areas? %d\n", circle_Area(circle) == shape_Area(circleShape));
    printf("Equal square areas? %d\n", square_Area(square) == shape_Area(squareShape));

    // ... free up memory

    return 0;
}