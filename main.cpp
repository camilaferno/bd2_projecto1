#include <iostream>
#include "hash.h"

using namespace std;

int main(int argc, char const *argv[]) {

    Hash* myhash = new Hash(5,10000,"pokemon.csv");
    myhash->print();

    return 0;
}
