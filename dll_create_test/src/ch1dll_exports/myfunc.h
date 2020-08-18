#ifdef CH1DLLEXPORTS_EXPORTS
#define CH1DLLEXPORTS_API __declspec(dllexport)
#else
#define CH1DLLEXPORTS_API __declspec(dllimport)
#endif

CH1DLLEXPORTS_API int sum(int a, int b);

CH1DLLEXPORTS_API int sub(int a, int b);

class CH1DLLEXPORTS_API Cal {
public:
    int subb(int a, int b);
};

