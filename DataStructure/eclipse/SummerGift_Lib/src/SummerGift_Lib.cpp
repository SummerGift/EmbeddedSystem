#include <iostream>
#include "SmartPointer.h"

using namespace std;
using namespace SGLib;

class hellosummer {
public:
    hellosummer() {
        cout << "hello sumer" << endl;
    }

    ~hellosummer() {
        cout << "goodbbye sumer" << endl;
    }
};

int main() {

    SmartPointer<hellosummer> sp = new hellosummer();
    SmartPointer<hellosummer> s;

    s = sp;

    cout << "sp= " << sp.isNull() << endl;
    cout << "s= " << s.isNull() << endl;

    //s++;

    cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
    return 0;
}
