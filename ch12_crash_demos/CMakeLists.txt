cmake_minimum_required(VERSION 3.16)
project(CrashDemos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows 平台设置.
if(WIN32)
    # 生成 PDB（即使 Release 也要）.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /DEBUG")
endif()

# 01: VEH 演示.
add_executable(demo_01_veh 01_veh.cpp)
target_link_libraries(demo_01_veh PRIVATE DbgHelp)

# 02: SEH 演示.
add_executable(demo_02_seh 02_seh.cpp)
target_link_libraries(demo_02_seh PRIVATE DbgHelp)

# 03: UEF + MiniDump.
add_executable(demo_03_uef_minidump 03_uef_minidump.cpp)
target_link_libraries(demo_03_uef_minidump PRIVATE DbgHelp)

# 04: 空指针/野指针.
add_executable(demo_04_nullptr 04_nullptr.cpp)

# 05: 堆破坏.
add_executable(demo_05_heap_corruption 05_heap_corruption.cpp)

# 06: 栈溢出.
add_executable(demo_06_stack_overflow 06_stack_overflow.cpp)

# 07: 多线程竞争.
add_executable(demo_07_race_condition 07_race_condition.cpp)

# 08: 未初始化变量.
add_executable(demo_08_uninitialized 08_uninitialized.cpp)

# 09: DLL 加载.
add_executable(demo_09_dll_load 09_dll_load.cpp)

# 10: 内存耗尽.
add_executable(demo_10_memory_exhaust 10_memory_exhaust.cpp)

# 11: 纯虚函数调用.
add_executable(demo_11_pure_virtual 11_pure_virtual.cpp)

# 12: 断言.
add_executable(demo_12_assert 12_assert.cpp)

# 13: 全局静态变量初始化顺序.
add_executable(demo_13_static_init
    13_static_init_order_main.cpp
    13_static_init_order_a.cpp
    13_static_init_order_b.cpp
)

# 14: 跨模块内存分配.
# 先编译 DLL.
add_library(my_dll SHARED 14_cross_module_dll.cpp)
target_compile_definitions(my_dll PRIVATE MYDLL_EXPORTS)
# 再编译主程序.
add_executable(demo_14_cross_module 14_cross_module_main.cpp)
add_dependencies(demo_14_cross_module my_dll)
