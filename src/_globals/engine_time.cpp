/*!
 * wtengine | File:  engine_time.cpp
 * 
 * \author Matthew Evans
 * \version 0.7.2
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2022
 */

#include "wtengine/_globals/engine_time.hpp"

namespace wte {

int64_t engine_time::current_time = 0;

/*
 *
 */
const std::int64_t engine_time::check(void) { return current_time; };

/*
 *
 */
void engine_time::set(const std::int64_t& t) {  current_time = t; };

}  //  end namespace wte
