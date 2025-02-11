#include <iostream>
#include "../gh/elements.h"

std::string inp = "a b c\nd e g\ne  \n";

int main(){
    std::cout << "yo\n";
    nvenn yo(inp);
    yo.showCells();
}
