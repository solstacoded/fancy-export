#include "LevelObject.hpp"

#include <iostream>

using std::string;
using std::string_view;

std::ostream& operator<<(std::ostream &os, const LevelObject &obj) {
    
    for ( auto kv = obj.inner.begin(); kv != obj.inner.end(); kv++ ) {
        if (kv != obj.inner.begin()) {
            os << ',';
        }
        os << kv->first << ',' << kv->second;
    }
    os << ';';
    return os;
}
