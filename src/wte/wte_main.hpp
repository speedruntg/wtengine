/*!
 * \brief WTEngine | File:  wte_main.hpp
 * \author Matthew Evans
 *
 * \version 0.1a
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2020
 *
 * \details The main engine object.
 */

#ifndef WTE_MAIN_HPP
#define WTE_MAIN_HPP

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_physfs.h>
#include <physfs.h>

#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

#include "wte_global_defines.hpp"
#include "engine_cfg_map.hpp"
#include "game_cfg_map.hpp"
#include "engine_flags.hpp"
#include "alert.hpp"
#include "mgr/engine_time.hpp"
#include "mgr/managers.hpp"

namespace wte
{

//! WTEngine Main Class.
/*!
 * The main engine object.
 * Sets up various system objects used by the engine.
 * Contains the main game loop and members for managing the game and engine.
 */
class wte_main {
    public:
        /*!
         * Frees up instance, set initialized flag to false.
         * Also makes sure to unload the engine.
         */
        inline ~wte_main() {
            if(load_called == true) wte_unload();

            PHYSFS_deinit();

            al_destroy_timer(main_timer);
            al_destroy_event_queue(main_queue);
            al_destroy_display(display);
            al_uninstall_system();

            initialized = false;
        };

        //!  Remove copy constructor.
        wte_main(const wte_main&) = delete;
        //!  Remove assignment operator.
        void operator=(wte_main const&) = delete;

        /*!
         * Call first to load the engine.
         */
        inline void wte_load(void) {
            //  Start the input & audio threads.
            input_th.start();
            audio_th.start();

            //  Initialize renderer and menu manager.
            screen.initialize(al_create_builtin_font());
            menus.initialize(al_create_builtin_font(), WTE_COLOR_WHITE, WTE_COLOR_DARKPURPLE);

            //  Load user configured menus.
            load_menus();

            //  Init done, set flag to true.
            load_called = true;
        };

        /*!
         * Call to unload the engine.
         */
        inline void wte_unload(void) {
            input_th.stop();
            audio_th.stop();

            load_called = false;
        };

        /*!
         * Add file path to provide to PhysFS.
         */
        inline static void add_file_location(const std::string flocation) {
            file_locations.push_back(flocation);
        };

        //!  Call to start up the main game loop.
        void do_game(void);

    protected:
        /*!
         * Force single instance, set initialized flag to true.
         * Throws a runtime error if another instance is called.
         */
        inline wte_main(const int argc, char **argv, const std::string title) : window_title(title), load_called(false) {
            if(initialized == true) throw std::runtime_error(window_title + " already running!");
            initialized = true;

            //  Initialize Allegro.
            if(!al_init()) throw std::runtime_error("Allegro failed to load!");

            //  Initialize additional Allegro components.
            if(!al_install_keyboard()) throw std::runtime_error("Failed to detect keyboard!");
            if(!al_init_image_addon()) throw std::runtime_error("Failed to load Allegro image addon!");
            if(!al_init_font_addon()) throw std::runtime_error("Failed to load Allegro font addon!");
            al_install_joystick();

            //  Configure PhysFS.
            if(!PHYSFS_init(argv[0])) throw std::runtime_error("Failed to load PhysFS!");
            if(file_locations.empty()) throw std::runtime_error("Need to configure locations for PhysFS!");
            for(auto & it : file_locations) PHYSFS_mount(it.c_str(), NULL, 1);
            al_set_physfs_file_interface();

            //  Configure display.
            if(!engine_cfg::is_reg("screen_width")) throw std::runtime_error("Screen width not set!");
            if(!engine_cfg::is_reg("screen_height")) throw std::runtime_error("Screen height not set!");
            display = al_create_display(engine_cfg::get<int>("screen_width"),
                                        engine_cfg::get<int>("screen_height"));
            if(!display) throw std::runtime_error("Failed to configure display!");
            al_set_window_title(display, window_title.c_str());

            //  Configure main timer.
            main_timer = al_create_timer(1.0 / WTE_TICKS_PER_SECOND);
            if(!main_timer) throw std::runtime_error("Failed to create timer!");

            //  Configure main event queue.
            main_queue = al_create_event_queue();
            if(!main_queue) throw std::runtime_error("Failed to create event queue!");

            //  Register event sources.
            al_register_event_source(main_queue, al_get_display_event_source(display));
            al_register_event_source(main_queue, al_get_timer_event_source(main_timer));

            //  Map commands to enums for switching over in the system msg handler.
            map_cmd_str_values["exit"] = CMD_STR_EXIT;
            map_cmd_str_values["alert"] = CMD_STR_ALERT;
            map_cmd_str_values["new_game"] = CMD_STR_NEW_GAME;
            map_cmd_str_values["end_game"] = CMD_STR_END_GAME;
            map_cmd_str_values["open_menu"] = CMD_STR_OPEN_MENU;
            map_cmd_str_values["close_menu"] = CMD_STR_CLOSE_MENU;
            map_cmd_str_values["enable_system"] = CMD_STR_ENABLE_SYSTEM;
            map_cmd_str_values["disable_system"] = CMD_STR_DISABLE_SYSTEM;
            map_cmd_str_values["set_engcfg"] = CMD_STR_SET_ENGCFG;
            map_cmd_str_values["set_gamecfg"] = CMD_STR_SET_GAMECFG;
            map_cmd_str_values["fps_counter"] = CMD_STR_FPS_COUNTER;

            //  Set default colors for alerts.
            alert::set_font_color(WTE_COLOR_WHITE);
            alert::set_bg_color(WTE_COLOR_RED);
        };

        /* These function members are overridden in the derived class */
        //!  Define this to implement custom menu layout.
        virtual void load_menus(void) = 0;
        //!  Define this to load all systems to be used by the game.
        virtual void load_systems(void) = 0;
        //!  Define what gets loaded when a game starts.
        virtual void new_game(void) = 0;
        //!  Define what happens at the end of a game.
        virtual void end_game(void) = 0;
        //!  Optional:  On menu open.
        virtual void on_menu_open(void) {};
        //!  Optional:  On menu close.
        virtual void on_menu_close(void) {};
        //!  Optional:  Define custom system message handling.
        virtual void handle_custom_sys_msg(const message) {};
        /* *** End overridden function members *** */

        //  Managers used by the engine declared here.
        mgr::audio_manager audio_th;
        mgr::entity_manager world;
        mgr::input_manager input_th;
        mgr::menu_manager menus;
        mgr::message_manager messages;
        mgr::render_manager screen;
        mgr::spawn_manager spawner;
        mgr::system_manager systems;

    private:
        //!  Start up a new game.
        void process_new_game(void);
        //!  End the game in progress.
        void process_end_game(void);
        //!  Process messages passed to the system.
        void handle_sys_msg(message_container);

        //  Used for mapping commands and switching on system messages:
        enum CMD_STR_VALUE {
            CMD_STR_EXIT,           CMD_STR_ALERT,
            CMD_STR_NEW_GAME,       CMD_STR_END_GAME,
            CMD_STR_OPEN_MENU,      CMD_STR_CLOSE_MENU,
            CMD_STR_ENABLE_SYSTEM,  CMD_STR_DISABLE_SYSTEM,
            CMD_STR_SET_ENGCFG,     CMD_STR_SET_GAMECFG,
            CMD_STR_FPS_COUNTER
        };
        std::map<std::string, CMD_STR_VALUE> map_cmd_str_values;

        //  Allegro objects used by the engine.
        inline static ALLEGRO_DISPLAY* display = NULL;
        inline static ALLEGRO_TIMER* main_timer = NULL;
        inline static ALLEGRO_EVENT_QUEUE* main_queue = NULL;
        ALLEGRO_EVENT event;

        bool load_called;  //  Flag to make sure wte_load was called.
        std::string window_title;  //  Title for application window.

        //  Vector of file paths to provide to PhysFS.
        inline static std::vector<std::string> file_locations = {};
        //  Restrict to one instance of the engine running.
        inline static bool initialized = false;
};

/*!
 * Call every time a new game is starting.
 * \param void
 * \return void
 */
inline void wte_main::process_new_game(void) {
    std::srand(std::time(nullptr));  //  Seed random.

    engine_flags::unset(GAME_MENU_OPENED);

    //  Load a new message data file.
    messages.new_data_file("game.sdf");  //  Update later to load from settings.

    //  Load systems and prevent further systems from being loaded.
    load_systems();
    systems.finalize();

    //  Clear world and load starting entities.
    world.clear();
    new_game();

    //  WIP
    messages.add_message(message("audio", "null", "test"));

    //  Restart the timer at zero.
    al_stop_timer(main_timer);
    al_set_timer_count(main_timer, 0);
    engine_flags::set(GAME_STARTED);
    al_start_timer(main_timer);
    mgr::engine_time::set_time(al_get_timer_count(main_timer));
}

/*!
 * Calls the user defined end game process, then shuts down the game.
 * \param void
 * \return void
 */
inline void wte_main::process_end_game(void) {
    al_stop_timer(main_timer);
    engine_flags::unset(GAME_STARTED);
    al_set_timer_count(main_timer, 0);
    mgr::engine_time::set_time(al_get_timer_count(main_timer));

    //  WIP
    messages.add_message(message("audio", "null", "test"));

    //  Call end game process
    end_game();
    world.clear();
    systems.clear();

    engine_flags::set(GAME_MENU_OPENED);
}

/*!
 * The main game loop.
 * \param void
 * \return void
 */
inline void wte_main::do_game(void) {
    if(load_called == false) wte_load();  //  Auto call load.

    message_container temp_msgs;

    messages.clear_queue();

    engine_flags::set(IS_RUNNING);
    engine_flags::unset(GAME_STARTED);
    engine_flags::set(GAME_MENU_OPENED);

    //  test code.
    engine_flags::set(DRAW_FPS);
    //  end test code.

    while(engine_flags::is_set(IS_RUNNING)) {
        //  Pause / resume timer depending on if the game menu is opened.
        //  Also process the on_menu events.
        if(engine_flags::is_set(GAME_MENU_OPENED) && al_get_timer_started(main_timer)) {
            al_stop_timer(main_timer);
            on_menu_open();
        }
        if(!engine_flags::is_set(GAME_MENU_OPENED) && !al_get_timer_started(main_timer)) {
            on_menu_close();
            al_resume_timer(main_timer);
        }

        //  Game not running, make sure the timer isn't and force the menu manager.
        if(!engine_flags::is_set(GAME_STARTED)) {
            al_stop_timer(main_timer);
            engine_flags::set(GAME_MENU_OPENED);
        }
        //  Game menu is opened, run the menu manager
        if(engine_flags::is_set(GAME_MENU_OPENED)) menus.run(messages);

        /* *** GAME LOOP ************************************************************ */
        //  Capture event from queue.
        const bool queue_not_empty = al_get_next_event(main_queue, &event);
        //  Call our game logic update on timer events.  Timer is only running when the game is running.
        if(event.type == ALLEGRO_EVENT_TIMER && queue_not_empty) {
            const int64_t the_time = al_get_timer_count(main_timer);
            //  Set the engine_time object to the current time.
            mgr::engine_time::set_time(the_time);

            //  Run all systems.
            systems.run(world, messages, the_time);
            //  Process messages.
            systems.dispatch(world, messages);

            //  Get any spawner messages and pass to handler.
            temp_msgs = messages.get_messages("spawner");
            if(!temp_msgs.empty()) spawner.process(temp_msgs, world);
        }
        /* *** END GAME LOOP ******************************************************** */

        //  Render the screen.
        screen.render(menus, world);

        //  Send audio messages to the audio queue.
        temp_msgs = messages.get_messages("audio");
        if(!temp_msgs.empty()) audio_th.transfer_messages(temp_msgs);

        //  Get any system messages and pass to handler.
        temp_msgs = messages.get_messages("system");
        if(!temp_msgs.empty()) handle_sys_msg(temp_msgs);

        //  Ignore message pruning if WTE_NO_PRUNE build flag is defined.
        #ifndef WTE_NO_PRUNE
        //  Delete messages that were not processed.
        messages.prune();
        #endif

        //  Force quit if the game window is closed.
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            if(engine_flags::is_set(GAME_STARTED)) process_end_game();
            engine_flags::unset(IS_RUNNING);
        }
    }
}

/*!
 * Switch over the system messages and process.
 * Remaining messages are passed to the custom handler.
 * \param void
 * \return void
 */
inline void wte_main::handle_sys_msg(message_container sys_msgs) {
    for(auto it = sys_msgs.begin(); it != sys_msgs.end();) {
        //  Switch over the system messages, deleting each as they are processed.
        switch(map_cmd_str_values[it->get_cmd()]) {
            //  cmd:  exit - Shut down engine.
            case CMD_STR_EXIT:
                if(engine_flags::is_set(GAME_STARTED)) process_end_game();
                engine_flags::unset(IS_RUNNING);
                it = sys_msgs.erase(it);
                break;

            //  cmd:  alert - Display an alert.
            case CMD_STR_ALERT:
                alert::set_alert(it->get_arg(0));
                it = sys_msgs.erase(it);
                break;

            //  cmd:  new_game - start up a new game.
            case CMD_STR_NEW_GAME:
                //  If the game is running, ignore.
                if(!engine_flags::is_set(GAME_STARTED)) {
                    menus.reset();
                    process_new_game();
                }
                it = sys_msgs.erase(it);
                break;

            //  cmd:  end_game - end current game.
            case CMD_STR_END_GAME:
                //  If the game not is running, ignore.
                if(engine_flags::is_set(GAME_STARTED)) {
                    process_end_game();
                    menus.reset();
                }
                it = sys_msgs.erase(it);
                break;

            //  cmd:  open_menu argstring - open a menu, passing a string as an argument.
            //  If the menu doesn't exist, the default will be opened.
            case CMD_STR_OPEN_MENU:
                menus.open_menu(it->get_arg(0));
                it = sys_msgs.erase(it);
                break;

            //  cmd:  close_menu argstring - close the opened menu.
            //  If argstring = "all", close all opened menus.
            case CMD_STR_CLOSE_MENU:
                if(it->get_arg(0) == "all") menus.reset();
                else menus.close_menu();
                it = sys_msgs.erase(it);
                break;

            //  cmd:  enable_system - Turn a system on for processing.
            case CMD_STR_ENABLE_SYSTEM:
                systems.enable_system(it->get_arg(0));
                it = sys_msgs.erase(it);
                break;

            //  cmd:  disable_system - Turn a system off so it's run member is skipped.
            //  Message dispatching is still processed.
            case CMD_STR_DISABLE_SYSTEM:
                systems.disable_system(it->get_arg(0));
                it = sys_msgs.erase(it);
                break;

            //  cmd:  set_engcfg - Set engine cfg variable.
            case CMD_STR_SET_ENGCFG:
                for(std::size_t i = 0; i < it->num_args(); i++)
                    engine_cfg::set(it->get_arg(i));
                it = sys_msgs.erase(it);
                break;

            //  cmd:  set_gamecfg - Set game cfg variable.
            case CMD_STR_SET_GAMECFG:
                for(std::size_t i = 0; i < it->num_args(); i++)
                    game_cfg::set(it->get_arg(i));
                it = sys_msgs.erase(it);
                break;

            //  cmd:  fps_counter - Enable/disable on-screen fps counter.
            case CMD_STR_FPS_COUNTER:
                if(it->get_arg(0) == "on") engine_flags::set(DRAW_FPS);
                if(it->get_arg(0) == "off") engine_flags::unset(DRAW_FPS);
                it = sys_msgs.erase(it);
                break;

            //  cmd:  new_cmd - description
            //case CMD_STR_X:
                //
                //it = sys_msgs.erase(it);
                //break;

            //  Command not defined, iterate to next.
            default:
                it++;
        }
    }

    //  Pass remaining system messages to the custom handler.
    if(!sys_msgs.empty()) for(auto & it : sys_msgs) handle_custom_sys_msg(it);
}

} //  end namespace wte

#endif