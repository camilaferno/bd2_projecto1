

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Bucket{
public:
    int bucket_id;
    string bucket_name;
    int overflow_bucket_id;
    string overflow_bucket = "";
    int counter = 0;

    Bucket(){};
    ~Bucket(){};
};
