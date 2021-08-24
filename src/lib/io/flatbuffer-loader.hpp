// GPL v3 (c) 2021, Daniel Williams 

// standard library includes
#include <string>
#include <vector>

#pragma once

#include "chart-box.hpp"
#include "base-loader.hpp"

namespace chartbox::io::flatbuffer {

const std::string extension = ".fb";

template< typename layer_t >
class FlatBufferLoader : BaseLoader<layer_t, FlatBufferLoader<layer_t> > {
public:

    FlatBufferLoader( FrameMapping& _mapping, layer_t& _layer )
        : layer_(_layer)
        , mapping_(_mapping)
        {}


    bool load( const std::filesystem::path& path);

private:
    layer_t& layer_;
    FrameMapping& mapping_;

};

} // namespace chart::io

#include "flatbuffer-loader.inl"