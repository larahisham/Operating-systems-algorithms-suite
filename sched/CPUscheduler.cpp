/*************************************************************************************
 *  CPU Scheduler (Multiple Algorithms)
 *  Algorithms: LJF, FCFS, SJF, Round Robin, Priority, SRTF
 ************************************************************************************/
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <queue>
#include <climits>
using namespace std;

struct Process {
    string name;
    int arrivalTime;
    int processingTime;
    int remainingTime;
    int priority;
    int responseTime;
    int turnaround;
    int delay;
    int startTime;
    bool hasStarted;
    
    Process(string name, int arrivalTime, int processingTime, int priority = 0) :
       name(name), arrivalTime(arrivalTime), processingTime(processingTime),
       remainingTime(processingTime), priority(priority),
       responseTime(0), turnaround(0), delay(0), startTime(-1), hasStarted(false) {}
    
    // Comparators
    static bool compareLJF(const Process& p1, const Process& p2) {
        if (p1.processingTime != p2.processingTime)
            return p1.processingTime > p2.processingTime;
        return p1.arrivalTime < p2.arrivalTime;
    }
    
    static bool compareSJF(const Process& p1, const Process& p2) {
        if (p1.processingTime != p2.processingTime)
            return p1.processingTime < p2.processingTime;
        return p1.arrivalTime < p2.arrivalTime;
    }
    
    static bool compareFCFS(const Process& p1, const Process& p2) {
        return p1.arrivalTime < p2.arrivalTime;
    }
    
    static bool comparePriority(const Process& p1, const Process& p2) {
        if (p1.priority != p2.priority)
            return p1.priority < p2.priority; // Lower number = higher priority
        return p1.arrivalTime < p2.arrivalTime;
    }
    
    static bool compareSRTF(const Process& p1, const Process& p2) {
        if (p1.remainingTime != p2.remainingTime)
            return p1.remainingTime < p2.remainingTime;
        return p1.arrivalTime < p2.arrivalTime;
    }
};

class CPUScheduler {
private:
    vector<Process> processes;
    vector<string> executionOrder;
    
    void resetProcesses() {
        for (auto& p : processes) {
            p.remainingTime = p.processingTime;
            p.responseTime = 0;
            p.turnaround = 0;
            p.delay = 0;
            p.startTime = -1;
            p.hasStarted = false;
        }
        executionOrder.clear();
    }

public:
    void readInput(const string& filename, bool readPriority = false) {
        ifstream file(filename);
        if (!file) {
            cerr << "Error opening file: " << filename << endl;
            exit(1);
        }
        
        int numProcesses;
        file >> numProcesses;
        file.ignore();
        
        string line, name;
        int arrivalTime, processingTime, priority;
        
        for (int i = 0; i < numProcesses; i++) {
            getline(file, line);
            stringstream s_stream(line);
            s_stream >> name >> arrivalTime >> processingTime;
            
            if (readPriority && s_stream >> priority) {
                processes.emplace_back(name, arrivalTime, processingTime, priority);
            } else {
                processes.emplace_back(name, arrivalTime, processingTime, i);
            }
        }
        file.close();
    }
    
    // 1. FCFS (First Come First Serve)
    void scheduleFCFS() {
        resetProcesses();
        vector<Process> processesCopy = processes;
        vector<Process> finishedProcesses;
        
        sort(processesCopy.begin(), processesCopy.end(), Process::compareFCFS);
        
        int currentTime = 0;
        
        for (auto& p : processesCopy) {
            if (currentTime < p.arrivalTime) {
                currentTime = p.arrivalTime;
            }
            
            p.responseTime = currentTime - p.arrivalTime;
            p.startTime = currentTime;
            currentTime += p.processingTime;
            p.turnaround = currentTime - p.arrivalTime;
            p.delay = p.turnaround - p.processingTime;
            
            executionOrder.push_back(p.name);
            finishedProcesses.push_back(p);
        }
        
        writeOutput("out_fcfs.txt", finishedProcesses);
    }
    
    // 2. SJF (Shortest Job First)
    void scheduleSJF() {
        resetProcesses();
        vector<Process> processesCopy = processes;
        vector<Process> readyQueue;
        vector<Process> finishedProcesses;
        int currentTime = 0;
        
        while (!processesCopy.empty() || !readyQueue.empty()) {
            for (auto it = processesCopy.begin(); it != processesCopy.end();) {
                if (it->arrivalTime <= currentTime) {
                    readyQueue.push_back(*it);
                    it = processesCopy.erase(it);
                } else {
                    ++it;
                }
            }
            
            if (readyQueue.empty()) {
                currentTime = processesCopy.front().arrivalTime;
                continue;
            }
            
            sort(readyQueue.begin(), readyQueue.end(), Process::compareSJF);
            
            Process p = readyQueue.front();
            readyQueue.erase(readyQueue.begin());
            
            p.responseTime = max(0, currentTime - p.arrivalTime);
            int endTime = currentTime + p.processingTime;
            p.turnaround = endTime - p.arrivalTime;
            p.delay = p.turnaround - p.processingTime;
            currentTime = endTime;
            
            executionOrder.push_back(p.name);
            finishedProcesses.push_back(p);
        }
        
        writeOutput("out_sjf.txt", finishedProcesses);
    }
    
    // 3. LJF (Longest Job First) 
    void scheduleLJF() {
        resetProcesses();
        vector<Process> processesCopy = processes;
        vector<Process> readyQueue;
        vector<Process> finishedProcesses;
        int currentTime = 0;
        
        while (!processesCopy.empty() || !readyQueue.empty()) {
            for (auto it = processesCopy.begin(); it != processesCopy.end();) {
                if (it->arrivalTime <= currentTime) {
                    readyQueue.push_back(*it);
                    it = processesCopy.erase(it);
                } else {
                    ++it;
                }
            }
            
            if (readyQueue.empty()) {
                currentTime = processesCopy.front().arrivalTime;
                continue;
            }
            
            sort(readyQueue.begin(), readyQueue.end(), Process::compareLJF);
            
            Process p = readyQueue.front();
            readyQueue.erase(readyQueue.begin());
            
            p.responseTime = max(0, currentTime - p.arrivalTime);
            int endTime = currentTime + p.processingTime;
            p.turnaround = endTime - p.arrivalTime;
            p.delay = p.turnaround - p.processingTime;
            currentTime = endTime;
            
            executionOrder.push_back(p.name);
            finishedProcesses.push_back(p);
        }
        
        writeOutput("out_ljf.txt", finishedProcesses);
    }
    
    // 4. Round Robin
    void scheduleRoundRobin(int timeQuantum = 2) {
        resetProcesses();
        vector<Process> processesCopy = processes;
        queue<Process*> readyQueue;
        vector<Process> finishedProcesses;
        int currentTime = 0;
        int processIndex = 0;
        
        sort(processesCopy.begin(), processesCopy.end(), Process::compareFCFS);
        
        while (!processesCopy.empty() || !readyQueue.empty()) {
            // Add newly arrived processes
            while (processIndex < processesCopy.size() && 
                   processesCopy[processIndex].arrivalTime <= currentTime) {
                readyQueue.push(&processesCopy[processIndex]);
                processIndex++;
            }
            
            if (readyQueue.empty()) {
                if (processIndex < processesCopy.size()) {
                    currentTime = processesCopy[processIndex].arrivalTime;
                }
                continue;
            }
            
            Process* p = readyQueue.front();
            readyQueue.pop();
            
            if (!p->hasStarted) {
                p->responseTime = currentTime - p->arrivalTime;
                p->startTime = currentTime;
                p->hasStarted = true;
            }
            
            int execTime = min(timeQuantum, p->remainingTime);
            currentTime += execTime;
            p->remainingTime -= execTime;
            
            executionOrder.push_back(p->name);
            
            // Add newly arrived processes before re-queueing current process
            while (processIndex < processesCopy.size() && 
                   processesCopy[processIndex].arrivalTime <= currentTime) {
                readyQueue.push(&processesCopy[processIndex]);
                processIndex++;
            }
            
            if (p->remainingTime > 0) {
                readyQueue.push(p);
            } else {
                p->turnaround = currentTime - p->arrivalTime;
                p->delay = p->turnaround - p->processingTime;
                finishedProcesses.push_back(*p);
            }
        }
        
        writeOutput("out_rr.txt", finishedProcesses);
    }
    
    // 5. Priority Scheduling (Non-preemptive)
    void schedulePriority() {
        resetProcesses();
        vector<Process> processesCopy = processes;
        vector<Process> readyQueue;
        vector<Process> finishedProcesses;
        int currentTime = 0;
        
        while (!processesCopy.empty() || !readyQueue.empty()) {
            for (auto it = processesCopy.begin(); it != processesCopy.end();) {
                if (it->arrivalTime <= currentTime) {
                    readyQueue.push_back(*it);
                    it = processesCopy.erase(it);
                } else {
                    ++it;
                }
            }
            
            if (readyQueue.empty()) {
                currentTime = processesCopy.front().arrivalTime;
                continue;
            }
            
            sort(readyQueue.begin(), readyQueue.end(), Process::comparePriority);
            
            Process p = readyQueue.front();
            readyQueue.erase(readyQueue.begin());
            
            p.responseTime = max(0, currentTime - p.arrivalTime);
            int endTime = currentTime + p.processingTime;
            p.turnaround = endTime - p.arrivalTime;
            p.delay = p.turnaround - p.processingTime;
            currentTime = endTime;
            
            executionOrder.push_back(p.name);
            finishedProcesses.push_back(p);
        }
        
        writeOutput("out_priority.txt", finishedProcesses);
    }
    
    // 6. SRTF (Shortest Remaining Time First - Preemptive SJF)
    void scheduleSRTF() {
        resetProcesses();
        vector<Process> processesCopy = processes;
        vector<Process> finishedProcesses;
        int currentTime = 0;
        int completed = 0;
        int n = processesCopy.size();
        
        while (completed < n) {
            int idx = -1;
            int minRemaining = INT_MAX;
            
            // Find process with shortest remaining time
            for (int i = 0; i < n; i++) {
                if (processesCopy[i].arrivalTime <= currentTime && 
                    processesCopy[i].remainingTime > 0) {
                    if (processesCopy[i].remainingTime < minRemaining) {
                        minRemaining = processesCopy[i].remainingTime;
                        idx = i;
                    } else if (processesCopy[i].remainingTime == minRemaining) {
                        if (processesCopy[i].arrivalTime < processesCopy[idx].arrivalTime) {
                            idx = i;
                        }
                    }
                }
            }
            
            if (idx == -1) {
                currentTime++;
                continue;
            }
            
            Process& p = processesCopy[idx];
            
            if (!p.hasStarted) {
                p.responseTime = currentTime - p.arrivalTime;
                p.hasStarted = true;
            }
            
            p.remainingTime--;
            currentTime++;
            executionOrder.push_back(p.name);
            
            if (p.remainingTime == 0) {
                completed++;
                p.turnaround = currentTime - p.arrivalTime;
                p.delay = p.turnaround - p.processingTime;
                finishedProcesses.push_back(p);
            }
        }
        
        writeOutput("out_srtf.txt", finishedProcesses);
    }
    
    void writeOutput(const string& filename, const vector<Process>& finishedProcesses) {
        ofstream file(filename);
        if (!file) {
            cerr << "Error opening file: " << filename << endl;
            exit(1);
        }
        
        for (const auto& p : finishedProcesses) {
            file << p.name << ": (response=" << p.responseTime
                << ", turnaround=" << p.turnaround
                << ", delay=" << p.delay << ")\n";
        }
        
        for (const auto& name : executionOrder)
            file << name;
        file << endl;
        
        file.close();
    }
    
    void runAll(int timeQuantum = 2) {
        cout << "Running FCFS..." << endl;
        scheduleFCFS();
        
        cout << "Running SJF..." << endl;
        scheduleSJF();
        
        cout << "Running LJF..." << endl;
        scheduleLJF();
        
        cout << "Running Round Robin (quantum=" << timeQuantum << ")..." << endl;
        scheduleRoundRobin(timeQuantum);
        
        cout << "Running Priority..." << endl;
        schedulePriority();
        
        cout << "Running SRTF..." << endl;
        scheduleSRTF();
        
        cout << "All scheduling algorithms completed!" << endl;
    }
};

int main() {
    CPUScheduler scheduler;
    scheduler.readInput("in.txt");
    
    // Run all algorithms
    scheduler.runAll(2); // 2 is the time quantum for Round Robin
    
    cout << "All processes are DONE!\n";
    return 0;
}
