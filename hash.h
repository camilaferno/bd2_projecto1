#ifndef HASH_H
#define HASH_H

#include "bucket.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

class Hash{
private:
    int num_blocks;
    int fd;
    vector<Bucket> hash_table;

public:
    Hash(int N, int fd_input, string filename){
        num_blocks = N;
        fd = fd_input;

        ifstream input_file;
        input_file.open(filename);

        //initialize bucket objects
        for(int i=0; i<num_blocks; i++){
            Bucket tmp;
            tmp.bucket_id = i;
            tmp.bucket_name = to_string(i).append(".txt");

            hash_table.push_back(tmp);
        }

        string tmp;
        stringstream line;
        if(input_file.is_open()){
            while(getline(input_file,tmp)){
                int bucket = hash_function(stoi(tmp.substr(0, tmp.find(","))));
                string name_of_txt = to_string(bucket).append(".txt");

                ofstream output_file;
                output_file.open(name_of_txt, ios::app);
                if(output_file.is_open()){
                    output_file << tmp << endl;
                }
                output_file.close();

            }
        }
        input_file.close();

    }

    int hash_function(int key){
        return key % num_blocks;
    }

    void insert(){

    }

    void print(){
        for(auto it = hash_table.begin(); it != hash_table.end(); it++){;
            cout << (*it).bucket_id << " " << (*it).bucket_name << endl;
        }
    }
};

#endif
