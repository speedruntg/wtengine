/*!
 * WTEngine | File:  motion.cpp
 * 
 * \author Matthew Evans
 * \version 0.71
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2021
 */

#include "wtengine/cmp/motion.hpp"

namespace wte::cmp {

/*
 *
 */
motion::motion(
    const float& d,
    const float& xv,
    const float& yv
) : direction(d), x_vel(xv), y_vel(yv) {}

}  //  end namespace wte::cmp
