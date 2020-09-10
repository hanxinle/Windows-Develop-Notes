#include <stdio.h>
#include <Windows.h>

class MyClass {
public:
    MyClass() {
        printf("constructor called\n");
    }
    ~MyClass() {
        printf("destructor called\n");
    }
};


int main2() {
    MyClass obj1;
    ::ExitThread(0);

    system("pause");
    return 0;
}