// GPL v3 (c) 2020

#include <cstring>
#include <memory>

#include <Eigen/Geometry>

#include <nlohmann/json.hpp>

#include "geometry/layout.hpp"

using std::string;

using Eigen::Vector2d;

namespace yggdrasil::geometry {

const string x_key("x");
const string y_key("y");
const string precision_key("precision");
const string width_key("width");

std::unique_ptr<Layout> Layout::make_from_json(nlohmann::json& doc){
    if(!doc.contains(precision_key) || !doc[precision_key].is_number()){
        return {};
    }else if(!doc.contains(x_key) || !doc[x_key].is_number()){
        return {};
    }else if(!doc.contains(y_key) || !doc[y_key].is_number()){
        return {};
    }else if(!doc.contains(width_key) || !doc[width_key].is_number()){
        return {};
    }

    const double precision = doc[precision_key].get<double>();
    const double x = doc[x_key].get<double>();
    const double y = doc[y_key].get<double>();
    const double width = doc[width_key].get<double>();

    return std::make_unique<Layout>(precision, x, y, width);
}


bool Layout::contains(const Vector2d& at) const {
    // outside x-bounds:
    if( (at[0] < x_c - half_width) || (at[0] > x_c + half_width) ){
        return false;
    }

    // outside y-bounds:
    if( (at[1] < y_c - half_width) || (at[1] > y_c + half_width) ){ 
        return false;
    }
    return true;
}
    
const Vector2d Layout::get_anchor() const {
    return {get_x_min(), get_y_min()};
}

double Layout::get_x_max() const {
    return x_c + half_width;
}
double Layout::get_x_min() const {
    return x_c - half_width;
}
double Layout::get_y_max() const {
    return y_c + half_width;
}
double Layout::get_y_min() const {
    return y_c - half_width;
}

bool Layout::operator!=(const Layout& other) const {
    return ! (this->operator==(other));
}

bool Layout::operator==(const Layout& other) const {
    const double error =  abs(precision - other.precision) 
                        + abs(x_c - other.x_c)
                        + abs(y_c - other.y_c)
                        + abs(width - other.width);

    if( epsilon > error){
        return true;
    }

    return false;
}

nlohmann::json Layout::to_json() const {
    return {{"precision", precision},
            {"x", x_c},
            {"y", y_c},
            {"width", width}};
}

std::string Layout::to_string() const {
    return "x: " + std::to_string(x_c)\
        +  ", y: " + std::to_string(y_c)\
        +  ", prec: " + std::to_string(precision)\
        +  ", width: " + std::to_string(width); 
}

} // namespace yggdrasil::geometry
