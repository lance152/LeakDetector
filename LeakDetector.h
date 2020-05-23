#ifndef _LEAKDETECTOR_H_
#define _LEAKDETECTOR_H_

//重载全局new，new[]
void* operator new(size_t _size,char *_file,unsigned int _line);
void* operator new[](size_t _size,char *_file,unsigned int _line);

//这里我们用了内置宏来获取文件名及行号
#ifndef __NEW_OVERLOAD_IMPLEMENTATION__
#define new new(__FILE__,__LINE__)
#endif

class _leak_detector{
public:
  static unsigned int callCount; //这里定义了一个静态成员变量确保最后析构时才检查
  _leak_detector() noexcept {
    ++callCount;
  }

  ~_leak_detector() noexcept{
    if(--callCount==0) LeakDetector();
  }

private:
  static unsigned int LeakDetector() noexcept;
};

static _leak_detector _exit_counter;

#endif
