// GPL v3 (c) 2021, Daniel Williams 
#pragma once

// standard library includes
#include <string>


namespace chartbox::io {

template< typename layer_t, typename loader_t >
class BaseLoader {
public:
    bool load(const std::filesystem::path& path){ 
        return loader().load(path); }

protected:

    BaseLoader() = default;

    loader_t& loader() { 
        return *static_cast<loader_t*>(this); }

    const loader_t& loader() const {
        return *static_cast<const loader_t*>(this);}

    ~BaseLoader() = default;

};

} // namespace chartbox::io
