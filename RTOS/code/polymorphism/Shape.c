#include "Shape.h"
#include "stdlib.h"

Shape *
shape_Create(void *instance, ShapeInterface *interface)
{
    Shape *shape = (Shape *) malloc(sizeof(Shape));
    shape->instance = instance;
    shape->interface = interface;
    return shape;
}

double
shape_Area(Shape *shape)
{
    return (shape->interface->Area)(shape->instance);
}