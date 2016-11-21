#include <iostream>
#include "Step.cpp"


int main(int argc, char * argv[]) {


    State s;
    std::string tmp;
    std::cin >> tmp;
    std::string person;
    while (tmp != "end") {
        if (std::isdigit(tmp.back())) {
            
            s.addPerson(person);
            s.addTime(person, tmp);
//            g.addEdge(person, tmp);
        } else {
            person = tmp;
            
        }
        std::cin >> tmp;
    }
    int i = 0; 
    while (!s.run(i++));

    return 0;
  
}

