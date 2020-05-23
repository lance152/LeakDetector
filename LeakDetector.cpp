#include <iostream>
#include <cstring>

#define __NEW_OVERLOAD_IMPLEMENTATION__
#include "LeakDetector.h"

//这里用了双向链表
struct _MemoryHead{
  struct _MemoryHead *next,*prev;
  size_t size; //申请内存大小
  bool isArray;//是否为数组
  char *file; //文件名
  unsigned int line;//行号
};

static unsigned long _memory_allocated = 0; //未释放的内存大小
static _MemoryHead _root = {&_root,&_root,0,false,NULL,0}; //头节点，默认prev和next都指向其本身

unsigned int _leak_detector::callCount = 0;

void * AllocateMemory(size_t _size,bool _array,char* _file,unsigned int _line){
  //从头节点开始分配
  size_t newSize = sizeof(_MemoryHead) + _size;//申请的内存为头部+对象所需的
  _MemoryHead *newNode = (_MemoryHead*) malloc(newSize); //new被重载，需要用malloc

  newNode->next = _root.next;
  newNode->prev = &_root;
  newNode->size = _size;
  newNode->isArray = _array;
  newNode->file = NULL;

  //如果有文件名，则保存
  if(_file){
    newNode->file = (char*)malloc(strlen(_file)+1);
    strcpy(newNode->file,_file);
  }

  newNode->line = _line;

   //更新头节点
   _root.next->prev = newNode;
   _root.next = newNode;

  _memory_allocated += _size; //更新未释放内存

  return (char*)newNode + sizeof(_MemoryHead);//返回真正存放对象的地址
}

void DeleteMemory(void* _ptr,bool _array){
  //找到节点头
  _MemoryHead *currentNode = (_MemoryHead*)((char*)_ptr-sizeof(_MemoryHead));

  if(currentNode->isArray != _array) return;

  currentNode->prev->next = currentNode->next;
  currentNode->next->prev = currentNode->prev;
  _memory_allocated -= currentNode->size;

  if(currentNode->file) free(currentNode->file);//释放保存文件名的内存
  free(currentNode);
}

//重载new
void* operator new(size_t _size){
  return AllocateMemory(_size,false,NULL,0);
}

void* operator new[](size_t _size){
  return AllocateMemory(_size,true,NULL,0);
}

void* operator new(size_t _size,char* _file,unsigned int _line){
  return AllocateMemory(_size,false,_file,_line);
}

void* operator new[](size_t _size,char * _file,unsigned int _line){
  return AllocateMemory(_size,true,_file,_line);
}

//重载delete
void operator delete(void *_ptr) noexcept{
  DeleteMemory(_ptr,false);
}

void operator delete[](void *_ptr) noexcept{
  DeleteMemory(_ptr,true);
}

unsigned int _leak_detector::LeakDetector(void) noexcept{
  unsigned int count = 0;

  //遍历所有头
  _MemoryHead* cur = _root.next;
  while(cur && cur!=&_root){
    if(cur->isArray){
      std::cout<<"leak[]";
    }else{
      std::cout<<"leak";
    }
    std::cout<<cur<<" size "<<cur->size;
    if(cur->file){
      std::cout<<" (locate in "<<cur->file<<" line "<<cur->line<<")";
    }else{
      std::cout<<" (Cannot find position)";
    }
    std::cout<<std::endl;
    ++count;
    cur = cur->next;
  }

  if(count){
    std::cout<<"Total "<<count<<" leaks, size"<<_memory_allocated<<" byte."<<std::endl;
  }
  return count;
}
