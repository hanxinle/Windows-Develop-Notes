#include <windows.h>

using namespace std;

int main() {
    MessageBox(NULL, "窗口显示问题", "标题", MB_OKCANCEL);
    return 0;
}

// use X64 - no "const char * \ LPSWSTR" problem