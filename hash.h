#ifndef HASH_H
#define HASH_H

#include "bucket.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <thread>
#include <mutex>

using namespace std;

class Hash{
private:
    int HashTable_size;
    int fd;
    int num_buckets = 0;
    mutex m;
    Bucket* hash_table;

public:
    Hash(int N, int fd_input, string filename){
        HashTable_size = N;
        hash_table = new Bucket[N];
        fd = fd_input;

        //initialize bucket objects
        for(int i=0; i<HashTable_size; i++){
            Bucket tmp;
            tmp.bucket_id = i;
            tmp.bucket_name = to_string(i).append(".txt");

            hash_table[i] = tmp;
        }
        num_buckets = HashTable_size;

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
        return key % HashTable_size;
    }

    bool bucket_isfull(Bucket* bucket){
        if(bucket->counter < fd){
            return false;
        }
        else{ return true;}
    }

    void insert(string input){
        Bucket* bucket = &(hash_table[hash_function(stoi(input.substr(0, input.find(","))))]);
        m.lock();
        while(bucket->overflow_bucket){
            bucket = bucket->overflow_bucket;
        }

        if(bucket_isfull(bucket)){
            //Create new bucket
            Bucket* tmp = new Bucket();
            tmp->bucket_id = num_buckets;
            tmp->bucket_name = to_string(num_buckets).append(".txt");

            bucket->overflow_bucket = tmp;
            num_buckets++;

            bucket = tmp;
        }

        string name_of_txt = bucket->bucket_name;

        ofstream output_file;
        output_file.open(name_of_txt, ios::app);

        if(output_file.is_open()){
            output_file << input << endl;
            bucket->counter += 1;
        }

        output_file.close();
        m.unlock();
    }


    string search(int key){
        Bucket* bucket = &(hash_table[hash_function(key)]);



        while(true){
            cout << bucket->bucket_name << endl;
            string name_of_txt = bucket->bucket_name;
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
            if(bucket->overflow_bucket == nullptr)
                return "not found";
            else
                bucket = bucket->overflow_bucket;
        }
    }


    void print(){
        for(int i = 0; i < HashTable_size; i++){;
            cout << (hash_table[i]).bucket_id << " " << (hash_table[i]).bucket_name;
            Bucket *tmp = (hash_table[i]).overflow_bucket;
            while(tmp){
                cout << " -> " << tmp->bucket_id << " " << tmp->bucket_name;
                tmp = tmp->overflow_bucket;
            }
            cout << endl;
        }
    }
};

#endif
