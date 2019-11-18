// GPL v3 (c) 2020

#ifndef _YGGDRASIL_WRITERS_HPP_
#define _YGGDRASIL_WRITERS_HPP_

#include <cstdio>
#include <string>
#include <vector>

#include <Eigen/Geometry>

#include <nlohmann/json.hpp>

#include "geometry/cell_value.hpp"
#include "geometry/layout.hpp"
#include "io/json.hpp"


namespace yggdrasil::io {

template<typename target_t>
void to_console(const target_t& target);

///! \brief writes a json document to given output stream
template<typename target_t>
bool to_json(const target_t& t, std::ostream& document);

///! \brief outputs this yggdrasil into the given json document
template<typename target_t>
bool to_json_grid(const target_t& t, nlohmann::json& doc);

///! \brief writes a png file to the given outstream
template<typename target_t>
bool to_png(const target_t& t, const std::string& filename);

template<typename target_t>
bool to_png(const target_t& t, FILE* dest);


}; // namespace yggdrasil::io

#include "writers.inl"

#endif  // #ifdef _YGGDRASIL_WRITERS_HPP_
