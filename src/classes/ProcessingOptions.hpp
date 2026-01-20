#pragma once

enum class ProcessingOption {
    FixLayers = 0,
    FixWhite,
    FixWavyBlocks,
    UnfixGlow,
    UnfixUncolored3D,
};

struct ProcessingOptions {
    bool fix_layers;
    bool fix_white;
    bool fix_wavy_blocks;
    bool unfix_uncolored_3d;
    bool unfix_glow;
    
    bool operator==(ProcessingOptions const& other) const {
        return (
            fix_layers == other.fix_layers
            && fix_white == other.fix_white
            && fix_wavy_blocks == other.fix_wavy_blocks
            && unfix_uncolored_3d == other.unfix_uncolored_3d
            && unfix_glow == other.unfix_glow
        );
    }
};

static const ProcessingOptions ALL_OPTIONS_OFF = { false, false, false, false, false };
