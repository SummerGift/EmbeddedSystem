#include <stdio.h>
#include <stdlib.h>

int div16(int x)
{
    int biss = (x >> 31) & 0xf; //计算偏置
    printf("biss: %d\n", biss);
    return (x + biss) >> 4;
}

int main()
{
    setbuf(stdout,NULL);
    int test_number = -31;
    int result = 0;

    result = div16(test_number);


    printf("result: %d\n", result);

    printf("result: %d\n", test_number >> 4);
  return 0;
}


