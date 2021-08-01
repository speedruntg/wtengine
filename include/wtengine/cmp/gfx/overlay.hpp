/*!
 * WTEngine | File:  overlay.hpp
 * 
 * \author Matthew Evans
 * \version 0.5
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2021
 */

#ifndef WTE_CMP_OVERLAY_HPP
#define WTE_CMP_OVERLAY_HPP

#include <string>
#include <map>
#include <functional>
#include <memory>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "wtengine/cmp/gfx/animator.hpp"

#include "wtengine/_globals/wrappers.hpp"
#include "wtengine/_globals/wte_asset.hpp"
#include "wtengine/mgr/assets.hpp"

namespace wte
{

namespace cmp
{

/*!
 * \class overlay
 * \brief Component for storing an overlay image and defining its animation process.
 */
class overlay final : public animator {

    public:
        /*!
         * \brief Create a new Overlay component.
         * \param bmp Bitmap asset to use.
         * \param font Font asset to use.
         * \param x Horizontal location of the overlay in pixels.
         * \param y Verticle location of the overlay in pixels.
         * \param l Layer for sorting.
         * \param func Function to define what is displayed in the overlay.
         */
        overlay(
            std::shared_ptr<wte_asset> bmp,
            std::shared_ptr<wte_asset> font,
            const float& x,
            const float& y,
            const std::size_t& l,
            const std::function<void(const entity_id&)>& func
        );

        ~overlay();

        /*!
         * \brief Draw text on the overlay.
         * \param txt Text to be displayed.
         * \param color Allegro color object.
         * \param x Horizontal location of the text.
         * \param y Verticle location of the text.
         * \param f Text flags for drawing - see Allegro docs on al_draw_text.
         */
        void draw_text(
            const std::string& txt,
            const ALLEGRO_COLOR& color,
            const float& x,
            const float& y,
            const int& f
        );

        /*!
         * \brief Get X position.
         * \return X position.
         */
        const float get_pos_x(void) const;

        /*!
         * \brief Get Y position.
         * \return Y position.
         */
        const float get_pos_y(void) const;

    private:
        //  Font for overlay.
        std::shared_ptr<wte_asset> overlay_font;
        float pos_x, pos_y;  //  X/Y position of overlay.
};

} //  namespace cmp

} //  namespace wte

#endif
