#ifndef _INTARRAY_H_
#define _INTARRAY_H_

class IntArray
{
private:
    int m_length;
    int* m_pointer;
    
    IntArray(int len);
    IntArray(const IntArray& obj);
    bool construct();
public:
    static IntArray* NewInstance(int length); 
    int length();
    bool get(int index, int& value);
    bool set(int index ,int value);
    ~IntArray();
};

#endif
