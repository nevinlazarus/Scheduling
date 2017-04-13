#include <iostream>
#include <fstream>
#include "Step.cpp"


int main(int argc, char * argv[]) {
    if (argc < 3) {
      std::cout << "Usage args[1]: input file name, args[2]: ppl per shift,  optional args[3]: amount of leaders" << std::endl;
      return 1;
    }

    std::ifstream input;
    input.open(argv[1]);

    int leaders = 0;
    // if leaders param set
    if (argc >= 4) {
        leaders = atoi(argv[3]);
    }
    State s(atoi(argv[2]), leaders);
    std::string tmp;
    input >> tmp;
    std::string person;
    while (tmp != "end") {
        if (std::isdigit(tmp.back())) {            
            s.addPerson(person);
            s.addTime(person, tmp);
        } else {
            person = tmp;
            
        }
        input >> tmp;
    }    
    s.run();

    return 0;
  
}

