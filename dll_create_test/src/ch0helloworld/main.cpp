#include <windows.h>

using namespace std;

int main() {
    MessageBox(NULL, "Hello win32", "标题栏", MB_OKCANCEL);
    return 0;
}

// use X64 - no "const char * \ LPSWSTR" problem