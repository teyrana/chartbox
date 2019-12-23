// GPL v3 (c) 2020

#ifndef _YGGDRASIL_WRITERS_HPP_
#define _YGGDRASIL_WRITERS_HPP_

#include <cstddef>
#include <string>
#include <vector>

#include <Eigen/Geometry>

#include <nlohmann/json.hpp>

#include "io/json.hpp"


namespace yggdrasil::io {

#ifdef ENABLE_GDAL
///! \brief writes a png file to the given outstream
bool to_grayscale_png(const std::vector<std::byte>& source, const std::string& filename);
#endif 

}; // namespace yggdrasil::io

#endif  // #ifdef _YGGDRASIL_WRITERS_HPP_
