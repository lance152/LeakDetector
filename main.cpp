#include <iostream>

class Err{
public:
  Err(int n){
    if(n==0) throw 1000;
    data = new int[n];
  }
  ~Err(){
    delete[] data;
  }
private:
  int *data;
};

int main(){
  int *a = new int;
  int *b = new int;
  delete a;

  try{
    Err *e = new Err(0);
    delete e;
  }catch (int &ex){
    std::cout<<"Exception catch: "<<ex<<std::endl;
  };

  return 0;
}
