// This file is provided in addition to snark and is not an integral
// part of snark library.
// Copyright (c) 2018 Vsevolod Vlaskine
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// snark is a generic and flexible library for robotics research
// Copyright (c) 2011 The University of Sydney
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the University of Sydney nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/// @author vsevolod vlaskine

#include <memory.h>
#include <cmath>
#include <boost/tuple/tuple.hpp>
#include "packet.h"

namespace snark { namespace robosense {

std::pair< double, double > msop::packet::data_t::azimuths( unsigned int block ) const // quick and dirty
{
    double t = blocks[ block ].azimuth_as_radians();
    double r;
    double d;
    if( block + 1 == number_of_blocks ) // quick and dirty; watch precision
    {
        double s = blocks[ block - 1 ].azimuth_as_radians();
        if( t < s ) { t += M_PI * 2; }
        d = ( t - s ) / 2;
    }
    else
    {
        double s = blocks[ block + 1 ].azimuth_as_radians();
        if( s < t ) { s += M_PI * 2; }
        d = ( s - t ) / 2;
    }
    r = t + d;
    if( r > M_PI * 2 ) { r -= M_PI * 2; }
    return std::make_pair( t, r );
}

static double azimuth_step_( const std::pair< double, double >& azimuths )
{ 
    return ( ( azimuths.first > azimuths.second ? azimuths.second + M_PI * 2 : azimuths.second ) - azimuths.first ) * ( msop::packet::data_t::laser_return::firing_interval() / msop::packet::data_t::block::firing_interval() );
}

msop::packet::const_iterator::const_iterator() : packet_( NULL ), done_( true ) {}

msop::packet::const_iterator::const_iterator( const packet* p )
    : packet_( p )
    , block_( 0 )
    , subblock_( 0 )
    , done_( false )
{
    update_value_( packet_->data.azimuths( block_ ) );
}

void msop::packet::const_iterator::update_value_()
{
    const msop::packet::data_t::laser_return& r = packet_->data.blocks[block_].channels[subblock_][value_.id];
    value_.range = r.range();
    value_.reflectivity = r.reflectivity(); // todo: apply curves
    value_.delay = msop::packet::data_t::laser_return::firing_interval() * value_.id + ( subblock_ == 0 ? 0.0 : msop::packet::data_t::block::firing_interval() / 2 );
    value_.azimuth += value_.azimuth_step;
}

void msop::packet::const_iterator::update_value_( double azimuth )
{
    update_value_();
    value_.azimuth = azimuth;
}

void msop::packet::const_iterator::update_value_( const std::pair< double, double >& azimuths )
{
    update_value_( azimuths.first );
    value_.azimuth_step = robosense::azimuth_step_( azimuths );
}

void msop::packet::const_iterator::operator++()
{
    ++value_.id;
    if( value_.id < msop::packet::data_t::number_of_lasers ) { update_value_(); return; }
    ++subblock_;
    value_.id = 0;
    if( subblock_ < msop::packet::data_t::number_of_subblocks ) { update_value_( packet_->data.azimuths( block_ ).second ); return; }
    subblock_ = 0;
    ++block_;
    if( block_ == msop::packet::data_t::number_of_blocks ) { done_ = true; return; }
    update_value_( packet_->data.azimuths( block_ ) );
}

bool msop::packet::valid() const // quick and dirty; what is azimuth 0xffff?
{
    return data.blocks[0].azimuth() <= 36000;
}

bool msop::packet::const_iterator::value_type::valid() const
{
    static unsigned int min_range = 2; // as in https://github.com/RoboSense-LiDAR/ros_rslidar/blob/develop-curves-function/rslidar_pointcloud/src/rawdata.cc
    static unsigned int max_range = 20000; // as in https://github.com/RoboSense-LiDAR/ros_rslidar/blob/develop-curves-function/rslidar_pointcloud/src/rawdata.cc
    return range > min_range && range < max_range;
}

template < unsigned int Size >
static std::array< char, Size > make_zeroes() { std::array< char, Size > a; ::memset( &a[0], 0, a.size() ); return a; }

bool difop::packet::data_t::corrected_vertical_angles_t::empty() const
{
    const char* p = values[0].data(); // as in ros driver
    for( unsigned int i = 0; i < 4; ++i ) { if( p[i] != 0x00 && p[i] != 0xff ) { return false; } } // as in ros driver
    return true;
    
    //enum { size_in_bytes = msop::packet::data_t::number_of_lasers * difop::packet::data_t::corrected_vertical_angle::size };
    //static std::array< char, size_in_bytes > zeroes = make_zeroes< size_in_bytes >();
    //return ::memcmp( corrected_vertical_angles.data(), &zeroes[0], size_in_bytes ) == 0;
}

double difop::packet::data_t::top_board_firmware_version_t::range_resolution() const
{
    auto d = value.data();
    return ( d[1] == 0x00 && d[2] == 0x00 && d[3] == 0x00 ) || ( d[1] == 0xff && d[2] == 0xff && d[3] == 0xff ) || ( d[1] == 0x55 && d[2] == 0xaa && d[3] == 0x5a ) ? 0.01 : 0.005;
}

} } // namespace snark { namespace robosense {
