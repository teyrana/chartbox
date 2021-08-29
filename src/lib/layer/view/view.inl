// GPL v3 (c) 2021, Daniel Williams 

#include <cmath>

#include <fmt/core.h>

using chartbox::layer::ChartLayerInterface;
using chartbox::layer::ViewLayer;
using chartbox::layer::VIEW;
using chartbox::geometry::BoundBox;
using chartbox::geometry::UTMLocation;

template< typename layer_0_t, typename layer_1_t >
ViewLayer<layer_0_t,layer_1_t>::ViewLayer( const BoundBox<UTMLocation>& _bounds, layer_0_t _layer0, layer_1_t _layer1 )
    : ChartLayerInterface< ViewLayer<layer_0_t,layer_1_t>, VIEW >( _bounds)
    , layer_0_(_layer0)
    , layer_1_(_layer1)
{
    fmt::print( "======== ======== ViewLayer Constructor:  ======== ======== \n" );
    fmt::print( "    ( Hardcoded Layers: )");
    fmt::print( "    ::[0]: Layer: {}\n", layer_0_.name() );
    fmt::print( "    ::[1]: Layer: {}\n", layer_1_.name() );

    assert( layer_0_.dimension == layer_1_.dimension );
}

template< typename layer_0_t, typename layer_1_t >
uint8_t ViewLayer<layer_0_t,layer_1_t>::get(const UTMLocation& p) const {
    const uint8_t value0 = layer_0_.get(p);
    const uint8_t value1 = layer_1_.get(p);
    return std::max( value0, value1 );
}

