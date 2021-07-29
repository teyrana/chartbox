// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <memory>

#include <Eigen/Geometry>

#include <gdal.h>
#include <ogr_geometry.h>


namespace chartbox {

class FrameMapping {
public:
    FrameMapping();

    bool move_to_center( const Eigen::Vector2d& center, const double width );

    bool move_to_corners( const Eigen::Vector2d& min, const Eigen::Vector2d& max );

    inline const Eigen::AlignedBox2d& global_bounds() const { return global_bounds_; }
    inline const Eigen::AlignedBox2d& utm_bounds() const { return utm_bounds_; }


    bool move_local_bounds( const Eigen::Vector2d& min_lon_lat, const Eigen::Vector2d& max_lon_lat );
    
    void print() const;

    constexpr static size_t snap_power_2(const size_t target);

    OGRPoint* to_global( const double easting, const double northing );
    OGRPoint* to_utm( const double longitude, const double latitude );
 
    Eigen::Vector2d to_local( const Eigen::Vector2d& from_lat_lon );
    Eigen::Vector2d to_global( const Eigen::Vector2d& from_local );
    Eigen::Vector2d to_utm( const Eigen::Vector2d& from_lat_lon );

    ~FrameMapping() = default;

protected:
    Eigen::AlignedBox2d global_bounds_;
    OGRSpatialReference global_frame_;

    OGRCoordinateTransformation * global_to_utm_transform_;
    OGRCoordinateTransformation * utm_to_global_transform_;

    Eigen::AlignedBox2d utm_bounds_;
    OGRSpatialReference utm_frame_;


    constexpr static double min_local_width_ = 128;   // === 2^7
    double current_local_width_ = min_local_width_;
    constexpr static double max_local_width_ = 16384; // === 2^14


}; // class FrameMapping< mapping_t >

} // namespace chart