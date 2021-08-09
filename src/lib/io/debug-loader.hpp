// GPL v3 (c) 2021, Daniel Williams 

// standard library includes
#include <string>
#include <vector>

#pragma once

#include "chart-box.hpp"
#include "base-loader.hpp"

namespace chartbox::io {

template< typename layer_t >
class DebugLoader : BaseLoader<layer_t, DebugLoader<layer_t> > {
public:

    DebugLoader( FrameMapping& _mapping, layer_t& _layer )
        : layer_(_layer)
        , mapping_(_mapping)
        {}


    bool load( const std::filesystem::path& /*path*/) {
        fmt::print("        >>> With DebugLoader:\n");
        fmt::print("        <<< This loading is pass-through.  It performs no actions.\n");
        return true; 
    }

private:
    layer_t& layer_;
    FrameMapping& mapping_;

};

} // namespace chart::io