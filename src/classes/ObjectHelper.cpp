#include "ObjectHelper.hpp"

#include <Geode/Geode.hpp>

using obj_helper::ObjData;

static ObjectHelper* s_sharedHelper = nullptr;

ObjectHelper const* obj_helper::getSharedHelper() {
    if (!s_sharedHelper) {
        s_sharedHelper = new ObjectHelper();
        geode::log::info("Initialised ObjectHelper");
    }
    return s_sharedHelper;
}

bool ObjectHelper::initObjData() {
    // i don't care if this crashes right now
    auto json = geode::utils::file::readJson(geode::Mod::get()->getResourcesDir() / "obj_data.json");
    if (!json.isOk()) { return false; }
    auto arr = (*json).asArray();
    if (!arr.isOk()) { return false; }
    
    for (auto it = (*arr).begin(); it != (*arr).end(); it++) {
        auto item = *it;
        if (!item.isObject()) { return false; }
        auto data = item["data"];
        if (!data.isObject()) { return false; }
        
        ObjData obj_data = {
            static_cast<uint8_t>(data["default_color"].asInt().unwrapOr(0)),
            static_cast<int8_t>(data["z"].asInt().unwrapOr(0)),
            data["has_child"].asBool().unwrapOr(false),
            data["has_color_child"].asBool().unwrapOr(false),
            data["color_child_only"].asBool().unwrapOr(false),
            data["force_bottom"].asBool().unwrapOr(false),
            data["secondary"].asBool().unwrapOr(false)
        };
        
        auto ids = item["ids"].asArray();
        if (!ids.isOk()) { return false; }
        auto ids_arr = *ids;
        for (auto it = ids_arr.begin(); it != ids_arr.end(); it++) {
            auto id = (*it).asString();
            if (!id.isOk()) { return false; }
            m_obj_data.insert_or_assign(*id, obj_data);
        }
    }
    return true;
}
