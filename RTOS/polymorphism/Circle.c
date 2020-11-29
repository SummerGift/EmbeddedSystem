#include <math.h>
#include "stdlib.h"
#include "Circle.h"

double
circle_Area(Circle *circle)
{
    return M_PI * (circle->radius * circle->radius);
}

ShapeInterface *CircleAsShape = &(ShapeInterface) {
    .Area = (double (*)(void *)) circle_Area
};

Circle *
circle_Create(double radius)
{
    Circle *circle = (Circle *) malloc(sizeof(Circle));
    circle->radius = radius;
    return circle;
}