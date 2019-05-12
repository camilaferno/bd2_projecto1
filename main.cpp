#include <iostream>
#include "hash.h"

using namespace std;

int main(int argc, char const *argv[]) {

    Hash* myhash = new Hash(5,80,"pokemon.csv");
    myhash->print();
    myhash->insert("10158,bulbasaur,1,7,69,64,1,1");
    return 0;
}
