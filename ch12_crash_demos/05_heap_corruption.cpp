// Demo 05: 堆破坏 (Heap Corruption) 演示.
// 包括：缓冲区溢出、double free、use-after-free.
// 注意：部分测试会导致程序崩溃，建议逐个注释掉测试用例运行.

#include <windows.h>
#include <cstdio>

void TestBufferOverflow() {
    printf("\n[测试 1] 堆缓冲区溢出...\n");
    printf("  分配 16 字节，写入 32 字节，覆盖堆管理结构.\n");

    char* buffer = new char[16];

    // 写入超过分配大小的数据，破坏堆块头.
    // 这可能会在接下来的 free 或后续堆操作时崩溃.
    for (int i = 0; i < 32; i++) {
        buffer[i] = 'A';  // 越界写入.
    }

    printf("  越界写入完成，现在尝试释放...\n");

    // 这里可能会因为堆结构被破坏而崩溃.
    // 在 Debug 模式下通常能检测出来，Release 可能延后崩溃.
    __try {
        delete[] buffer;
        printf("  释放成功（但堆可能已经损坏）.\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  释放时触发异常，堆已被破坏!\n");
    }

    // 再分配一次，验证堆是否已损坏.
    printf("  再次分配内存验证堆状态...\n");
    __try {
        char* buffer2 = new char[16];
        buffer2[0] = 'X';
        delete[] buffer2;
        printf("  二次分配释放成功.\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  二次分配/释放触发异常，确认堆已损坏!\n");
    }
}

void TestDoubleFree() {
    printf("\n[测试 2] Double Free（重复释放）...\n");

    int* p = new int(42);
    printf("  分配内存: %p\n", (void*)p);

    delete p;
    printf("  第一次释放成功.\n");

    // 再次释放同一块内存.
    // 在 Debug 模式下通常能检测出来，Release 可能破坏堆链表.
    printf("  尝试第二次释放同一块内存...\n");
    __try {
        delete p;  // 这里会崩溃或破坏堆.
        printf("  第二次释放居然成功了（Release 模式可能不立即崩溃）.\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  第二次释放触发异常!\n");
    }
}

void TestUseAfterFree() {
    printf("\n[测试 3] Use After Free（释放后使用）...\n");

    int* p = new int(42);
    printf("  分配内存: %p, 值 = %d\n", (void*)p, *p);

    delete p;
    printf("  内存已释放.\n");

    // 尝试读写已释放的内存.
    printf("  尝试读取已释放的内存...\n");
    __try {
        int value = *p;  // 未定义行为.
        printf("  读取成功，值 = %d（这是未定义行为，可能读到垃圾值）.\n", value);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  读取已释放内存触发异常!\n");
    }

    printf("  尝试写入已释放的内存...\n");
    __try {
        *p = 100;  // 未定义行为，可能破坏堆.
        printf("  写入成功（但可能已破坏堆）.\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  写入已释放内存触发异常!\n");
    }
}

void TestHeapEnableTermination() {
    printf("\n[测试 4] 启用 HeapEnableTerminationOnCorruption...\n");
    printf("  这个 API 可以让堆破坏时尽早崩溃，调用栈更准确.\n");

    BOOL result = HeapSetInformation(
        GetProcessHeap(),
        HeapEnableTerminationOnCorruption,
        nullptr,
        0
    );

    if (result) {
        printf("  HeapEnableTerminationOnCorruption 启用成功.\n");
        printf("  现在如果发生堆破坏，程序会立即终止而不是继续运行.\n");
    } else {
        printf("  启用失败，错误码: %lu\n", GetLastError());
    }
}

int main() {

	HeapSetInformation(
		GetProcessHeap(),
		HeapEnableTerminationOnCorruption,
		nullptr,
		0
	);


    printf("=== Demo 05: 堆破坏 (Heap Corruption) ===\n");
    printf("警告：部分测试会导致程序崩溃，建议逐个注释掉运行.\n");
    printf("在 VS 中可以在项目属性里启用 AddressSanitizer 来检测这些问题.\n\n");

    // 建议逐个取消注释测试用例.

     //TestBufferOverflow();
     TestDoubleFree();
    //TestUseAfterFree();
    // TestHeapEnableTermination();

    printf("\n=== 测试完成 ===\n");
    return 0;
}
