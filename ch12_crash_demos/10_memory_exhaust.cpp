// Demo 10: 内存耗尽 (Memory Exhaustion) 演示.
// 分配超大内存或不断分配直到内存耗尽，演示如何处理.

#include <windows.h>
#include <cstdio>
#include <vector>

void TestLargeAllocation() {
    printf("\n[测试 1] 尝试分配超大内存块...\n");

    // 尝试分配 100 GB.
    const size_t hugeSize = 100ULL * 1024 * 1024 * 1024;  // 100 GB.
    printf("  尝试分配 %llu GB...\n", hugeSize / (1024ULL * 1024 * 1024));

    void* p = malloc(hugeSize);
    if (p == nullptr) {
        printf("  分配失败，返回 nullptr.\n");
        printf("  正确处理：检查返回值，优雅降级.\n");
    } else {
        printf("  分配成功（不太可能）.\n");
        free(p);
    }
}

void TestExhaustMemory() {
    printf("\n[测试 2] 不断分配直到内存耗尽...\n");
    printf("  每次分配 10MB，直到失败.\n");

    std::vector<void*> blocks;
    const size_t blockSize = 10 * 1024 * 1024;  // 10 MB.
    int count = 0;

    while (true) {
        void* p = malloc(blockSize);
        if (p == nullptr) {
            printf("  第 %d 次分配失败，内存耗尽!\n", count + 1);
            break;
        }

        // 写入数据，确保内存真正被提交.
        memset(p, 0xAB, blockSize);
        blocks.push_back(p);
        count++;

        if (count % 100 == 0) {
            printf("  已分配 %d 个块 (%d MB)...\n", count, count * 10);
        }
    }

    printf("  总共分配了 %d 个块 (%d MB = %.2f GB)\n",
           count, count * 10, count * 10.0 / 1024.0);

    // 释放所有内存.
    printf("  释放所有分配的内存...\n");
    for (void* p : blocks) {
        free(p);
    }
    printf("  内存已释放.\n");
}

void TestGracefulDegradation() {
    printf("\n[测试 3] 优雅降级示例...\n");
    printf("  模拟：程序需要缓存大量数据，内存不足时减少缓存.\n");

    size_t desiredCacheSize = 500 * 1024 * 1024;  // 期望 500 MB.
    size_t actualCacheSize = desiredCacheSize;

    void* cache = nullptr;

    // 逐步减小分配大小，直到成功.
    while (actualCacheSize >= 1024 * 1024) {  // 最小 1 MB.
        cache = malloc(actualCacheSize);
        if (cache != nullptr) {
            printf("  成功分配 %.2f MB 缓存.\n", actualCacheSize / (1024.0 * 1024));
            break;
        }

        printf("  分配 %.2f MB 失败，尝试减半...\n", actualCacheSize / (1024.0 * 1024));
        actualCacheSize /= 2;
    }

    if (cache == nullptr) {
        printf("  严重：连 1MB 都分配不了，进入最低功能模式.\n");
    } else {
        printf("  实际缓存大小: %.2f MB (期望值 %.2f MB)\n",
               actualCacheSize / (1024.0 * 1024),
               desiredCacheSize / (1024.0 * 1024));
        free(cache);
    }
}

void TestMemoryLeak() {
    printf("\n[测试 4] 内存泄漏模拟...\n");
    printf("  分配内存但不释放，模拟泄漏.\n");

    const int leakCount = 100;
    const size_t leakSize = 1024 * 1024;  // 1 MB per leak.

    for (int i = 0; i < leakCount; i++) {
        void* p = malloc(leakSize);
        if (p) {
            memset(p, 0xCD, leakSize);
        }
        // 故意不释放，模拟内存泄漏.
        // free(p);  // 这行被注释掉了.
    }

    printf("  泄漏了 %d MB 内存（这些内存已无法回收）.\n", leakCount);
    printf("  实际程序中请使用智能指针或确保每个 new/malloc 都有对应的 delete/free.\n");
}

int main() {
    printf("=== Demo 10: 内存耗尽 (Memory Exhaustion) ===\n");
    printf("演示内存分配失败的处理策略.\n\n");

    TestLargeAllocation();
    // 注意：TestExhaustMemory 会占用大量内存，可能让系统变卡.
    // TestExhaustMemory();
    TestGracefulDegradation();
    TestMemoryLeak();

    printf("\n=== 测试完成 ===\n");
    printf("工程建议：\n");
    printf("  1. 大内存分配后必须检查返回值是否为 nullptr.\n");
    printf("  2. 内存不足时优雅降级，不要直接崩溃.\n");
    printf("  3. 使用智能指针 (unique_ptr/shared_ptr) 避免内存泄漏.\n");
    printf("  4. 开发阶段用 AddressSanitizer 检测泄漏.\n");
    printf("  5. 定期检查进程内存使用，超过阈值报警.\n");
    return 0;
}
