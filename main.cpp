#include <iostream>
#include <string>
#include "covid19.cpp"

using namespace std;

int main(){

    /* The number of days simulation will run for */
    int days_to_simulate = 100;

    /* Ask the user for the population and
    connections files */
    cout << "Welcome to CoronaSim!" << endl;
    string pop_file, conn_file;
    cout << "Enter population file name: ";
    cin >> pop_file;
    cout << "Enter connections file name: ";
    cin >> conn_file;

    // Populating graph data structure
    vector<Node> x = insertNode(pop_file);
    insertEdge(x, conn_file);

    bool is_populated = false;

    if (is_populated)
        cout << "File Loaded!" << endl;


    while(true){

        int choice;

        /* Print menu of things that your application can do*/
        cout << "*** *** *** *** *** *** *** *** *" << endl;
        cout << "What would you like to do?" << endl;
        cout << "0. Print intial population summary" << endl;
        cout << "1. Print population" << endl;
        cout << "2. Simulate" << endl;
        cout << "3. Summarize population" << endl;
        cout << "4. Implement policy and simulate" << endl;
        cout << "5. Summarize population after policy" << endl;
        cout << "6. Exit" << endl;
        cout << endl;
        cout << "*** *** *** *** *** *** *** *** *" << endl;
        cout << "Your selection: ";

        cin >> choice;

        switch(choice){

            case 0:
            /* Prints intial summary before simulation */
            Summary(x);
            break;

            case 1:
                /* Prints graph */
                printGraph(x);     //print population
                break;

            case 2:
                /* Run simulation.*/
                Simulation(x, days_to_simulate);    //simulation
                cout << "Simulation complete!" << endl;
                break;

            case 3:
                /* Print a stats summary of the population .*/
                Summary(x);      //print statistics
                break;

            case 4:
                /* Partial Lockdown policy and resimulation.*/
                Lockdown();                              //policy details
                rerunSimulation(x, days_to_simulate);    //policy implementation & simulation
                cout << "Policy implementation and Simulation complete!" << endl;
                break;

            case 5:
                /* Print summary after policy implementation */
                cout << "Results after policy implementation: " << endl;
                Summary(x);
                break;

            case 6:
                /* de-allocating all dynamically
                allocated variables, if any, before exiting*/
                cout << "ALWAYS VISIT FOR AUTHENTIC UPDATES" << endl;
                return 0; // exiting
        }
    }
}