#include <iostream>
#include "Step.cpp"


int main(int argc, char * argv[]) {
    if (argc < 2) {
      printf("Need to add how many people per shift");
      return 1;
    }


    State s(atoi(argv[1]));
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
    s.run();

    return 0;
  
}

