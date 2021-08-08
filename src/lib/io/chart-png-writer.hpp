// GPL v3 (c) 2021, Daniel Williams 
#pragma once

// standard library includes
#include <string>

#include "chart-base-writer.hpp"

namespace chartbox::io {

template< typename layer_t >
class PNGWriter : ChartBaseWriter<layer_t, PNGWriter<layer_t> > {
public:
    const static std::string extension;
    
    PNGWriter( layer_t& _source_layer )
        : layer_(_source_layer)
        {}

    bool write_to_path( const std::filesystem::path& filename );

private:
    layer_t& layer_;

}; // class PNGWriter

} // namespace chartbox::io

#include "chart-png-writer.inl"
