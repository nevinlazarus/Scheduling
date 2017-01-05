#include <iostream>
#include <fstream>
#include "Step.cpp"


int main(int argc, char * argv[]) {
    if (argc < 3) {
      printf("Need to add how many people per shift and file name");
      return 1;
    }

    std::ifstream input;
    input.open(argv[2]);
    State s(atoi(argv[1]));
    std::string tmp;
    input >> tmp;
    std::string person;
    while (tmp != "end") {
        if (std::isdigit(tmp.back())) {
            
            s.addPerson(person);
            s.addTime(person, tmp);
//            g.addEdge(person, tmp);
        } else {
            person = tmp;
            
        }
        input >> tmp;
    }    
    s.run();

    return 0;
  
}

