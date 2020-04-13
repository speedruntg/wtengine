/*!
 * \brief WTEngine | File:  game_cfg_map.hpp
 * \author Matthew Evans
 *
 * \version 0.1a
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2020
 *
 * \details Engine config.
 */

#ifndef WTE_GAME_CFG_MAP_HPP
#define WTE_GAME_CFG_MAP_HPP

#include <string>
#include <fstream>
#include <stdexcept>

#include "variable_map.hpp"

namespace wte
{

//!  Game config
/*!
 * Statics to store game config variables
 */
class game_cfg_map final : public variable_map<game_cfg_map> {
    public:
        /*!
         * Load
         * False on fail, true on success
         */
        inline static bool load(void) {
            std::ifstream data_file("game.cfg");
            if(!data_file.good()) return false;

            std::string it;
            //  Read each line, try to register or set
            while(std::getline(data_file, it)) {
                if(!reg(it)) set(it);
            }

            data_file.close();
            return true;
        };

        /*!
         * Clear save
         */
        inline static void clear_save(void) {
            std::ofstream data_file("game.cfg", std::ofstream::trunc);
            data_file.close();
        };

        /*!
         * Save
         * False on fail, true on success
         */
        inline static bool save(const std::string var) {
            std::ofstream data_file("game.cfg", std::ofstream::app);
            if(!data_file.good()) return false;

            try {
                data_file << var << "=" << _map.at(var) << std::endl;
            } catch (std::out_of_range& e) {
                data_file.close();
                return false;  //  Didn't find var
            }

            data_file.close();
            return true;
        };
};

typedef game_cfg_map::variable_map<game_cfg_map> game_cfg;

template <> inline std::map<std::string, std::string> game_cfg::_map = {};

} //  end namespace wte

#endif