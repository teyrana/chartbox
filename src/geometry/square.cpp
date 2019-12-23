
const string x_key("x");
const string y_key("y");
const string precision_key("precision");
const string width_key("width");

using yggdrasil::geometry::Square;

// ========= ========= ========= ========= ========= =========

constexpr Square::Square() 
    : dimension(1), width(1.), x_c(0.), y_c(0.),  // primary fields
{}

constexpr Square::Square(const double _precision, const double x_in, const double y_in, const double _width)
    : precision(snap_precision(_precision)), width(snap_width(_width)), x_c(x_in), y_c(y_in),     // primary fields
      dimension(width/precision), half_width(width/2), padding(calculate_padding(dimension)), size(dimension*dimension)     // derived fields
{
    assert( dimension <= maximum_supported_dimension );
}

// ========= ========= ========= ========= ========= =========

bool Square::contains(const Vector2d& at) const {
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

constexpr uint8_t Layout::calculate_padding( const double dimension ){
    size_t power = 1;
    size_t padding = 64;

    // this loop basically calculates log2(dimension), but in constexpr 
    while( dimension > power){
        power <<= 1;
        padding -= 2;
    }

    return padding;
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


constexpr double Layout::limit_x( const double x_in) const {
    if( x_in < get_x_min()){
        return get_x_min();
    }else if( x_in > get_x_max()){
        return get_x_max();
    }else{
        return x_in;
    }
}



constexpr double Layout::limit_y( const double x_in) const {
    if( x_in < get_x_min()){
        return get_x_min();
    }else if( x_in > get_x_max()){
        return get_x_max();
    }else{
        return x_in;
    }
}


constexpr double Layout::snap_precision( double _precision){
    double next_precision = 1.;
    while( 1 < _precision ){
        if( 2 > _precision ){
            return next_precision;
        }
        _precision *= 0.5;
        next_precision *= 2;
    }

    return next_precision;
}

// constexpr double Layout::snap_precision( const double precision, const double width){
//     const double dimension_estimate = width / precision;
//     // calculate the next-higher power-of-2, in constexpr form
//     size_t power = 1;
//     while( dimension_estimate > power){
//         power = power << 1;
//     }
//     return width / power;
// }

constexpr double Layout::snap_width( const double _width){
    double next_width = precision;

    while( next_width < _width){
        next_width *= 2;
    }

    return next_width;
}

std::unique_ptr<Square> Square::make_from_json(nlohmann::json& doc){
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

    return std::make_unique<Square>(precision, x, y, width);
}

    
nlohmann::json Square::to_json() const {
    return {{"precision", precision},
            {"x", x_c},
            {"y", y_c},
            {"width", width}};
}

std::string Square::to_string() const {
    return "x: " + std::to_string(x_c)\
        +  ", y: " + std::to_string(y_c)\
        +  ", prec: " + std::to_string(precision)\
        +  ", width: " + std::to_string(width); 
}

template<typename T, size_t d, size_t w>
constexpr double Grid<T,d,w>::limit_x( const double x_in) const {
    if( x_in < get_x_min()){
        return T.get_x_min();
    }else if( x_in > T.get_x_max()){
        return T.get_x_max();
    }else{
        return x_in;
    }
}

template<typename T, size_t d, size_t w>
constexpr double Grid<T,d,w>::limit_y( const double y_in) const {
    if( y_in < T.get_y_min()){
        return T.get_y_min();
    }else if( y_in > T.get_y_max()){
        return T.get_y_max();
    }else{
        return y_in;
    }
}