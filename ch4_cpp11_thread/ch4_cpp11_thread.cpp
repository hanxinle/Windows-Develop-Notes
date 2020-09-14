#include <iostream>
#include <thread>
#include <mutex>
#include <windows.h>

using namespace std;

mutex mtx;
static int cnt = 100;

class SaleTicket{
public:
    void Main() {
        ////mtx.lock();    //error
        cout << "In sale " << id << endl;
        for (;;) {
            mtx.lock();
            if (cnt <= 0) {
                mtx.unlock();
                break;
            }
            cnt--;
            mtx.unlock();
            sale++;
            Sleep(1);
        }
    }

    int sale = 0;
    int id = 0;
};


int main() {
    SaleTicket s1;
    SaleTicket s2;
    s1.id = 1;
    s2.id = 2;


    thread th1(&SaleTicket::Main, &s1);
    thread th2(&SaleTicket::Main, &s2);

    th2.join();
    th1.join();

    cout << "s1 sales" << s1.sale << endl;
    cout << "s2 sales" << s2.sale << endl;
    //th1.detach();
    //th2.detach();

    system("pause");
    return 0;
}