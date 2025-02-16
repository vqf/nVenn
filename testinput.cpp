#include <iostream>
#include "../gh/elements.h"

std::string inp = "a b c\nd e g\n  e  \n";

int main(){
    std::cout << "yo\n";
    nvenn yo(inp, ' ', f);
    yo.showCells();
    yo.showSets();

    std::vector<std::string> g = split(" e", ' ');
    std::cout << g.size() << std::endl;
}
