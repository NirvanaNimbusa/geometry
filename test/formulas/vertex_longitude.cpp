// Boost.Geometry
// Unit Test

// Copyright (c) 2017 Oracle and/or its affiliates.

// Contributed and/or modified by Vissarion Fysikopoulos, on behalf of Oracle

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <geometry_test_common.hpp>

#include "test_formula.hpp"
#include "vertex_longitude_cases.hpp"

#include <boost/geometry/formulas/vertex_latitude.hpp>
#include <boost/geometry/formulas/vertex_longitude.hpp>
#include <boost/geometry/formulas/vincenty_inverse.hpp>
#include <boost/geometry/formulas/thomas_inverse.hpp>
#include <boost/geometry/formulas/andoyer_inverse.hpp>

#include <boost/geometry/strategies/strategies.hpp>

#include <boost/geometry/util/math.hpp>

namespace bg = boost::geometry;

template
<
        template <typename, bool, bool, bool, bool, bool> class FormulaPolicy,
        typename CS_Tag,
        typename CT
>
CT test_vrt_lon(CT lon1r,
                CT lat1r,
                CT lon2r,
                CT lat2r)
{
    // WGS84
    bg::srs::spheroid<CT> spheroid(6378137.0, 6356752.3142451793);

    typedef FormulaPolicy<CT, false, true, false, false, false> formula;
    CT a1 = formula::apply(lon1r, lat1r, lon2r, lat2r, spheroid).azimuth;

    CT vertex_lat = bg::formula::vertex_latitude<CT, bg::geographic_tag>
                            ::apply(lat1r, a1, spheroid);

    bg::strategy::azimuth::geographic<> azimuth_geographic;

    std::cout << "(lat=" << vertex_lat * bg::math::r2d<double>() << ")" << std::endl;

    return bg::formula::vertex_longitude<CT, CS_Tag>::
                                                  apply(lon1r, lat1r,
                                                        lon2r, lat2r,
                                                        vertex_lat,
                                                        a1,
                                                        azimuth_geographic);

}

void test_all(expected_results const& results)
{
    double const d2r = bg::math::d2r<double>();
    double const r2d = bg::math::r2d<double>();

    double lon1r = results.p1.lon * d2r;
    double lat1r = results.p1.lat * d2r;
    double lon2r = results.p2.lon * d2r;
    double lat2r = results.p2.lat * d2r;

    double res_an = test_vrt_lon<
                                    bg::formula::andoyer_inverse,
                                    bg::geographic_tag
                                >(lon1r, lat1r, lon2r, lat2r);
    double res_th = test_vrt_lon<
                                    bg::formula::thomas_inverse,
                                    bg::geographic_tag
                                >(lon1r, lat1r, lon2r, lat2r);
    double res_vi = test_vrt_lon<
                                    bg::formula::vincenty_inverse,
                                    bg::geographic_tag
                                >(lon1r, lat1r, lon2r, lat2r);
    double res_sh = test_vrt_lon<
                                    bg::formula::andoyer_inverse,
                                    bg::spherical_equatorial_tag
                                >(lon1r, lat1r, lon2r, lat2r);

    std::cout.precision(10);
    std::cout << res_an * bg::math::r2d<double>() << std::endl;
    std::cout << res_th * bg::math::r2d<double>() << std::endl;
    std::cout << res_vi * bg::math::r2d<double>() << std::endl;
    std::cout << res_sh * bg::math::r2d<double>() << std::endl<< std::endl;

    check_one(res_an, results.andoyer * d2r, res_vi, 0.001);
    check_one(res_th, results.thomas * d2r, res_vi, 0.00001);
    check_one(res_vi, results.vincenty * d2r, res_vi, 0.0000001);
    check_one(res_sh, results.spherical * d2r, res_vi, 1);
}


int test_main(int, char*[])
{

    for (size_t i = 0; i < expected_size; ++i)
    {
        test_all(expected[i]);
    }

    return 0;
}

