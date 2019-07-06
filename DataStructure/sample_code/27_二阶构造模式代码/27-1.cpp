#include <stdio.h>

class Test
{
    int mi;
    int mj;
    bool mStatus;
public:
    Test(int i, int j) : mStatus(false)
    {
        mi = i;
        
        return;
        
        mj = j;
        
        mStatus = true;
    }
    int getI()
    {
        return mi;
    }
    int getJ()
    {
        return mj;
    }
    int status()
    {
        return mStatus;
    }
};

int main()
{  
    Test t1(1, 2);
    
    if( t1.status() )
    {
        printf("t1.mi = %d\n", t1.getI());
        printf("t1.mj = %d\n", t1.getJ());
    
    }
    
    return 0;
}
