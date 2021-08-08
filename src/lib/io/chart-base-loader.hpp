// GPL v3 (c) 2021, Daniel Williams 
#pragma once

// standard library includes
#include <string>


namespace chartbox::io {

template< typename layer_t, typename loader_t >
class ChartBaseLoader {
public:
    bool load(const std::filesystem::path& path){ return loader().load(path); }

protected:

    ChartBaseLoader() = default;

    loader_t& loader() { 
        return *static_cast<loader_t*>(this); }

    const loader_t& loader() const {
        return *static_cast<const loader_t*>(this);}

    ~ChartBaseLoader() = default;

};

} // namespace chartbox::io
