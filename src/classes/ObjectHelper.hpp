#pragma once

#include <Geode/Geode.hpp>

using std::string;

namespace obj_helper {
    struct ObjData {
        uint8_t default_color;
        int8_t z;
        bool has_child;
        bool has_color_child;
        bool color_child_only;
        bool force_bottom;
        bool secondary;
    };

    class ObjectHelper {
    protected:
        bool initObjData();
    public:
        std::unordered_map<string, ObjData> m_obj_data;
        
        ObjectHelper() {
            if (!initObjData()) {
                geode::log::warn("Unable to load object data !");
            }
        }
    };

    ObjectHelper const* getSharedHelper();

}
using obj_helper::ObjectHelper;
