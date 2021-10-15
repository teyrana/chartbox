// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <memory>

#include <ogr_spatialref.h>

#include "bound-box.hpp"
#include "global-location.hpp"
#include "utm-location.hpp"
#include "local-location.hpp"
#include "polygon.hpp"

namespace chartbox::geometry {

class FrameMapping {
public:
    FrameMapping();

    // NYI
    bool move_to_center( const GlobalLocation& center, const double width );

    bool move_to_corners( const BoundBox<GlobalLocation>& bounds );
    
    // NYI
    bool move_to_corners( const BoundBox<UTMLocation>& bounds );

    inline const BoundBox<GlobalLocation>& global_bounds() const { return global_bounds_; }
    inline const BoundBox<UTMLocation>& utm_bounds() const { return utm_bounds_; }
    BoundBox<LocalLocation> local_bounds() const;
    
    // BoundBox<GlobalLocation> map_to_global( const BoundBox<UTMLocation>& from ) const;
    GlobalLocation map_to_global( const UTMLocation& from ) const;

    BoundBox<LocalLocation> map_to_local( const BoundBox<GlobalLocation>& from ) const;
    LocalLocation map_to_local( const GlobalLocation& from ) const;
    Polygon<LocalLocation> map_to_local( const Polygon<GlobalLocation>& from ) const;

    // const BoundBox<UTMLocation>& map_to_utm( const BoundBox<GlobalLocation>& from ) const;
    UTMLocation map_to_utm( const GlobalLocation& from ) const;

    void print() const;

    constexpr static size_t snap_power_2(const size_t target);

    ~FrameMapping() = default;

protected:
    BoundBox<GlobalLocation> global_bounds_;
    OGRSpatialReference global_frame_;

    OGRCoordinateTransformation * global_to_utm_transform_;
    OGRCoordinateTransformation * utm_to_global_transform_;

    BoundBox<UTMLocation> utm_bounds_;
    OGRSpatialReference utm_frame_;

    constexpr static double min_local_width_ = 128;   // === 2^7
    double current_local_width_ = min_local_width_;
    constexpr static double max_local_width_ = 16384; // === 2^14


}; // class FrameMapping< mapping_t >

} // namespace chart