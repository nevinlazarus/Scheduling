#include <iostream>
#include "Graph.hpp"


int main(int argc, char * argv[]) {
    Graph g;

    std::string tmp;
    std::cin >> tmp;
    std::string person;
    while (tmp != "end") {
        
        if (std::isdigit(tmp.back())) {
            

            g.addEdge(person, tmp);
        } else {
            person = tmp;
            
        }
        std::cin >> tmp;
    }    

    g.schedule();

    return 0;
}

