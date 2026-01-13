#pragma once

#include <Geode/Geode.hpp>

using std::string;

namespace obj_helper {
    
    struct ObjData {
        uint8_t default_color;
        int8_t z;
        bool has_child;
        bool has_color_child;
        bool force_bottom;
        bool secondary;
    };


    class ObjectHelper {
    protected:

    public:
        std::unordered_map<string, ObjData> m_obj_data;
        
        ObjectHelper() {
            // i don't care if this crashes right now
            auto json = geode::utils::file::readJson(geode::Mod::get()->getResourcesDir() / "obj_data.json").unwrap()
                .asArray().unwrap();
            
            for (auto item = json.begin(); item != json.end(); item++) {
                auto data = (*item)["data"];
                
                ObjData obj_data = {
                    (uint8_t)(data["default_color"].asInt().unwrapOr(0)),
                    (int8_t)(data["z"].asInt().unwrapOr(0)),
                    data["has_child"].asBool().unwrapOr(false),
                    data["has_color_child"].asBool().unwrapOr(false),
                    data["force_bottom"].asBool().unwrapOr(false),
                    data["secondary"].asBool().unwrapOr(false)
                };
                
                auto ids = (*item)["ids"].asArray().unwrap();
                for (auto id = ids.begin(); id != ids.end(); id++) {
                    m_obj_data.insert_or_assign((*id).asString().unwrap(), obj_data);
                }
            }
        }
    };
    
    ObjectHelper const* get_shared_helper();
}