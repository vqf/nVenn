#include <iostream>
#include "../gh/elements.h"

std::string inp = "a b c\nd e g\n e \n";


std::vector<std::string> setn = {"set1", "set2", "set3"};
std::vector<std::vector<std::string>> setinp = {{"a", "b", "c", "x"}, {"c", "b", "c"}, {"a", "b", "d", "x"}};

int main(){

    nvenn yo;
    for (UINT i = 0; i < setn.size(); i++){
      yo.addSet(setn[i], setinp[i]);
    }
    std::cout << yo.getCode() << std::endl;

}
