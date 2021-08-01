/*!
 * WTEngine | File:  menus.cpp
 * 
 * \author Matthew Evans
 * \version 0.5
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2021
 */

#include "wtengine/mgr/menus.hpp"

namespace wte
{

namespace mgr
{

template <> bool menus::manager<menus>::initialized = false;

mnu::menu_item_citerator menus::menu_position;
ALLEGRO_BITMAP* menus::cursor_bitmap = NULL;
ALLEGRO_FONT* menus::menu_font = NULL;
ALLEGRO_COLOR menus::menu_font_color;
ALLEGRO_COLOR menus::menu_bg_color;
ALLEGRO_TIMER* menus::menu_timer = NULL;
ALLEGRO_EVENT_QUEUE* menus::menu_event_queue = NULL;
std::vector<mnu::menu_sptr> menus::_menus = {};
std::stack<mnu::menu_csptr> menus::opened_menus = {};
float menus::menu_width = 500, menus::menu_height = 400, menus::menu_padding = 32;
int menus::font_size = 8;
bool menus::select_menu_option = false;
bool menus::is_button_left = true;
std::string menus::menu_font_file = "";
int menus::menu_font_size = 0;
int menus::menu_font_flags = 0;
int64_t menus::last_tick = 0;
bool menus::do_render = false;

/*
 *
 */
menus::menus() {
    _menus.clear();
    opened_menus = {};
}

/*
 *
 */
menus::~menus() {
    opened_menus = {};
    _menus.clear();
}

/*
 *
 */
void menus::initialize(void) {
    if(menu_font_file.empty()) menu_font = al_create_builtin_font();
    else {
        menu_font = al_load_font(menu_font_file.c_str(), menu_font_size, menu_font_flags);
        if(!menu_font) menu_font = al_create_builtin_font();
    }
    if(!menu_font) throw std::runtime_error("Unable to set font for menus!");

    //  Create the main menu.
    if(!new_menu(mnu::menu("main_menu", "Main Menu")))
        throw std::runtime_error("Unable to create main menu!");

    //  Create the in-game menu.
    if(!new_menu(mnu::menu("game_menu", "Game Menu")))
        throw std::runtime_error("Unable to create game menu!");

    //  Set font size.
    font_size = al_get_font_line_height(menu_font);

    //  Create the menu cursor.
    cursor_bitmap = al_create_bitmap(font_size, font_size);

    //  Create the the menu bitmap for rendering.
    mgr::assets::secret_load<al_bitmap>(
        "wte_menu_bitmap",
        mgr::renderer::get_arena_width(),
        mgr::renderer::get_arena_height(),
        true
    );
    al_set_target_bitmap(**mgr::assets::secret_get<al_bitmap>("wte_menu_bitmap"));
    al_clear_to_color(WTE_COLOR_TRANSPARENT);

    //  Create timer & its queue.
    menu_timer = al_create_timer(1.0f / 30.0f);
    if(!menu_timer) throw std::runtime_error("Failed to create menu timer!");
    menu_event_queue = al_create_event_queue();
    if(!menu_event_queue) throw std::runtime_error("Failed to create menu event queue!");
    al_register_event_source(menu_event_queue, al_get_timer_event_source(menu_timer));
    al_set_timer_count(menu_timer, 0);
}

/*
 *
 */
void menus::de_init(void) {
    al_destroy_bitmap(cursor_bitmap);
    al_destroy_font(menu_font);
    al_destroy_event_queue(menu_event_queue);
    al_destroy_timer(menu_timer);
}

/*
 *
 */
void menus::generate_menus(void) {
    for(auto& it: _menus) it->render();
}

/*
 *
 */
void menus::reset(void) {
    opened_menus = {};
    config::flags::game_menu_opened = false;
}

/*
 *
 */
void menus::open_menu(const std::string& menu_id) {
    opened_menus.push(get_menu(menu_id));
    config::flags::game_menu_opened = true;
    menu_position = opened_menus.top()->items_cbegin();

    //  Set default values for any menu settings objects.
    for(auto it = opened_menus.top()->items_cbegin(); it != opened_menus.top()->items_cend(); it++) {
        (*it)->set_default();
    }
}

/*
 *
 */
void menus::close_menu(void) {
    al_stop_timer(menu_timer);
    al_set_timer_count(menu_timer, 0);
    opened_menus.pop();
    if(opened_menus.empty()) config::flags::game_menu_opened = false;
    else menu_position = opened_menus.top()->items_cbegin();
}

/*
 *
 */
void menus::menu_pos_up(void) {
    if(menu_position != opened_menus.top()->items_cbegin()) menu_position--;
}

/*
 *
 */
void menus::menu_pos_down(void) {
    if(menu_position != --opened_menus.top()->items_cend()) menu_position++;
}

/*
 *
 */
void menus::menu_pos_start_left(void) {
    if(menu_position != opened_menus.top()->items_cend()) {
        is_button_left = true;
        select_menu_option = true;
        last_tick = al_get_timer_count(menu_timer);
    }
}

/*
 *
 */
void menus::menu_pos_stop_left(void) { select_menu_option = false; }

/*
 *
 */
void menus::menu_pos_start_right(void) {
    if(menu_position != opened_menus.top()->items_cend()) {
        is_button_left = false;
        select_menu_option = true;
        last_tick = al_get_timer_count(menu_timer);
    }
}

/*
 *
 */
void menus::menu_pos_stop_right(void) { select_menu_option = false; }

/*
 *
 */
void menus::menu_item_select(void) {
    if(menu_position != opened_menus.top()->items_cend()) (*menu_position)->on_select();
}

/*
 *
 */
void menus::run(void) {
    if(opened_menus.empty()) {
        //  No menus currently opened, add one to the stack.
        if(config::flags::game_started) open_menu("game_menu");  //  Add the in-game menu to the stack.
        else open_menu("main_menu");  //  Add the main menu to the stack.
    }
    if(!al_get_timer_started(menu_timer)) al_resume_timer(menu_timer);  //  Make sure timer is running.

    ALLEGRO_EVENT event;
    const bool queue_not_empty = al_get_next_event(menu_event_queue, &event);
    if(queue_not_empty && event.type == ALLEGRO_EVENT_TIMER) {
        do_render = true;

        if(select_menu_option) {
            bool toggle_menu_item = false;

            if(al_get_timer_count(menu_timer) == last_tick + 1) {
                toggle_menu_item = true;
            }
            if(al_get_timer_count(menu_timer) >= last_tick + 20) {
                if(al_get_timer_count(menu_timer) >= last_tick + 70) toggle_menu_item = true;
                else if(al_get_timer_count(menu_timer) % 5 == 0) toggle_menu_item = true;
            }

            if(toggle_menu_item) {
                if(is_button_left) (*menu_position)->on_left();
                else (*menu_position)->on_right();
            }
        }
        //  End menu item processing.
    }
}

/*
 *
 */
ALLEGRO_BITMAP* menus::render_menu(void) {
    if(do_render) {
        //  Set drawing to the menu bitmap.
        al_set_target_bitmap(**mgr::assets::secret_get<al_bitmap>("wte_menu_bitmap"));
        al_clear_to_color(menu_bg_color);

        //  Render menu title.
        al_draw_text(menu_font, menu_font_color, menu_width / 2, menu_padding,
                     ALLEGRO_ALIGN_CENTER, opened_menus.top()->get_title().c_str());

        //  Render menu items.
        float cursor_pos = 10.0;
        float vpart = 0.0, hpart = 0.0, offset = 0.0;
        std::size_t vcounter = 0;

        offset = menu_padding + font_size + menu_padding;
        vpart = (menu_height - offset) / (opened_menus.top()->num_items() + 1);
        for(auto it = opened_menus.top()->items_cbegin(); it != opened_menus.top()->items_cend(); it++) {
            vcounter++;
            hpart = menu_width / ((*it)->get_text().size() + 1);
            for(std::size_t i = 0; i < (*it)->get_text().size(); i++)
                al_draw_text(menu_font, menu_font_color, hpart * (i + 1), (offset / 2) + (vpart * vcounter),
                            ALLEGRO_ALIGN_CENTER, (*it)->get_text()[i].c_str());
            if(it == menu_position) cursor_pos = (offset / 2) + (vpart * vcounter);
        }

        //  Render menu cursor.
        if(opened_menus.top()->num_items() != 0) al_draw_bitmap(cursor_bitmap, menu_padding, cursor_pos, 0);
        do_render = false;
    }

    return **mgr::assets::secret_get<al_bitmap>("wte_menu_bitmap");
}

}  // end namespace mgr

}  // end namespace wte