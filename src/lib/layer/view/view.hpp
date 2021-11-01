// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <cmath>
#include <memory>
#include <cstdlib>
#include <string>
#include <vector>

#include "layer/layer-interface.hpp"

namespace chartbox::layer {


/// \brief this class provides a specialized view into a subset of the chart layers
///
/// \note this class is not currently used or tested -- it turns out to just not be necessary
template< typename layer_0_t, typename layer_1_t >
class ViewLayer final : public LayerInterface<ViewLayer<layer_0_t, layer_1_t>, VIEW> {
public:

    /// \brief name of this layer's type
    constexpr static char type_name_[] = "ViewLayer";

public:

    ViewLayer() = delete;
    
    ViewLayer( const BoundBox<UTMLocation>& _bounds, layer_0_t _layer0, layer_1_t _layer1 );

    size_t dimension() const { return layer_0_.dimension; }

    bool fill( const uint8_t value ){ return false; }

    bool fill( const uint8_t* const buffer, size_t count ){ return false; }
    
    bool fill( const BoundBox<LocalLocation>& box, const uint8_t value ){ return false; }

    bool fill( const Polygon<LocalLocation>& poly, uint8_t value ){ return false; }

    /// \brief read only layer!  this function will not be implemented.
    bool fill( const std::vector<uint8_t>& source ){ return false; }

    // cannot define this function in a meaningful way -- 
    // ... calling this function should be treated as a bug
    // uint8_t& get( const LocalLocation& p );

    uint8_t get(const geometry::LocalLocation& p) const ;

    double precision() const { return NAN; }

    /// \brief Draws a simple debug representation of this grid to stderr
    void print_contents() const {}

    /// \brief read only layer!  this function will not be implemented.
    inline void reset(){}

    /// \brief read only layer!  this function will not be implemented.
    bool store(const geometry::LocalLocation& p, const uint8_t new_value){ return false; }

    ~ViewLayer() = default;

private:
    const layer_0_t & layer_0_;
    const layer_1_t & layer_1_;

private:

    LayerInterface< ViewLayer<layer_0_t, layer_1_t>, VIEW>& super() {
        return *static_cast< LayerInterface< ViewLayer<layer_0_t, layer_1_t>, VIEW>* >(this);
    }

    const LayerInterface< ViewLayer<layer_0_t, layer_1_t>, VIEW>& super() const {
        return *static_cast< const LayerInterface< ViewLayer<layer_0_t, layer_1_t>, VIEW>* >(this);
    }
};

} // namespace

#include "view.inl"
