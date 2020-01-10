/*
  WTEngine
  By:  Matthew Evans
  File:  wte_main.hpp

  See LICENSE.txt for copyright information

  The main engine class.  Extend this to create your game loop.
*/

#ifndef WTE_MAIN_HPP
#define WTE_MAIN_HPP

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
//#include <allegro5/allegro_acodec.h>
//#include <allegro5/allegro_memfile.h>

#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

#include "wte_globals.hpp"
#include "menu_manager.hpp"
#include "entity_manager.hpp"
#include "system_manager.hpp"
#include "audio_manager.hpp"
#include "message_queue.hpp"
#include "renderer.hpp"
#include "get_input.hpp"

namespace wte
{

//! wte_main class
/*!
  Main engine object
*/
class wte_main {
    public:
        int wte_init(void);                     /*!< Initialize the engine */
        void wte_unload(void);                  /*!< Unload the engine */
        void do_game(void);                     /*!< Run the game loop */

    private:
        void handle_sys_msg(msg::message_container);
        
        ALLEGRO_DISPLAY *display;               /*!< Display to draw to */
        ALLEGRO_TIMER *main_timer;              /*!< Timer to control game loop */
        ALLEGRO_EVENT_QUEUE *main_queue;        /*!< Main event queue */
        ALLEGRO_EVENT event;                    /*!< Container to fetch event */

        renderer game_screen;                   /*!< The renderer used to draw the game environment */

    protected:
        virtual void load_systems(void) = 0;    /*!< Override to load custom systems */
        virtual void load_game(void) = 0;       /*!< Override to load initial entities */
        virtual void end_game(void) = 0;        /*!< Override to define end game process */

        void generate_new_game(std::string);    /*!< Call to generate a new game */
        virtual void handle_custom_sys_msg(msg::message_container) {};
        
        ecs::entity_manager world;              /*!< Manager for entities */
        ecs::system_manager systems;            /*!< Manager for systems */
        msg::message_queue messages;            /*!< Message queue */
        mnu::menu_manager menus;
};

//! Initialize WTEngine
/*!
  Register everything for the engine to run
*/
inline int wte_main::wte_init(void) {
    //  Initialize Allegro and it's various objects
    if(!al_init()) return -1; //  Allegro didn't load - Exit

    display = al_create_display(768, 1024);
    if(!display) return -2; //  Failed to set display - Exit

    main_timer = al_create_timer(1.0 / TICKS_PER_SECOND);
    if(!main_timer) return -3; //  Failed to create timer - Exit

    main_queue = al_create_event_queue();
    if(!main_queue) return -4; //  Failed to create event queue - Exit

    //  Initialize additional Allegro components
    al_install_keyboard();
    al_install_joystick();
    al_init_image_addon();
    al_init_font_addon();
    al_install_audio();
    //al_init_acodec_addon();

    //  Register event sources
    al_register_event_source(main_queue, al_get_display_event_source(display));
    al_register_event_source(main_queue, al_get_timer_event_source(main_timer));

    //  Start the input thread as detached
    al_run_detached_thread(get_input, NULL);

    //  Start the audio thread as detached
    al_run_detached_thread(audio_manager, NULL);

    //  Load user configured systems
    load_systems();

    //  Load menu manager
    menus = mnu::menu_manager(al_create_builtin_font());

    //  Create render object
    game_screen = renderer(al_create_builtin_font());

    //  Blank the screen
    al_clear_to_color(WTE_COLOR_BLACK);
    al_flip_display();

    return 0;
}

//! Unload WTEngine
/*!
  Shut down the various Allegro objects
*/
inline void wte_main::wte_unload(void) {
    al_destroy_timer(main_timer);
    al_destroy_event_queue(main_queue);
    al_destroy_display(display);
    al_uninstall_system();
}

//! Call to generate a new game
/*!
  Call every time a new game is starting
*/
inline void wte_main::generate_new_game(std::string message_data) {
    end_game(); //  Call end game to clear any previously running game

    std::srand(std::time(nullptr));

    game_flag[GAME_STARTED] = true; //  Set game flag

    //  Create a new entity manager
    world = ecs::entity_manager();
    load_game();

    //  Create a new message queue
    messages = msg::message_queue();
    messages.new_data_file(message_data);

    //  Restart the timer at zero
    al_stop_timer(main_timer);
    al_set_timer_count(main_timer, 0);
    al_start_timer(main_timer);
}

//! Main game logic
/*!
  The main game loop
*/
inline void wte_main::do_game(void) {
    bool queue_not_empty;
    msg::message_container temp_msgs;

    generate_new_game("data\\game.sdf"); //  test code

    while(game_flag[IS_RUNNING]) {
        if(key[KEY_FIRE_3]) generate_new_game("data\\game.sdf"); //  test code to restart game

        //  Pause / resume timer depending on if the game menu is opened
        if(game_flag[GAME_MENU_OPENED] && al_get_timer_started(main_timer)) {
            menus.reset();
            al_stop_timer(main_timer);
        }
        if(!game_flag[GAME_MENU_OPENED] && !al_get_timer_started(main_timer)) {
            menus.reset();
            al_resume_timer(main_timer);
        }

        //  Game menu is opened, run the menu manager
        if(game_flag[GAME_MENU_OPENED]) menus.run(messages);

        /* *** GAME LOOP ************************************************************ */
        //  Capture event from queue
        queue_not_empty = al_get_next_event(main_queue, &event);
        //  Call our game logic update on timer events.  Timer is only running when the game is running.
        if(event.type == ALLEGRO_EVENT_TIMER && queue_not_empty) {
            //  Set the message queue's internal timer to the current time
            messages.set_time(al_get_timer_count(main_timer));
            //  Run all systems
            systems.run(world, messages, al_get_timer_count(main_timer));
            //  Process messages
            systems.dispatch(world, messages);
        }
        /* *** END GAME LOOP ******************************************************** */

        //  Render the screen
        game_screen.render(menus, world, al_get_timer_count(main_timer));

        //  Send audio messages to the audio queue
        temp_msgs = messages.get_messages("audio");
        audio_messages.insert(audio_messages.end(),
                              std::make_move_iterator(temp_msgs.begin()),
                              std::make_move_iterator(temp_msgs.end()));

        //  Get any system messages and pass to handler
        temp_msgs = messages.get_messages("system");
        if(!temp_msgs.empty()) handle_sys_msg(temp_msgs);

        //  Force quit if the game window is closed
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) game_flag[IS_RUNNING] = false;
    }
}

//! Process system messages
/*!
  ...
*/
inline void wte_main::handle_sys_msg(msg::message_container sys_msgs) {
    //
    handle_custom_sys_msg(sys_msgs);
}

} //  end namespace wte

#endif
