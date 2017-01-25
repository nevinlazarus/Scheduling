#include "Step.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <string>
#include <random>
#include <fstream>


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
    if (freeTime[t] && assignedTime[t]) {
      assignedTime.erase(t);
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

  State(int num) {
    peoplePerSlot = num;
  }

  public:
  State(State *s) {
     // Map from persons name to their data
     cost = 0;
     peopleList = s->peopleList;
     people = s->people;

     timeslot = s->timeslot;
     peoplePerSlot = s->peoplePerSlot;
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
    // sorts the people by the proportion of time they've been shifted on
    std::sort(v.begin(), v.end(), [&](const std::string &a, const std::string &b) {
        return (people[a].countHours() / people[a].countFree()) < (people[b].countHours() / people[b].countFree());  
        });
    
    // for each timeslot
    for (auto time = timeslot.begin(); time != timeslot.end(); ++time) {
      for (unsigned int i = 0; i < v.size() / 4; ++i) {
        auto newPerson = std::make_pair(v[i], people[v[i]]);
        // if they are already assigned to this timeslot
        
        // if they are not free at this time
        if (!people[newPerson.first].freeTime[time->first]) {
          continue;
        }
        int leaderCount = 0;
        for (auto p : peopleList) {
          if (people[p.first].leader && people[p.first].assignedTime[time->first]) {           
            leaderCount++;            
          }
        }
        
        // the current person hasn't been assigned and is free at that time
        for (auto assignedPerson : peopleList) {
          // if the person isn't assigned
          if (!people[assignedPerson.first].assignedTime[time->first]) continue;
          // don't swap with yourself
          if (assignedPerson.first == newPerson.first) continue;
          if (leaderCount <= leadersRequired) {            
            // cannot replace shift leader with a non-leader
            if (people[assignedPerson.first].leader && !people[newPerson.first].leader) {  
              continue;
            }
          }
          State newState = State();
          newState.cost = 0;
          newState.people = people;

          // if we can swap the two people on shift
          // create a new state
          if (newState.people[assignedPerson.first].unassignTime(time->first) 
              && newState.people[newPerson.first].assignTime(time->first)) {
            newState.peopleList = peopleList;
            
            newState.timeslot = timeslot;
            newState.days = days;
            newState.times = times;
            newState.people[assignedPerson.first].unassignTime(time->first);
            newState.people[newPerson.first].assignTime(time->first);
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

	// randomly shuffles the people
    srand(unsigned(time(NULL)));
    std::random_shuffle(randomPeople.begin(), randomPeople.end());

    // for each timeslot
    for (auto t = timeslot.begin(); t != timeslot.end(); ++t) {
      // assign as many people as needed
      for (auto p = randomPeople.begin(); p != randomPeople.end(); ++p) {
        // If enough leaders have been allocated break
        if (timeslot[t->first] <= 0 || timeslot[t->first] < peoplePerSlot - leadersRequired) {
          break;
        }
        // assign shift leaders to timeslots first
        if (people[*p].leader && people[*p].assignTime(t->first)) {
          timeslot[t->first]--;
        }
      }
      // assign all people as normal
      for (auto p = randomPeople.begin(); p != randomPeople.end(); ++p) {
        if (timeslot[t->first] <= 0) {
          break;
        }
        // assign time to the person
        if (people[*p].assignTime(t->first)) {
          timeslot[t->first]--;
        }
      }   
      
    }
    State bestState = *this;
    for (int i = 0; i < 100; ++i) {
      auto nextStates = bestState.generateNextStates();
      // if you cannot improve the current state end
      if (nextStates.size() == 0) break;
      bestState = nextStates[0];
      double lowestCost = bestState.getCost();
      for (auto s = nextStates.begin(); s != nextStates.end(); ++s) {
        auto tmp = s->getCost();
        if (tmp < lowestCost) {
          lowestCost = tmp;
          bestState = *s;
        }
      }
      //std::cout << i << " " << lowestCost << std::endl;
    }
    //std::cout << bestState.getCost() << std::endl;
    bestState.print();
    //auto bestState = *this;
   }

  void print() {
    std::map<std::string, std::map<std::string, bool>> timeToPeople;
    //std::ofstream out;
    //out.open("output.txt");
    int total = 0;
    std::cout << "Hours Assigned:" << std::endl;
    for (auto p : people) {
      std::cout << p.first << ": ";
      for (auto t : people[p.first].assignedTime) {
        if (t.second) {
          //std::cout << t.first << " ";
          timeToPeople[t.first][p.first] = true;
        }
      }
      std::cout << p.second.countHours() << " / ";
      std::cout << p.second.countFree() << " ";
      total += p.second.countHours();
      std::cout << std::endl;
    }
    std::cout << "Total Hours Assigned: " << total << std::endl << std::endl;
    for (auto time : timeToPeople) {
      std::cout << time.first << ":" << std::endl;
      for (auto person : time.second) {
        std::cout << "\t" << person.first << std::endl;        
      }
      std::cout << std::endl;
    }
    //out.close();
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
    timeslot[timeStr] = peoplePerSlot;
    people[name].addTime(timeStr);

    
    days[day] = true;
    times[stoi(time)] = true;
  }
  

  void calculateCost() {
    cost = 0;
    // If people haven't been allocated to a time
    for (auto times : timeslot) {
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
        cost += consecutive;
      }
    }
  }

  double cost = 0;
  int peoplePerSlot = 0;
  int leadersRequired = 2;
  // Map from persons name to their data
  std::map<std::string, Person> people;
  std::map<std::string, int> timeslot;
  std::map<std::string, bool> peopleList;
  
  std::map<std::string, bool> days;
  std::map<int, bool> times;

};
