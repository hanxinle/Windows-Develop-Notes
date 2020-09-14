#include <iostream>
#include <windows.h>
using namespace std;

class A {
public:
    A() {
        cout << "Constructor Called." << endl;
    }
    ~A() {
        cout << "Destructor Called." << endl;
    }
};


int main3() {
    A obj;
    ::ExitProcess(0);
    return 0;
}