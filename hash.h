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
    int HashTable_size = 0;
    int fd;
    int num_buckets = 0;
    mutex m;
    Bucket* hash_table;

public:
    Hash(int N, int fd_input, string filename){
        if(!is_file_exist("index.txt")){
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
                    insert_hash(tmp);
                }
            }
            input_file.close();

            //Creation of index file
            ofstream index_file;
            index_file.open("index.txt");

            if(index_file.is_open()){
                index_file << to_string(num_buckets) << endl;
                index_file << to_string(fd) << endl;
                for(int i = 0; i < HashTable_size; i++){
                    index_file << (hash_table[i]).bucket_name;
                    Bucket *tmp = (hash_table[i]).overflow_bucket;
                    while(tmp){
                        index_file << "," << tmp->bucket_name;
                        tmp = tmp->overflow_bucket;
                    }
                    index_file << endl;
                }
            }

            index_file.close();
        }

    }

    Hash(string index){
        ifstream index_file;
        index_file.open(index);
        if(index_file.is_open()){
            string tmp;
            getline(index_file, tmp);
            num_buckets = stoi(tmp);
            getline(index_file, tmp);
            fd = stoi(tmp);
            while (getline(index_file, tmp)){
                HashTable_size++;
            }
        }
    }

    ~Hash(){
        string tmp;
        vector <string> tmp_vec;

        ifstream old_index;
        old_index.open("index.txt");
        if(old_index.is_open()){
            getline(old_index,tmp);
            while(getline(old_index, tmp)){
                tmp_vec.push_back(tmp);
            }
        }  
        old_index.close();
        
        ofstream new_index;
        new_index.open("index.txt");
        if(new_index.is_open()){
            new_index << to_string(num_buckets) << endl;
            for(auto& it: tmp_vec){
                new_index << it << endl;
            }
        }

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

    void insert_hash(string input){
        Bucket* bucket = &(hash_table[hash_function(stoi(input.substr(0, input.find(","))))]);
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
    }

    void insert(string input){
        int bucket_id = hash_function(stoi(input.substr(0, input.find(","))));
        m.lock();

        ifstream input_file;
        input_file.open("index.txt");
        string tmp;
        string last_bucket;
        if(input_file.is_open()){
            for(int i=0; i<=bucket_id; i++){
                getline(input_file,tmp);
            }
            size_t found = tmp.find_last_of(",");

            last_bucket = tmp.substr(found+1);
        }
        input_file.close();

        ifstream bucket_file;
        int bucket_counter = 0;
        bucket_file.open(last_bucket);
        if(bucket_file.is_open()){
            cout << "Im open" << endl;
            while(getline(bucket_file,tmp))
                bucket_counter++;
            if (bucket_counter < fd){
                //insertar aca
                bucket_file.close();
                ofstream newb;
                newb.open(last_bucket, ios::app);
                if(newb.is_open()){
                    newb << input << endl;
                    newb.close();
                    cout << input << "im open again";
                }
                cout << "im closed again";
            }
            else{
                //crear nuevi bucket
                ofstream new_bucket;
                new_bucket.open(to_string(num_buckets).append(".txt"));
                new_bucket << input << endl;
                num_buckets++;
                new_bucket.close();
            }
        }
        else{cout << "im closed";}


        /*
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
        */
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

    bool is_file_exist(string fileName){
        ifstream infile(fileName);
        return infile.good();
    }


    void print(){
        for(int i = 0; i < HashTable_size; i++){
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
