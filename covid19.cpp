/*Author - Percy Brown
*Date - 15th May 2020
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <time.h>
#include <random>

using namespace std;

/* PHASE 1: *Reads the contents of two files and prints a graph in the form of an adjacency list*/

/*Global variables to keep track of total records at the end of 100 days*/
int pop_size = 0;
int total_infected = 0;
int total_sick = 0;
int total_recoveries = 0;

/*
*Persons in the population are stored as nodes in struct form.
*Contains their details, a vector that points to their edges and the possible states they can be found in during simulation
*/

struct Node{
    string name;
    int age;
    float spread_prob;
    vector<Node*> edges;
    float disease_prob;
    int number_of_days;
    bool is_infected;
    bool is_dead;
    bool is_sick;
    bool is_recovered;
};

/*Sets the disease probability for very node: age / maximum age*/
void setDP(Node& x, int age){
    x.disease_prob = (float) x.age/age;
}

/*
*Reads the population file
*Creates a struct for each node
*Stores them in a vector
*Also keeps track of the maximum age and sets the disease probability for every node.
*/
vector<Node> insertNode(string fileName){
    vector<Node> patients;
    int largest_age = 0;
    ifstream read(fileName);               //reads the provided filename from user
    if(!read){                                      //file not found
    cerr << "Sorry, file not in directory." << endl;
    }
    while(!read.eof()){                          //file found
        string n; string c; string prob;
        while(read >> n >> c >> prob){               //read each line and store name, age and probability
            int x = stoi(c);
            if(x > largest_age){largest_age = x;}
            float y = stof(prob);
            Node a{n, x, y};
            a.number_of_days = 0;
            a.is_infected = false;
            a.is_dead = false;
            a.is_sick = false;
            a.is_recovered = false;
            patients.push_back(a);
            pop_size++;
        }
        for(int i = 0; i < patients.size(); i++){             //set disease probability
            setDP(patients[i], largest_age);
        }
    }
    return patients;
}

/*
*Reference to the vector of nodes as a parameter.
*Reads from the connections file
*Appends each edge of a node to its vector in the struct.
*/

void insertEdge(vector<Node>& x, string file){
    ifstream read(file);
    if(!read){                           //file not found.
    cerr << "Sorry, file not in directory." << endl;
    }
    while(!read.eof()){                  //file found
        string a1; string a2;
        while(read >> a1 >> a2){          //read both names on each line and store
            for(int i = 0; i < x.size(); i++){
                if(a1 == x[i].name){            //if first name in vector of nodes
                   for(int j = 0; j < x.size(); j++){       //add neighbour
                       if(a2 == x[j].name){
                           x[i].edges.push_back(&x[j]);
                       }
                   }
                }
            }
            for(int k = 0; k < x.size(); k++){           // vice versa
                if(a2 == x[k].name){
                    for(int l = 0; l < x.size(); l++){
                        if(a1 == x[l].name){
                            x[k].edges.push_back(&x[l]);
                        }
                    }
                }
            }
        }
    }
}

/*
*Reference to the vector of nodes as a parameter.
Prints the population; each node and their respective edges (neighbours).
*/
void printGraph(vector<Node>& everyone){
    for(int j = 0; j < everyone.size() ; j++){
        cout << everyone[j].name << " - > ";
        for(int i = 0; i < everyone[j].edges.size(); i++){
            cout << " |" << everyone[j].edges[i]->name << "|" ;
        }
        cout <<endl;
    }

}

/* PHASE 2: Randomly infects a node, simulates for 100 days and prints summary of population. */

/* Simulates all probabilities in this program */
bool simulate_prob(float prob){
    prob = prob*1000;
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> random_number(1,1000); // distribution in range [1, 1000]
    if ( random_number(rng) <= prob)
        return true;
  return false;
}

/*
*Reference to the vector of nodes as a parameter.
*Randomly infects a node, sets its infection state to true.
*/
void random_infect(vector<Node> &x){
    srand(time(NULL));
    int n = rand() % x.size();
    x[n].is_infected = true;
    total_infected++;
}

/*
*Makes infections...
*Checks whether a node is infected, simulates its spread probability for each neigbour and infects them when true.
*/
void infect(Node& y){
    if((y.is_infected == true) && (y.is_dead == false)){
        for(int i = 0; i < y.edges.size(); i++){
            if((y.edges[i]->is_infected == false) && (y.edges[i]->is_dead == false)){
                if(simulate_prob(y.spread_prob)){
                y.edges[i]->is_infected = true;
                y.edges[i]->is_recovered = false;  /*your recovery should be set to false once you get infected, just to be safe
                                                    especially for those who recently recovered and had their recovery state set to true. */

                total_infected++;                  //increments total cases by 1.
                y.edges[i]->number_of_days = 0;
                }
            }
        }
    }
}

/* Checks for alive and infected but not sick and simulates their disease probability
* Persons become sick when simulation returns true.
* Returns a bool to indicate whether or not a person sick.
*/
bool makeSick(Node& x){
    bool sick = false;
    if((x.is_infected == true) && (x.is_sick == false) && (x.is_dead == false)){
        sick = simulate_prob(x.disease_prob);
        if(sick){
            x.is_sick = true;
            total_sick++;               //increments total sick cases by 1
            x.number_of_days = 1;          //days count begins
        }
    }
    return sick;
}

/* Checks the recovery status for infected and sick patients.
* Checks only for sick patients within 7 days of getting sick.
* They recover by simulating the inverse of their disease probability.
*/
void sick_rec(Node& a){
    if(a.is_sick == true){                //sick and alive patients are definitely infected.
        if(a.number_of_days < 8){
            if(simulate_prob(1 - a.disease_prob)){    //if true - means patient recovers
                    a.is_infected = false;             //no longer infected
                    a.is_sick = false;       //no longer sick
                    a.is_recovered = true;
                    total_recoveries++;       //total recovery counts increase by 1.
                    a.number_of_days = 0;      //patient can get infected again, number of days starts from 0 again.
            }
        }

    }
}

/*
*Checks for sick and alive patients and continuously increments their day count each day.
*Sick patient dies after 7 days of getting sick.
*/
void increaseDays_Death(Node& a){
    if(a.is_sick== true){
        a.number_of_days++;
    }
    if(a.number_of_days > 7){
        a.is_dead = true;        //sick patient dies
        a.is_sick = false;         //no longer sick
        a.is_recovered = false;    //not recovered
        a.is_infected = false;     //no longer active infected case
    }
}

/*
*Randomly infects a node before simulation begins
*Simulates for 100 days, with everyone's state updated each day
*/
void Simulation(vector <Node> &pop, int days){
    random_infect(pop);
    for(int i = 0; i < days; i++){
       for(int j = 0; j < pop.size(); j++){
           infect(pop[j]);
           makeSick(pop[j]);
           sick_rec(pop[j]);
           increaseDays_Death(pop[j]);
       }
   }
}

/*Prints summary of population such as stats on total cases, current infections, total deaths, total sick and total recoveries.*/
void Summary(vector <Node> &ch){
    int n_i = 0; int un = 0;  int s = 0; int d = 0; int i_ns = 0; int r = 0;
    for(int i = 0; i < ch.size(); i++){
        if(ch[i].is_infected == false && ch[i].is_dead == false){
            un++;
        }
        else if(ch[i].is_infected == true){
            n_i++;
        }
        else if(ch[i].is_infected == true && ch[i].is_sick == false){
            i_ns++;
        }
        else if(ch[i].is_sick == true){
            s++;
        }
        else if(ch[i].is_dead == true){
            d++;
        }
    }
    cout <<endl;
    cout <<"THIS TOO SHALL PASS!" << endl;
    cout <<"C-O-V-I-D---19:  U-P-D-A-T-E-S---T-O-D-A-Y :)" <<endl;
    cout << endl;
    cout <<"There are " << pop_size << " people living in the country." << endl;
    cout << endl;
    cout <<"TODAY: " << endl;
    cout << un << " person(s) are uninfected." << endl;
    cout << n_i << " person(s) are infected." << endl;
    cout << i_ns << " person(s) are infected but not sick." << endl;
    cout << s << " person(s) are sick." << endl;
    cout << endl;
    cout <<"TILL DATE: " <<endl;
    cout <<"A total of " << total_infected << " infected cases have been recorded." << endl;
    cout <<"A total number of " << total_sick << " persons have been recorded sick." << endl;
    cout <<"Total recovery counts: " << total_recoveries <<endl;
    cout <<"Sadly, " << d << " person(s) have died." << endl;
    cout <<endl;
}


/*PHASE 3: Implements a policy to slow the spread of the virus and reduce the number of infections*/

/*Prints Policy details*/
void Lockdown(){
    cout << endl;
    cout << "STAY - AT - HOME! - YOUR FRONT DOOR IS YOUR FRONT LINE IN COVID 19 FIGHT" << endl;
    cout << "In order to slow the spread of the virus, we need a Partial Lockdown and Ban on Social Gatherings!" << endl;
    cout << "Only essential service workers and individuals seeking essential services eg. food, healthcare etc. are allowed to go out" << endl;
    cout << "Hence, we expect people between 18 and 60 years to be out for essential service purposes" << endl;
    cout << "We expect that persons outside 18 - 60 years should stay at home and allow eligible persons to run essential errands for them" << endl;
    cout << "Eligible persons who can go out are expected to avoid social gatherings while observing the safety protocols." << endl;
    cout << "Therefore, the spread probabilities of these eligible persons will reduce by 30%, slowing the spread." << endl;
    cout << "Persons outside the eligible age group are less likely to be infected while staying at home, hence reduction of spread probaility by 50%." << endl;
    cout << endl;
}

/*Resets all states and statistical variables to their intialised states before policy implementation*/
void reset_States(vector<Node> &q){
    for(int i = 0; i < q.size(); i++){
        q[i].is_infected = false;
        q[i].is_sick = false;
        q[i].is_dead = false;
        q[i].is_recovered = false;
        q[i].number_of_days = 0;
    }
    total_infected = 0;
    total_sick = 0;
    total_recoveries = 0;
}

/*
*Partial lockdown and ban on social gatherings
*Reduces the spread probaility for nodes with neighbours by half for eligible persons who go out
*Ineligible persons have their spread probabilities remaining the same
*/
void PartialLockdown(vector<Node> &v){
    float reduce = 0.0;
    for(int i = 0; i < v.size(); i++){
        if(v[i].age > 17 && v[i].age < 61){
            reduce = v[i].spread_prob * 0.3          //30% of spread probaility
            v[i].spread_prob -= reduce;
        }
        else{
            reduce = v[i].spread_prob * 0.5      //50% of spread probaility
            v[i].spread_prob -= reduce
        }
    }

}

/* Simulates again for 100 days after imposing lockdown */
void rerunSimulation(vector<Node> &t, int days){
    reset_States(t);
    PartialLockdown(t);
    Simulation(t, days);
}


