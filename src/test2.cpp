
#include <iostream>
#include <fstream>

using namespace std;
int main(){

    std::fstream file;
    file.open("plain.txt");
    if(file.is_open()){
        std::cout << "Awesome";
        
    }
    else{std::cout << "cool";}



    return 0;
}