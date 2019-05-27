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
    Bucket* hash_table;
    mutex mu;

    int hash_function(int key){
        return key % HashTable_size;
    }

    bool bucket_isfull(Bucket* bucket){
        if(bucket->counter < fd){
            return false;
        }
        else{ return true;}
    }

    bool is_file_exist(string fileName){
        ifstream infile(fileName);
        return infile.good();
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

public:
    Hash(int N, int fd_input, string filename){
        if(!is_file_exist("hash.txt")){
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

            //first time hash
            string tmp;
            if(input_file.is_open()){
                while(getline(input_file,tmp)){
                    insert_hash(tmp);
                }
            }
            input_file.close();

            //Create hash.txt
            ofstream hash_file;
            hash_file.open("hash.txt");
            if(hash_file.is_open()){
                hash_file << to_string(HashTable_size) << endl;
                hash_file << to_string(num_buckets) << endl;
                hash_file << to_string(fd) << endl;
            }
            hash_file.close();

            //Creation of index files
            for (int i = 0; i < HashTable_size; i++){
                ofstream index_file;
                index_file.open(to_string(i).append("index.txt"));
                if(index_file.is_open()){
                    index_file << (hash_table[i]).bucket_name << endl;
                    Bucket *tmp = (hash_table[i]).overflow_bucket;
                    while(tmp){
                        index_file << tmp->bucket_name << endl;
                        tmp = tmp->overflow_bucket;
                    }
                }
                index_file.close();
            }
        }
        else Hash("hash.txt");
    }

    Hash(string hash){
        ifstream hash_file;
        hash_file.open(hash);
        if(hash_file.is_open()){
            string tmp;
            getline(hash_file, tmp);
            HashTable_size = stoi(tmp);
            getline(hash_file, tmp);
            num_buckets = stoi(tmp);
            getline(hash_file, tmp);
            fd = stoi(tmp);
        }
        hash_file.close();
    }

    void insert(string input){
        int bucket_id = hash_function(stoi(input.substr(0, input.find(","))));
        mu.lock();

        //read until bucket = last overflow bucket
        ifstream bucket_table;
        bucket_table.open(to_string(bucket_id).append("index.txt"));
        string bucket, tmp;
        if(bucket_table.is_open()){
            while(getline(bucket_table, tmp)){
                bucket = tmp;
            }
        }
        bucket_table.close();

        ifstream bucket_file;
        int bucket_counter = 0;
        bucket_file.open(bucket);
        if(bucket_file.is_open()){
            while(getline(bucket_file,tmp))
                bucket_counter++;
            bucket_file.close();
            if (bucket_counter < fd){
                //insertar aca
                ofstream newbucket;
                newbucket.open(bucket, ios::app);
                if(newbucket.is_open()){   
                    newbucket << input << endl;
                }
                newbucket.close();
            }
            else{
                //crear nuevo bucket
                ofstream newbucket;
                newbucket.open(to_string(num_buckets).append(".txt"));
                if(newbucket.is_open()){
                newbucket << input << endl;
                ofstream index;
                index.open(to_string(bucket_id).append("index.txt"), ios::app);
                if(index.is_open())
                index << to_string(num_buckets).append(".txt") << endl;
                num_buckets++;
                index.close();
                }
                newbucket.close();
		        ofstream hash_file;
	            hash_file.open("hash.txt");
                if(hash_file.is_open()){
                    hash_file << to_string(HashTable_size) << endl;
                    hash_file << to_string(num_buckets) << endl;
                    hash_file << to_string(fd) << endl;
                }
		        hash_file.close();
            }
        }
        mu.unlock();
    }


    string search(int key){
        int bucket_id = hash_function(key);
	    ifstream index;
	    index.open(to_string(bucket_id).append("index.txt"));
        if(index.is_open()){
            string bucket;
                while(getline(index, bucket)){
                    ifstream search_file;
                    search_file.open(bucket);

                    if(search_file.is_open()){
                        string tmp;
                        while(getline(search_file,tmp)){
                            if(tmp.find(to_string(key)) != string::npos){
                                    return tmp;
                            }
                        }
                    }
                    search_file.close();
                }
        }
        index.close();
        return "404 Not Found";
    }

    void removeHash(){
        bool u_sure = false;
        
        cout << "This will remove all txt files so that a new Hash can be created. Are you sure? (1/0) ";
        cin >> u_sure;
        if(u_sure){
            system("rm *.txt");
        }
        else{ cout << "not removed" << endl;}
    }

};

#endif