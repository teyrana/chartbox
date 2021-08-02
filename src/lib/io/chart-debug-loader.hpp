// GPL v3 (c) 2021, Daniel Williams 

// standard library includes
#include <string>
#include <vector>

#pragma once

#include "chart-box.hpp"
#include "chart-base-loader.hpp"

namespace chartbox::io {

template< typename layer_t >
class DebugLoader : ChartBaseLoader<layer_t, DebugLoader<layer_t> > {
public:

    DebugLoader( FrameMapping& _mapping, layer_t& _layer )
        : layer_(_layer)
        , mapping_(_mapping)
        {}


    bool load_file(const std::string& /*filename*/ ){ return true; }
    bool load_text(const std::string& /*source*/ ){ return true; }
    // bool load_binary( const std::stream& source);

private:
    layer_t& layer_;
    FrameMapping& mapping_;

};

} // namespace chart::io