/*!
 * WTEngine | File:  colision.hpp
 * 
 * \author Matthew Evans
 * \version 0.1a
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2020
 */

#ifndef WTE_SYS_COLISION_HPP
#define WTE_SYS_COLISION_HPP

#include "system.hpp"

namespace wte
{

namespace sys
{

/*!
 * \class colision
 * \brief Will select components by team and test for a colision
 */
class colision final : public system {
    public:
        colision() : system("colision") {};
        ~colision() {};

        /*!
         * \brief Allow the colision system to be disabled.
         */
        void disable(void) override { enabled = false; };

        /*!
         * \brief Colision system run.
         * 
         * Overrides virtual run member inherited from system object.
         * Selects components by team, then tests each team to see if there is a colision.
         * 
         * \param world Reference to the entity manager.
         * \param messages Reference to the message manager.
         * \param current_time Current engine time.
         */
        void run(mgr::entity_manager& world,
                        mgr::message_manager& messages,
                        const int64_t& current_time) override {
            const_component_container<cmp::team> team_components = world.get_components<cmp::team>();

            for(auto & it_a : team_components) {
                for(auto & it_b : team_components) {
                    /*
                     * Only test if:  Not the same entity.
                     *                Entities are on different teams.
                     *                Both entities have a location.
                     *                Both entities have a hitbox.
                     *                Both entities are enabled.
                     *                The entity being compared against is solid.
                     */
                    if((it_a.first != it_b.first) &&
                       (it_a.second->get_team() != it_b.second->get_team()) &&
                       world.has_component<cmp::location>(it_a.first) &&
                       world.has_component<cmp::location>(it_b.first) &&
                       world.has_component<cmp::hitbox>(it_a.first) &&
                       world.has_component<cmp::hitbox>(it_b.first) &&
                       world.get_component<cmp::enabled>(it_a.first)->check() &&
                       world.get_component<cmp::enabled>(it_b.first)->check() &&
                       world.get_component<cmp::hitbox>(it_b.first)->is_solid())
                    {
                        //  Use AABB to test colision
                        if((world.get_component<cmp::location>(it_a.first)->get_x() <
                            world.get_component<cmp::location>(it_b.first)->get_x() +
                            world.get_component<cmp::hitbox>(it_b.first)->get_width()
                            && 
                            world.get_component<cmp::location>(it_a.first)->get_x() +
                            world.get_component<cmp::hitbox>(it_a.first)->get_width() >
                            world.get_component<cmp::location>(it_b.first)->get_x())
                           &&
                           (world.get_component<cmp::location>(it_a.first)->get_y() <
                            world.get_component<cmp::location>(it_b.first)->get_y() +
                            world.get_component<cmp::hitbox>(it_b.first)->get_height()
                            && 
                            world.get_component<cmp::location>(it_a.first)->get_y() +
                            world.get_component<cmp::hitbox>(it_a.first)->get_height() >
                            world.get_component<cmp::location>(it_b.first)->get_y()))
                        {
                            //  Send a message that two entities colided.
                            //  Each entity will get a colision message.
                            //  Ex:  A hit B, B hit A.
                            messages.add_message(message("entities",
                                                         world.get_name(it_a.first),
                                                         world.get_name(it_b.first),
                                                         "colision", ""));
                        }
                    } //  End skip self check
                } //  End it_b loop
            } //  End it_a loop
        }
};

} //  namespace sys

} //  namespace wte

#endif