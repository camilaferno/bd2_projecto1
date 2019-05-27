#ifndef RANDOM_H
#define RANDOM_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <mutex>
#include <thread>

using namespace std;

class RandomFile{
private:
	map<int, int> indexMap;
	string csv;
	string binfile;
	vector<int> columnTypes;
	mutex muInsert;

public:
	RandomFile(string filename, vector<int> cTypes){
		csv = filename;
		columnTypes = cTypes;
		if(!is_file_exist("data.dat"))
			makeBinaryFile(csv, cTypes);
		binfile = "data.dat";
		
		if(!is_file_exist("index.txt")){
			ifstream data;
			data.open(binfile, ios::binary | ios::in);
			if(data.is_open()){
				ofstream index_file;
				index_file.open("index.txt");
				
				if(index_file.is_open()){
					//Size of file
					data.seekg(0, ios::end);
					int SizeOfFile = data.tellg();
					data.seekg(0,ios::beg);

					int nextlinebeg = 0;
					
					while(data.tellg() < SizeOfFile){

						for(int i=0; i < columnTypes.size(); i++){

							switch (columnTypes[i]){
								case 0:{//int
									data.seekg(nextlinebeg);
									int num;
									data.read((char*)&num, sizeof(int));
									if(i == 0){index_file << num << "," << nextlinebeg << endl;} //if id
									nextlinebeg += 4;
									break;}
								case 1:{//float
									data.seekg(nextlinebeg);
									float numf;
									data.read((char*)&numf, sizeof(float));
									if(i == 0){index_file << numf << "," << nextlinebeg << endl;} //if id
									nextlinebeg += 4;
									break;}
								case 2:{//string
									string id;
									//Read length string
									data.seekg(nextlinebeg);
									vector<char> stringSize;
									stringSize.resize(1);
									data.read(stringSize.data(), 1);
									nextlinebeg += 8;

									//save string
									data.seekg(nextlinebeg);
									vector<char> str;
									str.resize((int)stringSize[0]);
									data.read(str.data(), (int)stringSize[0]);
									for(int i=0; i < (int)stringSize[0]; i++){
										char C = static_cast<char>((int)str[i]);
										id += C;
									}
									nextlinebeg += (int)stringSize[0];
									if(i == 0){index_file << id << "," << nextlinebeg << endl;} //if id
									break;}
								default:
									break;
							}
						}
					}
					
				}
				index_file.close();
				
				buildMap();
			}
			else{cout << "not open" << endl;}
			data.close();

		}
		else{
			buildMap();

		}
		
	}

	void buildMap(){
		ifstream index_file;
		index_file.open("index.txt");
		if(index_file.is_open()){
			string line;
			while(getline(index_file, line)){
				pair<int, int> p;
				p.first = stoi(line.substr(0, line.find(",")));
				p.second = stoi(line.substr(line.find(",") + 1));
				indexMap.insert(p);
			}
		}
		index_file.close();
	}

	void makeBinaryFile(string filename, vector<int> columnType){
		ofstream outFile;
		outFile.open("data.dat", ios::binary | ios::out);
		ifstream inputFile;
		inputFile.open(filename);
		
		string line;
		while(getline(inputFile, line)){
			stringstream ss(line);
			for(auto it = columnType.begin(); it != columnType.end(); ++it){
				string word;
				if (word[word.size() - 1] == '\r') word.erase(word.size() - 1);
                getline(ss, word, ',');
				switch (*it){
					case 0:{//int
						int integer = stoi(word);
						outFile.write((char*)&integer, sizeof(int));
						break;}
					case 1:{//float
						float number = stof(word);
						outFile.write((char*)&number, sizeof(float));
						break;}
					case 2:{//string
						size_t size = word.size();
						outFile.write((char*)&size, sizeof(size));
						outFile.write(&word[0], size);
						break;}
					default:
						break;
				}
			}
		}
		inputFile.close();
		outFile.close();
	}

	void insert(string input){
		muInsert.lock();

		ofstream data;
		data.open(binfile, ios::binary | ios::app | ios::out);
		int beginningpos = data.tellp();

		if(data.is_open()){
			stringstream ss(input);
			for(auto it = columnTypes.begin(); it != columnTypes.end(); ++it){
				string word;
				if (word[word.size() - 1] == '\r') word.erase(word.size() - 1);
                getline(ss, word, ',');
				switch (*it){
					case 0:{//int
						int integer = stoi(word);
						data.write((char*)&integer, sizeof(int));
						break;}
					case 1:{//float
						float number = stof(word);
						data.write((char*)&number, sizeof(float));
						break;}
					case 2:{//string
						size_t size = word.size();
						data.write((char*)&size, sizeof(size));
						data.write(&word[0], size);
						break;}
					default:
						break;
				}
			}
		}
		data.close();

		//Insert to Index
		pair<int, int> p;
		p.first = stoi(input.substr(0, input.find(",")));
		p.second = beginningpos;
		indexMap.insert(p);

		ofstream index_file;
		index_file.open("index.txt", ios::app);
		if(index_file.is_open()){
			index_file << p.first << "," << p.second << endl;
		}
		index_file.close();

		muInsert.unlock();
	}

	string search(int key){
		string result;
		int position = indexMap.at(key);
		ifstream data;
		data.open(binfile);

		data.seekg(position);

		for(auto it = columnTypes.begin(); it != columnTypes.end(); ++it){
			switch (*it){
				case 0:{//int
					int num;
					data.read((char*)&num, sizeof(int));
					position += 4;
					result.append(" "+to_string(num));
					data.seekg(position);
					break;}
				case 1:{//float
					int numf;
					data.read((char*)&numf, sizeof(int));
					position = position+4;
					result.append(" "+to_string(numf));
					data.seekg(position);
					break;}
				case 2:{//string
					//Read length string
					data.seekg(position);
					vector<char> stringSize;
					stringSize.resize(1);
					data.read(stringSize.data(), 1);
					position = position + 8;

					//save string
					data.seekg(position);
					vector<char> str;
					str.resize((int)stringSize[0]);
					data.read(str.data(), (int)stringSize[0]);
					result.append(" ");
					for(int i=0; i < (int)stringSize[0]; i++){
						char C = static_cast<char>((int)str[i]);
						result += C;
					}
					position = position + (int)stringSize[0];
					break;}
				default:
					break;
			}
		}
		data.close();
		return result;
	}

	bool is_file_exist(string filename){
		ifstream infile(filename);
		return infile.good();
	}

	void removeRandomFile(){
        bool u_sure = false;
        
        cout << "This will remove all txt files so that a new Random File can be created. Are you sure? (1/0) ";
        cin >> u_sure;
        if(u_sure){
            system("rm index.txt");
			system("rm data.dat");
        }
        else{ cout << "not removed" << endl;}
    }

	~RandomFile(){}

};

#endif
