#include <iostream>
#include <thread>



//void function(string c);
void function(char c){
    //while(true){

    std::cout<<c<<std::endl;
    //}
}



int main(){
    std::thread first(function,'a');
    std::thread second(function, 'b');

    first.join();
    second.join();

    std::cout<<std::thread::hardware_concurrency()<<std::endl;




    return 0;
}