

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Bucket{
public:
    int bucket_id;
    string bucket_name;
    Bucket* overflow_bucket = nullptr;
    int counter = 0;

    Bucket(){};
    ~Bucket(){};
};
