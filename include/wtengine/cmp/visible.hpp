/*!
 * WTEngine | File:  visible.hpp
 * 
 * \author Matthew Evans
 * \version 0.1a
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2020
 */

#ifndef WTE_CMP_VISIBLE_HPP
#define WTE_CMP_VISIBLE_HPP

#include "component.hpp"

namespace wte
{

namespace cmp
{

/*!
 * \class visible
 * \brief Toggles if an entity will be drawn by the renderer.
 */
class visible final : public component {
    public:
        /*!
         * \brief Visible constructor.
         * 
         * Call this to be visible by default.
         */
        visible() : is_visible(true) {};

        /*!
         * \brief Visible constructor.
         * 
         * Call this to set if the entity is visible.
         * 
         * \param v Boolean value for visibility.
         */
        visible(const bool v) : is_visible(v) {};

        /*!
         * \brief Check if visible.
         * 
         * \return True if visible, false if not.
         */
        const bool check(void) const {
            return is_visible;
        };

        /*!
         * \brief Show the entity.
         * 
         * Sets internal flag to true.
         */
        void show(void) {
            is_visible = true;
        };

        /*!
         * \brief Hide the entity.
         * 
         * Sets internal flag to false.
         */
        void hide(void) {
            is_visible = false;
        };

    private:
        bool is_visible;
};

} //  namespace cmp

} //  namespace wte

#endif