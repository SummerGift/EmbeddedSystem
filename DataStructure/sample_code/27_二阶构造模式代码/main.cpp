#include <stdio.h>
#include "IntArray.h"

int main()
{
    IntArray* a = IntArray::NewInstance(5);    
    
    printf("a.length = %d\n", a->length());
    
    a->set(0, 1);
    
    for(int i=0; i<a->length(); i++)
    {
        int v = 0;
        
        a->get(i, v);
        
        printf("a[%d] = %d\n", i, v);
    }
    
    delete a;
    
    return 0;
}
