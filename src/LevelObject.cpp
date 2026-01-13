#include "LevelObject.hpp"

#include <iostream>

#include "ObjectHelper.hpp"

using std::string;
using std::string_view;

namespace Key {
    const string ID = "1";
    
    const string Color = "19";
    
    const string NewMainColor = "21";
    const string NewSecondaryColor = "22";
    
    const string ZLayer = "24";
    const string ZOrder = "25";
    
    const string MainHSVEnabled = "41";
    
    const string MainHSVString = "43";
    
    const string DisableGlow = "96";
};

const string NEW_WHITE = "1011";

enum Color {
    None  = 0,
    P1    = 1,
    P2    = 2,
    C1    = 3,
    C2    = 4,
    LBG   = 5,
    C3    = 6,
    C4    = 7,
    DLine = 8,
    White = 9
};


std::ostream& operator<<(std::ostream &os, const LevelObject &obj) {
    
    for (auto kv = obj.inner.begin(); kv != obj.inner.end(); kv++) {
        if (kv != obj.inner.begin()) {
            os << ',';
        }
        os << kv->first << ',' << kv->second;
    }
    os << ';';
    return os;
}


Color get_effective_color(LevelObject const& obj) {
    const auto inner = obj.inner;
    // if we've already fixed white then Key::Color is emptied
    if (
        (inner.contains(Key::NewSecondaryColor) && inner.at(Key::NewSecondaryColor) == NEW_WHITE)
        || (inner.contains(Key::NewMainColor) && inner.at(Key::NewMainColor) == NEW_WHITE)
    ) {
        return Color::White;
    }
    
    if (inner.contains(Key::Color)) {
        try {
            auto color = std::stoi(inner.at(Key::Color));
            if (1 <= color && color <= 9) {
                return static_cast<Color>(color);
            }
            else if (color != 0) {
                // all colors other than 1-8 are white
                return Color::White;
            }
        }
        catch (std::invalid_argument const&) {}
        catch (std::out_of_range const&) {}
    }
    auto id = inner.at(Key::ID);
    auto helper = obj_helper::get_shared_helper();
    if (helper && helper->m_obj_data.contains(id)) {
        return static_cast<Color>(helper->m_obj_data.at(id).default_color);
    }
    return Color::None;
}


bool LevelObject::fix_layers(obj_helper::ObjectHelper const* helper) {
    auto id = inner.at(Key::ID);
    if (!helper || !helper->m_obj_data.contains(id)) {
        return false;
    }
    
    auto obj_data = helper->m_obj_data.at(id);
    Color base_color;
    if (obj_data.has_color_child) {
        base_color = Color::None;
    }
    else if (obj_data.has_color_child) {
        base_color = static_cast<Color>(obj_data.default_color);
    }
    else {
        base_color = get_effective_color(*this);
    }
    
    bool bottom;
    switch (base_color) {
    case Color::P1: [[fallthrough]];
    case Color::P2: [[fallthrough]];
    case Color::LBG:
        // blending objects don't check for force_bottom on initialisation,
        // and p1/p2/lbg never run the update code that does
        bottom = obj_data.z < 0;
        break;
    default:
        bottom = obj_data.force_bottom || obj_data.z < 0;
        break;
    }
    inner.insert_or_assign(
        Key::ZOrder,
        std::to_string(obj_data.z)
    );
    inner.insert_or_assign(
        Key::ZLayer,
        bottom ? "2":"4"
    );
    return true;
}


bool LevelObject::fix_white(obj_helper::ObjectHelper const* helper) {
    if (!helper) {
        return false;
    }
    auto is_white = (get_effective_color(*this) == Color::White);
    if (is_white) {
        inner.erase(Key::Color);
        auto id = inner.at(Key::ID);
        if (helper->m_obj_data.contains(id) && helper->m_obj_data.at(id).secondary) {
            inner.insert_or_assign(Key::NewSecondaryColor, NEW_WHITE);
        }
        else {
            inner.insert_or_assign(Key::NewMainColor, NEW_WHITE);
        }
    }
    return is_white;
}


bool LevelObject::fix_wavy_blocks() {
    int id;
    try {
        id = std::stoi(inner.at(Key::ID));
    }
    catch (std::invalid_argument const&) {
        return false;
    }
    catch (std::out_of_range const&) {
        return false;
    }
    switch (id) {
    case 62: [[fallthrough]];
    case 63: [[fallthrough]];
    case 64: [[fallthrough]];
    case 65: [[fallthrough]];
    case 66: [[fallthrough]];
    case 68: [[fallthrough]];
    case 294: [[fallthrough]];
    case 295: [[fallthrough]];
    case 296: [[fallthrough]];
    case 297:
        // slightly increasing value so pure black is never reached
        inner.insert_or_assign(Key::MainHSVEnabled, "1");
        inner.insert_or_assign(Key::MainHSVString, "0a1a0.02a0a1");
        return true;
    default:
        return false;
    }
}


bool LevelObject::unfix_uncolored_3d() {
    int id;
    try {
        id = std::stoi(inner.at(Key::ID));
    }
    catch (std::invalid_argument const&) {
        return false;
    }
    catch (std::out_of_range const&) {
        return false;
    }
    // all of the uncolored 3d is contiguous which is very convenient
    if (560 <= id && id <= 577) {
        inner.insert_or_assign(Key::NewMainColor, NEW_WHITE);
        return true;
    }
    return false;
}


bool LevelObject::unfix_glow() {
    int id;
    try {
        id = std::stoi(inner.at(Key::ID));
    }
    catch (std::invalid_argument const&) {
        return false;
    }
    catch (std::out_of_range const&) {
        return false;
    }
    switch (id) {
    case 36: [[fallthrough]];
    case 63: [[fallthrough]];
    case 64: [[fallthrough]];
    case 67: [[fallthrough]];
    case 84: [[fallthrough]];
    case 140: [[fallthrough]];
    case 141:
        // slightly increasing value so pure black is never reached
        inner.insert_or_assign(Key::DisableGlow, "1");
        return true;
    default:
        return false;
    }
}
