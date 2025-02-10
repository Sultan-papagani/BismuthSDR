#include "global_helper.h"
#include <random>

char* generate_random_string(int len) {
    static std::random_device rd;
    static std::mt19937 gen(rd()); 
    static std::uniform_int_distribution<> dis('a', 'z');

    char* str = new char[len + 1];
    for (int i = 0; i < len; ++i) {
        str[i] = dis(gen); 
    }
    str[len] = '\0';
    return str;
}