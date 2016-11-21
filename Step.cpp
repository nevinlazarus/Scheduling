#include "Step.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <string>
#include <random>
#include <assert.h>
#include <ctime>

#include <thread>
#include <mutex>

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

  unsigned long long countHours() {
    int ret = 0;
    for (auto t : assignedTime) {
      if (t.second) ++ret;
    }
    return (unsigned long long) ret;
  }

  unsigned long long countFree() {
   int ret = 0;
    for (auto t : freeTime) {
      if (t.second) ++ret;
    }
    return (unsigned long long) ret; 
  }

  std::string name;
  bool leader = false;
  std::map<std::string, bool> freeTime;
  std::map<std::string, bool> assignedTime;
};

class State {

  public:
  State() {
    cost = 0;  
  }

  public:
  State(State *s) {
     // Map from persons name to their data
     cost = 0;
     peopleList = s->peopleList;
     people = s->people;
     timeToPeople = s->timeToPeople;
     timeslot = s->timeslot;

  }

  unsigned long long getCost() {
    if (!cost) {
      calculateCost();
    }
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
    for (auto time = timeslot.begin(); time != timeslot.end(); ++time) {
      for (unsigned int i = 0; i < v.size() / 4; ++i) {
        auto person = std::make_pair(v[i], people[v[i]]);
        if (!people[person.first].freeTime[time->first]) {
          continue;
        }
        int amountOfLeaders = 0;
        for (auto assignedPerson : timeToPeople[time->first]) {
          if (assignedPerson.first.find("(ex)") != std::string::npos) {
            ++amountOfLeaders;
          }
        }
        // the current person hasn't been assigned and is free at that time
        for (auto assignedPerson : timeToPeople[time->first]) {
          State newState = State();
          newState.people = people;
          // if you are replacing a leader with non-leader, 
          // check the shift has enough leaders
          if (people[assignedPerson.first].leader && !person.second.leader 
              && amountOfLeaders <= requiredLeaders) {
            continue;
          }
          // if we can swap the two people on shift
          if (newState.people[assignedPerson.first].unassignTime(time->first) 
              && newState.people[person.first].assignTime(time->first)) {
            newState.timeToPeople = timeToPeople;
            newState.timeToPeople[time->first][assignedPerson.first] = false;
            newState.timeToPeople[time->first][person.first] = true;
            newState.timeslot = timeslot;
            newState.days = days;
            newState.times = times;

            ret.push_back(newState);
          }
        }
      }
    }
    return ret; 
  }

  bool run(unsigned long long randSeed) {
    std::vector<std::string> randomPeople;
    for (auto p : peopleList) {
      randomPeople.push_back(p.first);
    }

    // randomly shuffle the people
    srand(time(NULL) + randSeed);
    std::random_shuffle(randomPeople.begin(), randomPeople.end());

    // for each slot
    for (auto t = timeslot.begin(); t != timeslot.end(); ++t) {
      // assign as many people as needed
      // first assign leaders
      if (timeslot[t->first] > 0) {
        int leaderCount = 0;
        for (auto p = randomPeople.begin(); p != randomPeople.end(); ++p) {
          // assign time to the person
          if (people[*p].assignTime(t->first) && people[*p].leader) {
            timeslot[t->first]--;
            timeToPeople[t->first][*p] = true;
            ++leaderCount;
            if (leaderCount == requiredLeaders) {
              break;
            }
          }
        }
        for (auto p = randomPeople.begin(); p != randomPeople.end(); ++p) {
          // assign time to the person
          if (people[*p].assignTime(t->first)) {
            timeslot[t->first]--;
            timeToPeople[t->first][*p] = true;
          }
          if (timeslot[t->first] == 0) {
            break;
          }
        }   
      }
    }
    State bestState = *this;
    std::cout << "HERE" << std::endl;
    std::vector<unsigned long long> costProgress;

    // creates threads
    int numThreads = std::thread::hardware_concurrency() - 1; 
    std::vector<std::thread> threads;
    std::vector<int> threadBestIndex(numThreads);
    std::cout << numThreads << std::endl;

    for (int i = 0; i < 75; ++i) {
      auto nextStates = bestState.generateNextStates();
      for (int threadID = 0; threadID < numThreads; ++threadID) {
        std::thread t ([threadID, numThreads, &threadBestIndex, &nextStates, this]{ 
            threadBestIndex[threadID] = getBestState(nextStates, threadID, numThreads);
            });
        threads.push_back(std::move(t)); 
      }
      unsigned long long lowestCost = 1e15;
      for (int threadID = 0; threadID < numThreads; ++threadID) {
        threads[threadID].join();
        threads.erase(threads.begin());   
        int idx = threadBestIndex[threadID];
        if (nextStates[idx].cost < lowestCost) {
          lowestCost = nextStates[idx].cost; 
          bestState = nextStates[idx];
        }    
      }
      
      lowestCost = bestState.getCost();
      costProgress.push_back(lowestCost);
      // if its not making progress, exit
      if (i > 5 && costProgress[i - 5] - lowestCost < 5) return false;
      std::cout << lowestCost << std::endl;
    }
    bestState.print();
    return true;
  }

  int getBestState(const std::vector<State> &nextStates, int threadID, int numThreads) {
    int ret = 0;
    State bestState = nextStates[0];
    unsigned long long lowestCost = bestState.getCost();
    for (unsigned i = 0; i < nextStates.size(); i += numThreads) {
      auto s = nextStates[i];
      auto tmp = s.getCost();
      if (tmp < lowestCost) {
        lowestCost = tmp;
        ret = i;
      }
    }
    return ret;
  }

  // Print out the time allocations of each person
  void print() {
    for (auto p : people) {
      std::cout << p.first << ": ";
      int mx = 0;
      for (auto day : days) {
        int consecutive = 1;
        for (auto time : times) {
          std::string tmp = std::to_string(time);
          if (p.second.assignedTime[day + tmp]) {
            consecutive++;
            mx = std::max(consecutive, mx);
          } else {
            consecutive = 1;
          }
        }
      }
      std::cout << "CONSEC: " << mx << std::endl;
      for (auto t : people[p.first].assignedTime) {
        if (t.second) {
          assert(people[p.first].freeTime[t.first]);
          //std::cout << t.first << " ";
        }
      }
      std::cout << (double) p.second.countHours() / p.second.countFree() << " "; 
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
    // front pad with 0 if its a single digit
    if (time.size() == 1) {
      time = "0" + time;
    }
    timeStr = day + time;
    timeslot[timeStr] = 6;
    people[name].addTime(timeStr);

    // the day doesn't exist
    if (find(days.begin(), days.end(), day) == days.end()) {
      days.push_back(day);
    }
    if (find(times.begin(), times.end(), stoi(time)) == times.end()) {
      times.push_back(stoi(time));
    }

  }
  

  void calculateCost() {
    cost = 0;
    // If people haven't been allocated to a time
    for (auto times : timeslot) {
      cost += times.second * 100;
    }
    std::vector<double> vals;
    double mx = 0;
    double mn = 1;
    for (auto p : people) {
      mx = std::max(mx, (double) people[p.first].countHours() / 
          people[p.first].countFree());
      mn = std::min(mn, (double) people[p.first].countHours() / 
          people[p.first].countFree());
      vals.push_back(people[p.first].countHours() / people[p.first].countFree());
    }
    // minimise standard deviation between time allocated
    cost += myStandardDeviation(vals) * 100000;
//    cost += (mx - mn) * 100000;

    // increase cost if people have consecutive shifts
    for (auto p : people) {
      for (auto day : days) {
        unsigned long long consecutive = 1;
        for (auto time : times) {
          std::string tmp = std::to_string(time);
          if (p.second.assignedTime[day + tmp]) {
            consecutive *= 10;
          } else {
            cost += consecutive;
            consecutive = 1;
          }
        }
      }
    }
  }

  unsigned long long cost = 0;
  // Map from persons name to their data
  std::map<std::string, Person> people;
  // Map from each timeslot to amount of people still required
  std::map<std::string, int> timeslot;
  std::map<std::string, bool> peopleList;
  std::map<std::string, std::map<std::string, bool>> timeToPeople;
  std::vector<std::string> days;
  std::vector<int> times;

  int requiredLeaders = 2;

};
