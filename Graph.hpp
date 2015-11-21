#include <iostream>
#include <algorithm>
#include <map>
#include <queue>
#include <list>
#include <vector>


class Graph {
    public:
    
    //constructor
    Graph() {          
        
    }
    
    void addNode(std::string name) {
        nodes[name] = Node(name);
    }
    
    void addEdge(std::string u, std::string v, int w=1) {
        if (!nodes.count(v)) {
            addNode(v);
        }
        if (!nodes.count(u)) {
            addNode(u);
        }
        nodes[u].addEdge(v, w);
    }
    
    void removeEdge(std::string u, std::string v) {
        nodes[u].removeEdge(v);
    }
    
    //Runs a network flow algorithm to find the min cut
    //Such that people can be scheduled to their available time slots
    void schedule() {

        const int maxHours = 15;
        const int maxPeople = 3;
        //set up the graph
        
        addNode("src");
        addNode("sink");
        
        
        std::vector<std::string> people; //list of people
        std::vector<std::string> time; //list of time slots
        std::map<std::string, int> availableHours;
        
        //for each node in the graph
        for (auto n : nodes) {             
            if (n.first != "src" && n.first != "sink") {
                if (n.second.edges.size()) { //its a person
                    availableHours[n.first] += n.second.edges.size();
                    addEdge("src", n.first, maxHours);
                    people.push_back(n.first);
                } else { //its a time               
                    addEdge(n.first, "sink", maxPeople);
                    time.push_back(n.first);                    
                }
            }
            
        }
        
        hours.clear();        
        std::list<std::string> path;
        //while there is a path from source to sink
        while (BFS("src", "sink", path)) {            
            std::string prev = "";
            for (std::string i : path) { 
                if (prev != "") {
                    //reverses the edge between the nodes
                    //to assign the hour to the person
                    --nodes[prev].edges[i];
                    if (nodes[prev].edges[i] == 0) {
                        nodes[prev].removeEdge(i);
                    }   
                    ++hours[i];
                    ++nodes[i].edges[prev];
                }
                
                prev = i;
            }
            path.clear();
        }
                        
        //for each timeslot
        for (auto t : time) {
            std::cout << "People for time: " << t << std::endl;
            for (auto connections : nodes[t].edges) {
                if (connections.first == "sink") continue;
                //check who is available for those times
                std::cout << connections.first << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl << "Hours Per Person: " << std::endl; //prints hours assigned out of hours available
        for (auto p : people) {
            std::cout << p << ": " << hours[p] << "/" << availableHours[p] << std::endl;
        }
        
        std::cout << "Checking completition: " << std::endl;
        int total = 0;
        for (auto e : nodes["sink"].edges) {
            if (e.second < maxPeople) { //if that time slot is not filled
                std::cout << e.first << std::endl;
            }
            total += e.second;
        }
        //prints true if all the time slots are filled
        std::cout << std::boolalpha << (total == (int) (time.size() * maxPeople)) << std::endl; 
        
        
        
    }

    
    bool BFS(const std::string &src, const std::string &dest, std::list<std::string> &path) {

        if (src == dest) return true;
        
        std::priority_queue<std::pair<int, std::string>> q;
        std::map<std::string, std::string> span;

        span[src] = "";
        q.push(std::pair<int, std::string>(-1, src));
        while (q.size()) {            
            
            auto pair = q.top(); q.pop();            
            auto curr = pair.second;
            if (pair.second == dest) break;
            
            for (auto n : nodes[curr].edges) {

                if (n.first == src || n.second == 0) continue;
                
                if (span[n.first] == "") {
                    q.push(std::pair<int, std::string>(-1 * hours[n.first], n.first));
                    span[n.first] = curr;
                }
            }
        }
        
        if (span[dest] == "") return false;
        std::string curr = span[dest];
        path.push_front(dest);
        while (curr != "") {
            path.push_front(curr);
            curr = span[curr];
        }
        return true;
    }
    
    class Node {
        friend class Graph;
        public:
        Node() { }    
        Node(std::string s) : name{s} { }
        
        void addEdge(std::string v, int value = 1) {

            edges[v] = value;
        }
        
        void removeEdge(std::string v) {
            edges.erase(v);
        }
        
        private:
        std::string name;
        std::map<std::string, int> edges;        
    };
    
    private:
    
    std::map<std::string, Node> nodes;
    std::map<std::string, int> hours;
    
};


