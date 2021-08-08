// GPL v3 (c) 2021, Daniel Williams 

#include "chart-base-writer.hpp"

#pragma once

namespace chartbox::io {

template< typename layer_t >
class DebugWriter : ChartBaseWriter<layer_t, DebugWriter<layer_t> > {
public:

    DebugWriter( layer_t& _source_layer )
        : layer_(_source_layer)
        {}

    bool write_to_path( const std::filesystem::path& /*filename*/ ){
        fmt::print("        >>> With DebugWriter:\n");
        fmt::print("        <<< This writer is pass-through.  It performs no actions.\n");
        return true;
    }

private:
    layer_t& layer_;

}; // class DebugWriter

} // namespace chartbox::io