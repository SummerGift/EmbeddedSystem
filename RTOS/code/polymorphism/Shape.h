#ifndef __Shape_H__
#define __Shape_H__

typedef struct shape_interface {
    double (*Area)(void *instance);
} ShapeInterface;

typedef struct {
    void *instance;
    const ShapeInterface *interface;
} Shape;

Shape * shape_Create(void *instance, ShapeInterface *interface);
double shape_Area(Shape *shape);

#endif /* end of __Shape_H__ */