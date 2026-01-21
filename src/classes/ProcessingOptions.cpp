#include "ProcessingOptions.hpp"

#include <Geode/Geode.hpp>
#include <Geode/Utils.hpp>
#include <Geode/utils/terminate.hpp>

char const* ProcessingOptions::getString(ProcessingOption option) {
    switch (option) {
    case ProcessingOption::FixLayers:
        return "fix_layers";
    case ProcessingOption::FixWhite:
        return "fix_white";
    case ProcessingOption::FixWavyBlocks:
        return "fix_wavy_blocks";
    case ProcessingOption::UnfixGlow:
        return "unfix_glow";
    case ProcessingOption::UnfixUncolored3D:
        return "unfix_uncolored_3d";
    default:
        geode::utils::unreachable("Attempted to get string of unknown processing option");
        return "";
    }
}

char const* ProcessingOptions::getDisplayString(ProcessingOption option) {
    switch (option) {
    case ProcessingOption::FixLayers:
        return "Fix Layers";
    case ProcessingOption::FixWhite:
        return "Fix White Channel";
    case ProcessingOption::FixWavyBlocks:
        return "Fix Wavy Blocks";
    case ProcessingOption::UnfixGlow:
        return "Unfix Glow Sprites";
    case ProcessingOption::UnfixUncolored3D:
        return "Unfix Uncolored 3D";
    default:
        geode::utils::unreachable("Attempted to get string of unknown processing option");
        return "";
    }
}

char const* ProcessingOptions::getFrameName(ProcessingOption option) {
    switch (option) {
    case ProcessingOption::FixLayers:
        return "fix_layers.png"_spr;
    case ProcessingOption::FixWhite:
        return "fix_white.png"_spr;
    case ProcessingOption::FixWavyBlocks:
        return "fix_wavy_blocks.png"_spr;
    case ProcessingOption::UnfixGlow:
        return "unfix_glow.png"_spr;
    case ProcessingOption::UnfixUncolored3D:
        return "unfix_uncolored_3d.png"_spr;
    default:
        geode::utils::unreachable("Attempted to get frame name of unknown processing option");
        return "";
    }
}

bool ProcessingOptions::getOption(ProcessingOption option) {
    switch (option) {
    case ProcessingOption::FixLayers:
        return fix_layers;
    case ProcessingOption::FixWhite:
        return fix_white;
    case ProcessingOption::FixWavyBlocks:
        return fix_wavy_blocks;
    case ProcessingOption::UnfixGlow:
        return unfix_glow;
    case ProcessingOption::UnfixUncolored3D:
        return unfix_uncolored_3d;
    default:
        geode::utils::unreachable("Attempted to get unknown processing option");
        return false;
    }
}

void ProcessingOptions::setOption(ProcessingOption option, bool value) {
    switch (option) {
    case ProcessingOption::FixLayers:
        fix_layers = value;
        break;
    case ProcessingOption::FixWhite:
        fix_white = value;
        break;
    case ProcessingOption::FixWavyBlocks:
        fix_wavy_blocks = value;
        break;
    case ProcessingOption::UnfixGlow:
        unfix_glow = value;
        break;
    case ProcessingOption::UnfixUncolored3D:
        unfix_uncolored_3d = value;
        break;
    default:
        geode::utils::unreachable("Attempted to set unknown processing option");
        return;
    }
}
