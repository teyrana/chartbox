// GPL v3 (c) 2020

#ifndef _VIEW_HPP_
#define _VIEW_HPP_


namespace yggdrasil::geometry {

/// \brief Defines a 2-D view into a geometric x-y view
/// \param 
/// \param 
/// \param 

// class-template
class GeometryView {
public:
    GeometryView( double width, Vector2d anchor );

    constexpr cell_t& operator()( const double x, const double y);
    // constexpr const cell_t& operator()( const uint32_t i, const uint32_t j) const;

private:
    const double width;
    const Vector2d anchor;

};


} // namespace yggdrasil::geometry

// #include "view.inl"

#endif // #ifdef _VIEW_HPP_
