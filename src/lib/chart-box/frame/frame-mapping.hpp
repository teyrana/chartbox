// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <memory>

#include <ogr_spatialref.h>

#include "bound-box.hpp"
#include "location2en.hpp"
#include "location2ll.hpp"
#include "location2xy.hpp"

namespace chartbox::frame {

class FrameMapping {
public:
    FrameMapping();    

    // NYI
    bool move_to_center( const Location2LL& center, const double width );

    bool move_to_corners( const BoundBox<Location2LL>& bounds );
    
    // NYI
    bool move_to_corners( const BoundBox<Location2EN>& bounds );

    inline const BoundBox<Location2LL>& global_bounds() const { return global_bounds_; }
    inline const BoundBox<Location2EN>& utm_bounds() const { return utm_bounds_; }

    void print() const;

    constexpr static size_t snap_power_2(const size_t target);

    Location2LL to_global( const Location2EN& from );
    Location2EN to_utm( const Location2LL& from );
    /// not convinced this api function is worthwhile
    // Location2EN to_local( const Location2LL& from );
    Location2xy to_local( const Location2LL& from );


    ~FrameMapping() = default;

protected:
    BoundBox<Location2LL> global_bounds_;
    OGRSpatialReference global_frame_;

    OGRCoordinateTransformation * global_to_utm_transform_;
    OGRCoordinateTransformation * utm_to_global_transform_;

    BoundBox<Location2EN> utm_bounds_;
    OGRSpatialReference utm_frame_;

    constexpr static double min_local_width_ = 128;   // === 2^7
    double current_local_width_ = min_local_width_;
    constexpr static double max_local_width_ = 16384; // === 2^14


}; // class FrameMapping< mapping_t >

} // namespace chart