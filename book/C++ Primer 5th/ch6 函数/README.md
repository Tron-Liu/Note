# 第6章

## 6.1 函数基础

### 6.1.1 局部对象

1. 局部静态对象 (local static object)
   局部静态对象在程序的执行路径第一次经过对象定义语句时初始化，并且直到程序终止才被销毁，在此期间即使对象所在的函数结束执行也不会对它有影响

   ```cpp
   size_t count_calls()
   {
     static size_t ctr = 0;
     return ++ctr;
   }

   int main()
   {
     std::cout << count_calls();
     return 0;
   }
   ```
