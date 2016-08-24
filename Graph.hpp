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
    
    void addNode(std::string name, bool isPerson) {
        nodes[name] = Node(name, isPerson);
    }
    
    void addEdge(std::string u, std::string v, int w=1) {

        if (!nodes.count(u)) {
            addNode(u, true); //adds a person node
        }
        if (!nodes.count(v)) {
            addNode(v, false); //adds a time node
        }
        nodes[u].addEdge(v, w);
    }
    
    void removeEdge(std::string u, std::string v) {
        nodes[u].removeEdge(v);
    }
    
    //Runs a network flow algorithm to find the min cut
    //Such that people can be scheduled to their available time slots
    void schedule() {

        
        //set up the graph
        
        addNode("src", false);
        addNode("sink", false);
        
        //for each node in the graph
        for (auto n : nodes) {             
            if (n.first != "src" && n.first != "sink") {
                if (n.second.isPerson) { //its a person
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
            for (auto p : path) {
                std::cout << p << " ";
            }
            std::cout << std::endl << "------------------------------" << std::endl;    
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
            std::cout << t << "\t";// << std::endl;
            for (auto connections : nodes[t].edges) {
                if (connections.first == "sink") continue;
                //check who is available for those times
                std::cout << connections.first << "\t";
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

    //checks if there is a path from source to dest
    //stores it in path
    bool BFS(const std::string &src, const std::string &dest, std::list<std::string> &path) {

        if (src == dest) return true;
        //priority queue of nodes to visit
        std::priority_queue<std::pair<int, std::string>> q; 
        //spanning tree for the graph
        //stores the previous node visited
        std::map<std::string, std::pair<std::string, int>> span;

        span[src] = {"", 0};
        q.push(std::pair<int, std::string>(0, src));
        while (q.size()) {            
            
            auto pair = q.top(); q.pop();            
            auto currQValue = pair.first;
            auto curr = pair.second;
            if (pair.second == dest) break;
            //std::cout << pair.second << std::endl;
            for (auto n : nodes[curr].edges) {

                if (n.first == src || n.second == 0) continue; //if its the source node or the edge's weight is 0
                
                if (span[n.first].first == "" || !nodes[n.first].isPerson) { //nodes is unvisited
                    int queueVal = 0;
                    if (nodes[n.first].isPerson) {
                        //pushes the person node onto the queue                       
                        
                        if (availableHours[n.first] - (hours[n.first]) == 0) continue; //a person has no free hours
                        queueVal = 1000  / (hours[n.first] + 1);
                    } else {                
                        //pushes the times to the end
                        queueVal = -1000000 + (1000 / (hours[curr] + 1));
                    }
                    
                                        
                    if (span[n.first].first == "" || span[n.first].second < currQValue) {
                        span[n.first] = std::pair<std::string, int>(curr, currQValue);
                        q.push(std::pair<int, std::string>(queueVal, n.first));        
                    }
                }
            }
        }
        
        if (span[dest].first == "") return false;
        std::string curr = span[dest].first;
        path.push_front(dest);
        while (curr != "") {
            path.push_front(curr);
            curr = span[curr].first;
        }
        return true;
    }
    
    class Node {
        friend class Graph;
        public:
        Node() { }    
        Node(std::string s, bool person) : name{s}, flowCount{0}, isPerson{person} { }
        
        void addEdge(std::string v, int value = 1) {
            flowCount += value;
            edges[v] = value;            
        }
        
        void removeEdge(std::string v) {
            edges.erase(v);
        }
        
        
        
        private:
        std::string name;
        int flowCount;
        bool isPerson;
        std::map<std::string, int> edges;        
    };
    
    private:
    
    std::map<std::string, Node> nodes;
    std::map<std::string, int> hours; //hours people have been assigned
    std::vector<std::string> people; //list of people
    std::vector<std::string> time; //list of time slots
    std::map<std::string, int> availableHours; //hours people have totally
    int maxHours = 20;
    int maxPeople = 8;
    
};


