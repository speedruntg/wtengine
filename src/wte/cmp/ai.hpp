/*!
 * \brief WTEngine | File:  ai.hpp
 * \author Matthew Evans
 *
 * \version 0.1a
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2020
 *
 * \details AI component - tag to be processed by logic system.
 */

#ifndef WTE_CMP_AI_HPP
#define WTE_CMP_AI_HPP

#include <functional>

#include "component.hpp"
#include "../mgr/entity_manager.hpp"
#include "../mgr/message_manager.hpp"

namespace wte
{

namespace cmp
{

//! AI component
/*!
 * Tag components to be processed by the Logic system.
 * Define logic in do_ai lambda.
 */
class ai final : public component {
    public:
        /*!
         * ...
         * \param void
         * \return void
         */
        inline ai(void func(entity, mgr::entity_manager&, mgr::message_manager&, int64_t)) : do_ai(func) {};

        /*!
         * ...
         * \param void
         * \return void
         */
        inline ~ai() {};

        /*!
         * ...
         * \param void
         * \return void
         */
        inline void run(entity eid, mgr::entity_manager& world, mgr::message_manager& messages, int64_t engine_time) {
            do_ai(eid, world, messages, engine_time);
        };

    private:
        std::function<void(entity, mgr::entity_manager&, mgr::message_manager&, int64_t)> do_ai;
};

} //  namespace cmp

} //  namespace wte

#endif