#ifndef HASH_H
#define HASH_H

#include "bucket.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

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

        //initialize bucket objects
        for(int i=0; i<num_blocks; i++){
            Bucket tmp;
            tmp.bucket_id = i;
            tmp.bucket_name = to_string(i).append(".txt");

            hash_table.push_back(tmp);
        }

        ifstream input_file;
        input_file.open(filename);

        string tmp;
        //stringstream line;
        if(input_file.is_open()){
            while(getline(input_file,tmp)){
                insert(tmp);
            }
        }
        input_file.close();

    }

    int hash_function(int key){
        return key % num_blocks;
    }

    bool bucket_isfull(int bucket){
        if(hash_table[bucket].counter < fd){
            return false;
        }
        else{ return true;}
    }

    void insert(string input){
        int bucket = hash_function(stoi(input.substr(0, input.find(","))));

        while(bucket_isfull(bucket)){
            if(!hash_table[bucket].overflow_bucket.empty()){
                bucket = hash_table[bucket].overflow_bucket_id;
            }
            else{
                int size = hash_table.size();
                Bucket tmp;
                tmp.bucket_id = size;
                tmp.bucket_name = to_string(size).append(".txt");
                hash_table.push_back(tmp);

                hash_table[bucket].overflow_bucket_id = tmp.bucket_id;
                hash_table[bucket].overflow_bucket = tmp.bucket_name;

                bucket = tmp.bucket_id;
            }
        }
        string name_of_txt = to_string(bucket).append(".txt");

        ofstream output_file;
        output_file.open(name_of_txt, ios::app);

        if(output_file.is_open()){
            output_file << input << endl;
            hash_table[bucket].counter += 1;
        }

        output_file.close();
    }

    string search(int key){
        int bucket = hash_function(key);

        while(bucket_isfull(bucket)){
            bucket = hash_table[bucket].overflow_bucket_id;
        }

        string name_of_txt = to_string(bucket).append(".txt");
        ifstream search_file;
        search_file.open(name_of_txt);

        string tmp;
        stringstream line;
        if(search_file.is_open()){
            while(getline(search_file,tmp)){
                if(tmp.find(to_string(key)) != string::npos){
                    return tmp;
                }
            }
        }
        search_file.close();

        return "not found";
    }

    void print(){
        for(auto it = hash_table.begin(); it != hash_table.end(); it++){;
            cout << (*it).bucket_id << " " << (*it).bucket_name << " " << (*it).overflow_bucket_id << " " << (*it).overflow_bucket << endl;
        }
    }
};

#endif
