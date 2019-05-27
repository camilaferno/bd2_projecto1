#include <iostream>
#include "hash.h"
#include <thread>
#include <mutex>
mutex constructor_first;

using namespace std;

int main(int argc, char const *argv[]) {
    Hash* myhash;

    bool hash_exists;

    ifstream infile("hash.txt");
    hash_exists = infile.good();

    if(!hash_exists){
        string file_name;
        cout << endl <<  "Enter csv file name: ";
        cin >> file_name;

        cout << endl <<  "Enter the number of buckets: ";
        int num_buckets;
        cin >> num_buckets;

        cout << endl <<  "Enter number of registers per bucket: ";
        int fd;
        cin >> fd;

        myhash = new Hash(num_buckets, fd, file_name);
    }
    else{
        cout << "Opening hash.txt...";
        myhash = new Hash("hash.txt");
    }

    bool run_program = true;
    while(run_program){
        bool run_parallel=true;

        cout << endl << "Do you want to run it in parallel?(0/1): ";
        cin >> run_parallel;

        if(run_parallel){
            
            bool instruct_seq=true;
            vector<string> insert_inputs;
            vector<int> search_inputs;
            while(instruct_seq){
                int instruct_type;

                cout << "Type 1 to add insert, type 2 to add searches, type 3 to deploy threads, 0 to quit ";
                cin >> instruct_type;

                if(instruct_type == 1){
                    string row;
                    cout << endl << "Enter register to insert: ";
                    cin >> row;
                    cout << endl;

                    insert_inputs.push_back(row);
                }
                else if(instruct_type == 2){
                    int key;
                    cout << endl << "Enter id to search: ";
                    cin >> key;
                    cout << endl;

                    search_inputs.push_back(key);
                }
                else if( instruct_type == 3){
                    cout << insert_inputs.size() << endl;
                    thread threads[insert_inputs.size()];
                    for(int i=0; i<insert_inputs.size(); i++){
                        threads[i] = thread(&Hash::insert, myhash, insert_inputs[i]);
                    }
                    thread threads2[search_inputs.size()];
                    for(int i=0; i<search_inputs.size(); i++){
                        threads2[i] = thread(&Hash::search, myhash, search_inputs[i]);
                    }
                    for(int i=0; i<insert_inputs.size(); i++){
                        threads[i].join();
                    }
                    for(int i=0; i<search_inputs.size(); i++){
                        threads2[i].join();
                    }
                    instruct_seq = false;
                    run_program = false;
                }
                else if(instruct_type == 0){
                    instruct_seq = false;
                }
            }
            
        }
        else{
            bool instruct_seq = true;

            while(instruct_seq){
                int instruct_type;
                cout << "Type 1 to insert, type 2 to search, type 3 to remove hash, 0 to quit ";
                cin >> instruct_type;

                if(instruct_type == 1){
                    string row;
                    cout << endl << "Enter register to insert: ";
                    cin >> row;
                    cout << endl;

                    myhash -> insert(row);
                    cout << "inserted" << endl;
                }
                else if(instruct_type == 2){
                    int key;
                    cout << endl << "Enter id to search: ";
                    cin >> key;
                    cout << endl;

                    cout << myhash -> search(key) << endl;
                }
                else if(instruct_type == 3){
                    myhash -> removeHash();
                    instruct_seq = false;
                    run_program = false;
                }
                else if(instruct_type == 0){
                    instruct_seq = false;
                }
            }
            
            if(run_program){
                cout << endl << "Do you want keep going?(0/1): ";
                cin >> run_program;
                cout << "run_program: " << run_program << endl;
            }
        }
    }

    
    return 0;
}
