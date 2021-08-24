// GPL v3 (c) 2021, Daniel Williams 

#include "base-writer.hpp"

#pragma once

namespace chartbox::io::flatbuffer {


template< typename layer_t >
class FlatBufferWriter : BaseWriter<layer_t, FlatBufferWriter<layer_t> > {
public:

    FlatBufferWriter( layer_t& _source_layer )
        : layer_(_source_layer)
        {}

    bool write( const std::filesystem::path& /*filename*/ );

private:
    layer_t& layer_;

}; // class DebugWriter

} // namespace chartbox::io

#include "flatbuffer-writer.inl"
