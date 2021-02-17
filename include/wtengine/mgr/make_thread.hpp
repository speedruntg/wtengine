/*!
 * WTEngine | File:  make_thread.hpp
 * 
 * \author Matthew Evans
 * \version 0.1a
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2020
 */

#ifndef WTE_MAKE_THREAD_HPP
#define WTE_MAKE_THREAD_HPP

#include <thread>
#include <future>
#include <chrono>

namespace wte
{

namespace mgr
{

/*!
 * \class make_thread
 * \brief Extend this to create a threaded object.
 * 
 * Has a virtual run member that is defined in the inheriting class.
 * Calling start runs the "run" member in a detached thread.
 * Inside the run member, is_running can be called to see if the thread is
 * considered active.  Call the stop member to end the thread.
 */
class make_thread {
    public:
        /*!
         * \brief Calls stop member if started flag is true.
         */
        virtual ~make_thread() {
            if(started) stop();
        };

        //!  Remove copy constructor
        make_thread(const make_thread&) = delete;
        //!  Remove assignment operator
        void operator=(make_thread const&) = delete;

        /*!
         * \brief Call to start execution of thread.
         */
        void start(void) {
            if(started == true) return;
            exit_state = exit_signal.get_future();
            std::thread th([&]() { run(); });
            th.detach();
            started = true;
        };

        /*!
         * \brief Call to end thread.
         */
        void stop(void) {
            exit_signal.set_value();
            started = false;
            exit_signal = std::promise<void>();
        };

        /*!
         * \brief Check if thread is running.
         */
        const bool is_running(void) const {
            return started;
        };

    protected:
        /*!
         * \brief Set started flag to false.
         */
        make_thread() : started(false) {};

        /*!
         * \brief Call this within run() to check if the thread is running.
         */
        bool keep_running(void) {
            if(exit_state.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout)
                return true;
            return false;
        };

        /*!
         * \brief Override this in extended class to implement a thread.
         */
        virtual void run(void) = 0;

    private:
        //  Call to trigger exit
        std::promise<void> exit_signal;
        //  Track exit signal state
        std::future<void> exit_state;

        //  Flag to track status of thread
        bool started;
};

} //  end namespace mgr

} //  end namespace wte

#endif