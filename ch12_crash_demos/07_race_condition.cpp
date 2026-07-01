// Demo 07: 多线程竞争 (Race Condition) 演示.
// 多个线程同时修改共享数据，不加锁会导致数据竞争和未定义行为.

#include <windows.h>
#include <cstdio>
#include <thread>
#include <vector>

// 共享计数器.
static int g_counter = 0;

// 测试 1：无锁竞争，最终计数器值可能不正确.
void TestRaceConditionNoLock() {
    printf("\n[测试 1] 无锁多线程竞争...\n");
    printf("  10 个线程各增加 10000 次，预期结果 = 100000.\n");

    g_counter = 0;
    const int numThreads = 10;
    const int incrementsPerThread = 10000;

    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back([]() {
            for (int j = 0; j < incrementsPerThread; j++) {
                // 这里没有锁保护，多个线程同时读写 g_counter.
                // 底层汇编是：读 -> 加 1 -> 写，这三步之间可能被其他线程打断.
                g_counter++;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    int expected = numThreads * incrementsPerThread;
    printf("  预期值: %d\n", expected);
    printf("  实际值: %d\n", g_counter);
    printf("  结果: %s\n", (g_counter == expected) ? "正确（运气好）" : "错误（发生竞争）");
}

// 测试 2：使用锁保护.
#include <mutex>
static std::mutex g_mutex;
static int g_counterLocked = 0;

void TestRaceConditionWithLock() {
    printf("\n[测试 2] 使用 std::lock_guard 保护...\n");
    printf("  10 个线程各增加 10000 次，使用互斥锁保护.\n");

    g_counterLocked = 0;
    const int numThreads = 10;
    const int incrementsPerThread = 10000;

    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back([]() {
            for (int j = 0; j < incrementsPerThread; j++) {
                std::lock_guard<std::mutex> lock(g_mutex);
                g_counterLocked++;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    int expected = numThreads * incrementsPerThread;
    printf("  预期值: %d\n", expected);
    printf("  实际值: %d\n", g_counterLocked);
    printf("  结果: %s\n", (g_counterLocked == expected) ? "正确" : "错误");
}

// 测试 3：数据竞争导致链表损坏（更严重的场景）.
struct Node {
    int value;
    Node* next;
};

Node* g_head = nullptr;

void PushNodeUnsafe(int value) {
    Node* node = new Node{value, nullptr};
    // 这两步不是原子的，可能被另一个线程打断.
    node->next = g_head;    // 步骤 1.
    g_head = node;          // 步骤 2.
}

void TestLinkedListRace() {
    printf("\n[测试 3] 链表竞争（可能丢失节点或内存泄漏）...\n");
    printf("  多个线程同时往链表头部插入节点，不加锁.\n");

    // 先清理之前的链表.
    while (g_head) {
        Node* temp = g_head;
        g_head = g_head->next;
        delete temp;
    }

    const int numThreads = 20;
    const int nodesPerThread = 1000;

    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back([i]() {
            for (int j = 0; j < nodesPerThread; j++) {
                PushNodeUnsafe(i * 10000 + j);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // 统计链表节点数.
    int count = 0;
    Node* cur = g_head;
    while (cur) {
        count++;
        cur = cur->next;
    }

    int expected = numThreads * nodesPerThread;
    printf("  预期节点数: %d\n", expected);
    printf("  实际节点数: %d\n", count);
    printf("  结果: %s\n", (count == expected) ? "正确（运气好）" : "错误（发生竞争，部分节点丢失）");

    // 清理链表.
    while (g_head) {
        Node* temp = g_head;
        g_head = g_head->next;
        delete temp;
    }
}

// 测试 4：使用原子变量（C++11）.
#include <atomic>
static std::atomic<int> g_atomicCounter{0};

void TestAtomicCounter() {
    printf("\n[测试 4] 使用 std::atomic 原子操作...\n");
    printf("  10 个线程各增加 10000 次，使用原子变量.\n");

    g_atomicCounter = 0;
    const int numThreads = 10;
    const int incrementsPerThread = 10000;

    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back([]() {
            for (int j = 0; j < incrementsPerThread; j++) {
                g_atomicCounter++;  // 原子自增，线程安全.
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    int expected = numThreads * incrementsPerThread;
    printf("  预期值: %d\n", expected);
    printf("  实际值: %d\n", g_atomicCounter.load());
    printf("  结果: %s\n", (g_atomicCounter.load() == expected) ? "正确" : "错误");
}

int main() {
    printf("=== Demo 07: 多线程竞争 (Race Condition) ===\n");
    printf("多线程不加锁同时修改共享数据会导致数据竞争.\n");
    printf("数据竞争是未定义行为，可能导致：\n");
    printf("  - 数据计算错误\n");
    printf("  - 内存结构损坏\n");
    printf("  - 程序崩溃\n\n");

    TestRaceConditionNoLock();
    TestRaceConditionWithLock();
    TestLinkedListRace();
    TestAtomicCounter();

    printf("\n=== 测试完成 ===\n");
    printf("工程建议：\n");
    printf("  1. 使用 std::mutex / std::lock_guard 保护共享数据.\n");
    printf("  2. 简单计数器可用 std::atomic.\n");
    printf("  3. 避免在锁内做耗时操作，减少竞争.\n");
    printf("  4. 使用读写锁 (std::shared_mutex) 提高读多写少场景的并发.\n");
    return 0;
}
