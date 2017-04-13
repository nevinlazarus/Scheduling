#include "Step.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <string>
#include <random>
#include <fstream>

// Large value = result will priorities hour balance more
#define BALANCE_HOURS_SCALAR (1000000)
// Large value = result will avoid giving people too many consecutive shifts
// This is exponential
#define CONSECUTIVE_HOURS_SCALAR (7)

// Maximum amount of steps it will try to make
// Large value = longer possible time to finish
#define MAX_STEPS (1000)

// enable output of progress
// turn it off if you want it to run faster (but you wont know when it breaks)
#define OUTPUT_PROGRESS (true)

// enable output of the percent of time people have been allocated
#define OUTPUT_PERCENTAGE (true)


class State;
class Person;

class Person {
  public:
    Person() {

    }
    Person(std::string personName) {
      name = personName;
      leader = name.find("leader") != std::string::npos;
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

    State(int people, int leaders=0) {
      peoplePerSlot = people;
      leadersRequired = leaders;
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
          return (people[a].countHours() / people[a].countFree()) < 
          (people[b].countHours() / people[b].countFree());  
          });

      // for each timeslot
      for (auto time = timeslot.begin(); time != timeslot.end(); ++time) {
        // only looks at the quarter of people with proportionally least hours
        // seems to work ok
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

          // the new person hasn't been assigned and is free at that time
          for (auto assignedPerson : peopleList) {
            // if the person isn't assigned at this time
            if (!people[assignedPerson.first].assignedTime[time->first]) continue;
            // don't swap with yourself
            if (assignedPerson.first == newPerson.first) continue;
            if (leaderCount <= leadersRequired) {            
              // cannot replace shift leader with a non-leader
              if (people[assignedPerson.first].leader && 
                  !people[newPerson.first].leader) {  
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
        // assign shift leaders as needed
        for (auto p = randomPeople.begin(); p != randomPeople.end(); ++p) {
          // If enough leaders have been allocated break
          if (timeslot[t->first] <= 0 || timeslot[t->first] < peoplePerSlot - leadersRequired) {
            break;
          }
          // if leader assign timeslot
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
      double prevLowest = bestState.getCost();
      for (int i = 0; i < MAX_STEPS; ++i) {
        auto nextStates = bestState.generateNextStates();
        // if you cannot improve the current state end
        if (nextStates.size() == 0) break;
        bestState = nextStates[0];
        double lowestCost = bestState.getCost();

        for (auto s = nextStates.begin(); s != nextStates.end(); ++s) {
          auto tmp = s->getCost();
          // make sure that its improving
          if (tmp < lowestCost && prevLowest > tmp) {
            lowestCost = tmp;
            bestState = *s;
          }
        }
        // its getting worse, break
        if (prevLowest <= lowestCost) break;
        prevLowest = lowestCost;
        if (OUTPUT_PROGRESS) {
          std::cout << i << " " << lowestCost << std::endl;
        }
      }
      bestState.print();
    }

    void print() {
      std::map<std::string, std::map<std::string, bool>> timeToPeople;

      int total = 0;
      std::cout << "Hours Assigned:" << std::endl;
      for (auto p : people) {
        std::cout << p.first << ": ";
        for (auto t : people[p.first].assignedTime) {
          if (t.second) {
            timeToPeople[t.first][p.first] = true;
          }
        }
        total += p.second.countHours();
        if (OUTPUT_PERCENTAGE) {
          std::cout << p.second.countHours() << " / ";
          std::cout << p.second.countFree() << " ";
          std::cout << (float) p.second.countHours() / p.second.countFree() << " ";                    
          std::cout << std::endl;
        }
      }
      std::cout << "Total Hours Assigned: " << total << std::endl << std::endl;
      for (auto time : timeToPeople) {
        std::cout << time.first << ":" << std::endl;
        for (auto person : time.second) {
          std::cout << "\t" << person.first << std::endl;        
        }
        std::cout << std::endl;
      }
    }

    void addPerson(std::string name) {
      if (peopleList[name]) return;
      peopleList[name] = true;
      people[name] = Person(name);
    }

    // person "name" is available at time "timeStr"
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
      if (cost) {
        // don't recompute
        return;
      }
      cost = 0;
      std::vector<double> vals;
      for (auto p : people) {
        vals.push_back(people[p.first].countHours() / 
            people[p.first].countFree());
      }
      // minimise standard deviation between time allocated
      double stdev = myStandardDeviation(vals);
      cost += stdev * BALANCE_HOURS_SCALAR;

      // exponentially increase cost if people have consecutive shifts
      for (auto p : people) {
        for (auto day : days) {
          double consecutive = 1;
          for (auto time : times) {
            std::string tmp = std::to_string(time.first);
            if (p.second.assignedTime[day.first + tmp]) {
              consecutive *= CONSECUTIVE_HOURS_SCALAR;
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
    int leadersRequired = 0;
    // Map from persons name to their data
    std::map<std::string, Person> people;
    std::map<std::string, int> timeslot;
    std::map<std::string, bool> peopleList;

    std::map<std::string, bool> days;
    std::map<int, bool> times;

};
