#include <iostream>
#include "hash.h"

using namespace std;

int main(int argc, char const *argv[]) {

    Hash* myhash = new Hash("index.txt");
    //myhash->print();
    myhash->insert("10158,bulbasaur,1,7,69,64,1,1");
    //cout << myhash->search(25) << endl;

    return 0;
}
