// GPL v3 (c) 2021, Daniel Williams 

// standard library includes
#include <iostream>
#include <string>
#include <vector>

#pragma once

namespace chartbox::io {

template< typename layer_t, typename writer_t >
class BaseWriter {
public:
    bool write( const std::filesystem::path& filename ){ 
        return writer().write(filename); }

protected:
    BaseWriter() = default;

    writer_t& writer() { 
        return *static_cast<writer_t*>(this); }

    const writer_t& writer() const {
        return *static_cast<const writer_t*>(this);}

    ~BaseWriter() = default;

};

} // namespace chartbox::io
