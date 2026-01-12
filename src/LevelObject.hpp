#pragma once

#include <Geode/Geode.hpp>
#include <iostream>

#include "spliterator.hpp"

using std::string;
using std::string_view;

struct LevelObject {
    std::map<string, string> inner;
    
    static LevelObject from_obj_string(string_view const obj_string) {
        std::map<string, string> map;
        Spliterator split(obj_string, ',', false);
        
        auto next_pair = split.next_pair();
        while (next_pair.has_value()) {
            auto pair = *next_pair;
            map.insert_or_assign(string(pair.first), string(pair.second));
            next_pair = split.next_pair();
        }
        return LevelObject { map };
    }
    
    static std::vector<LevelObject> from_level_string(string_view const level_string) {
        std::vector<LevelObject> vec;
    
        auto decompressed = cocos2d::ZipUtils::decompressString(string(level_string), false);
        Spliterator split(decompressed, ';', true);
        auto next = split.next();
        while (next.has_value()) {
            auto obj_string = *next;
            vec.push_back(LevelObject::from_obj_string(obj_string));
            next = split.next();
        }
        return vec;
    }
    
    friend std::ostream& operator<<(std::ostream &os, const LevelObject &obj);
    
    static string to_level_string(std::vector<LevelObject> const& objects) {
        std::ostringstream os;
        for (auto obj = objects.begin(); obj != objects.end(); obj++) {
            os << *obj;
        }
        return cocos2d::ZipUtils::compressString(os.str(), false);
    }
};
