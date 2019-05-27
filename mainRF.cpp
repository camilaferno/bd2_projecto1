#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <map>
#include <string>
#include <vector>
#include "randomfile.h"

using namespace std;
 
int main(){
    vector<int> columnTypes;
    columnTypes.push_back(0); //int
    columnTypes.push_back(2); //string
    columnTypes.push_back(0); //int
    columnTypes.push_back(0); //int
    columnTypes.push_back(0); //int
    columnTypes.push_back(0); //int
    columnTypes.push_back(0); //int
    columnTypes.push_back(0); //int

    
    RandomFile* rf = new RandomFile("pokemon.csv", columnTypes);
    /*
    thread one(&RandomFile::insert, rf, "723,heider,9,16,855,239,12,1");
    thread two(&RandomFile::insert, rf, "724,heider,9,16,855,239,12,1");
    one.join();
    two.join();
    */

   auto start_time = chrono::high_resolution_clock::now();
    rf -> insert("723,heider,9,16,855,239,12,1");
    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(end_time - start_time);
    cout << "inserted in " << time_span.count() << " s\n";

    auto start_time2 = chrono::high_resolution_clock::now();
    rf -> search(25);
    auto end_time2 = chrono::high_resolution_clock::now();
    chrono::duration<double> time_span2 = chrono::duration_cast<chrono::duration<double>>(end_time2 - start_time2);
    cout << "searched in " << time_span2.count() << " s\n";

    return 0;
}