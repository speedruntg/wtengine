/*!
 * WTEngine | File:  hitbox.cpp
 * 
 * \author Matthew Evans
 * \version 0.71
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2021
 */

#include "wtengine/cmp/hitbox.hpp"

namespace wte::cmp {

/*
 *
 */
hitbox::hitbox(
    const float& w,
    const float& h,
    const std::size_t& t
) : width(w), height(h), team(t), solid(true) {}

/*
 *
 */
hitbox::hitbox(
    const float& w,
    const float& h,
    const std::size_t& t,
    const bool& s
) : width(w), height(h), team(t), solid(s) {}

}  //  end namespace wte::cmp
