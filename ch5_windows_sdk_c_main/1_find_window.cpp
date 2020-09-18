#include <windows.h>

int main() {

    HWND h_find = ::FindWindow(NULL, "无标题 - 记事本");

    if (h_find != NULL) {
        ::SendMessage(h_find, WM_CLOSE, 0, 0);
    }

    system("pause");
    return 0;
}