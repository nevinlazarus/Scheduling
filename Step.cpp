#include "Step.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <string>
#include <random>


class State;
class Person;



class Person {
  public:
  Person() {
  
  }
  Person(std::string personName) {
    name = personName;
    leader = name.find("(ex)") != std::string::npos;
  }

  void addTime(std::string t) {
    freeTime[t] = true;

  }

  bool assignTime(std::string t) {
    if (freeTime[t] && !assignedTime[t]) {
      assignedTime[t] = true;
      return true;
    }
    return false;
  }

  bool unassignTime(std::string t) {
    if (assignedTime[t]) {
      assignedTime[t] = false;
      return true;
    }
    return false;
  }

  double countHours() {
    int ret = 0;
    for (auto t : assignedTime) {
      if (t.second) ++ret;
    }
    return (double) ret;
  }

  double countFree() {
   int ret = 0;
    for (auto t : freeTime) {
      if (t.second) ++ret;
    }
    return (double) ret; 
  }

  std::string name;
  bool leader = false;
  std::map<std::string, bool> freeTime;
  std::map<std::string, bool> assignedTime;
};

class State {

  public:
  State() {
    
  }

  public:
  State(State *s) {
     // Map from persons name to their data
     cost = 0;
     peopleList = s->peopleList;
     people = s->people;
     timeToPeople = s->timeToPeople;
     peopleNeeded = s->peopleNeeded;

  }

  double getCost() {
    calculateCost();
    return cost;
  }
  
  std::vector<State> generateNextStates() {
    std::vector<State> ret;
    // find the people who have the most time allocated
    std::vector<std::string> v;
    for (auto person : people) {
      v.push_back(person.first);
    }
    std::sort(v.begin(), v.end(), [&](const std::string &a, const std::string &b) {
        return (people[a].countHours() / people[a].countFree()) < (people[b].countHours() / people[b].countFree());  
        
        });
    for (auto time = peopleNeeded.begin(); time != peopleNeeded.end(); ++time) {
      for (unsigned int i = 0; i < v.size() / 4; ++i) {
        auto person = std::make_pair(v[i], people[v[i]]);
        if (!people[person.first].freeTime[time->first]) {
          continue;
        }
        // the current person hasn't been assigned and is free at that time
        for (auto assignedPerson : timeToPeople[time->first]) {
          State newState = State();
          newState.cost = 0;
          newState.people = people;

          // if we can swap the two people on shift
          if (newState.people[assignedPerson.first].unassignTime(time->first) 
              && newState.people[person.first].assignTime(time->first)) {
            newState.peopleList = peopleList;
            newState.timeToPeople = timeToPeople;
            newState.peopleNeeded = peopleNeeded;
            newState.days = days;
            newState.times = times;

            ret.push_back(newState);
          }
        }
      }
    }
    return ret; 
  }

  void run() {
    std::vector<std::string> randomPeople;
    for (auto p : peopleList) {
      randomPeople.push_back(p.first);
    }

    srand(unsigned(time(NULL)));
    std::random_shuffle(randomPeople.begin(), randomPeople.end());

    // for each slot
    for (auto t = peopleNeeded.begin(); t != peopleNeeded.end(); ++t) {
      // assign as many people as needed
      if (peopleNeeded[t->first] > 0) {
        for (auto p = randomPeople.begin(); p != randomPeople.end(); ++p) {
          // assign time to the person
          if (people[*p].assignTime(t->first) && people[*p].leader) {
            peopleNeeded[*p]--;
            timeToPeople[t->first][*p] = true;
            break;
          }
        }
        for (auto p = people.begin(); p != people.end(); ++p) {
          // assign time to the person
          if (p->second.assignTime(t->first)) {
            peopleNeeded[t->first]--;
            timeToPeople[t->first][p->first] = true;
          }
          if (peopleNeeded[t->first] == 0) {
            break;
          }
        }   
      }
    }
    State bestState = *this;
    std::cout << "HERE" << std::endl;
    for (int i = 0; i < 75; ++i) {
      auto nextStates = bestState.generateNextStates();
      bestState = nextStates[0];
      double lowestCost = bestState.getCost();
      for (auto s = nextStates.begin(); s != nextStates.end(); ++s) {
        auto tmp = s->getCost();
        if (tmp < lowestCost) {
          lowestCost = tmp;
          bestState = *s;
        }
      }
      std::cout << lowestCost << std::endl;
    }
    bestState.print();
    //auto bestState = *this;
   }

  void print() {
    for (auto p : people) {
      std::cout << p.first << ": ";
      for (auto t : people[p.first].assignedTime) {
        if (t.second) {
          std::cout << t.first << " ";
        }
      }
      std::cout << p.second.countHours() / p.second.countFree() << " "; 
      std::cout << p.second.countHours() << " / ";
      std::cout << p.second.countFree() << " ";

      std::cout << std::endl;
    }

  }

  void addPerson(std::string name) {
    if (peopleList[name]) return;
    peopleList[name] = true;
    people[name] = Person(name);
  }

  void addTime(std::string name, std::string timeStr) {
        std::string day = "";
    std::string time = "";
    for (auto c : timeStr) {
      if (!isdigit(c)) {
        day += c;
      } else {
        time += c;
      }
    }
    if (time.size() == 1) {
      time = "0" + time;
    }
    timeStr = day + time;
    peopleNeeded[timeStr] = 8;
    people[name].addTime(timeStr);

    
    days[day] = true;
    times[stoi(time)] = true;
  }
  

  void calculateCost() {
    cost = 0;
    // If people haven't been allocated to a time
    for (auto times : peopleNeeded) {
      cost += times.second * 100;
    }
    std::vector<double> vals;
    for (auto p : people) {
      vals.push_back(people[p.first].countHours() / 
          people[p.first].countFree());
    }
    // minimise standard deviation between time allocated
    double stdev = myStandardDeviation(vals);
    cost += stdev * 1000000;

    // increase cost if people have consecutive shifts
    for (auto p : people) {
      for (auto day : days) {
        double consecutive = 1;
        for (auto time : times) {
          std::string tmp = std::to_string(time.first);
          if (p.second.assignedTime[day.first + tmp]) {
            consecutive *= 7;
          } else {
            cost += consecutive;
            consecutive = 1;
          }
        }
      }
    }
  }

  double cost = 0;
  // Map from persons name to their data
  std::map<std::string, Person> people;
  std::map<std::string, int> peopleNeeded;
  std::map<std::string, bool> peopleList;
  std::map<std::string, std::map<std::string, bool>> timeToPeople;
  std::map<std::string, bool> days;
  std::map<int, bool> times;

};
