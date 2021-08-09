// GPL v3 (c) 2021, Daniel Williams 
#pragma once

// standard library includes
#include <string>

#include "base-writer.hpp"

namespace chartbox::io {

template< typename layer_t >
class PNGWriter : BaseWriter<layer_t, PNGWriter<layer_t> > {
public:
    const static std::string extension;
    
    PNGWriter( layer_t& _source_layer )
        : layer_(_source_layer)
        {}

    bool write( const std::filesystem::path& filename );

private:
    layer_t& layer_;

}; // class PNGWriter

} // namespace chartbox::io

#include "png-writer.inl"
