/*!
 * wtengine | File:  wte_exceptions.hpp
 * 
 * \author Matthew Evans
 * \version 0.7.2
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2022
 */

#ifndef WTE_EXCEPTION_HPP
#define WTE_EXCEPTION_HPP

#include <string>
#include <fstream>
#include <exception>

#include "wtengine/_globals/_defines.hpp"
#include "wtengine/_globals/engine_time.hpp"

namespace wte {

class logger;

class exception_item final {
    friend class logger;
    friend class runtime_error;
    friend class exception;

    public:
        inline exception_item(const std::string& d, const std::string& l, const uint& c) :
            description(d.c_str()), location(l.c_str()), code(c), time(engine_time::check()) {};

    private:
        const char* description;  //  Exception description.
        const char* location;     //  Exception location.
        const uint& code;
        const int64_t time;       //  Time of exception.
};

/*!
 * \class runtime_error
 * \brief Throws an internal engine exception that terminates the application.
 */
class runtime_error final : public std::exception {
    public:
        inline runtime_error(const exception_item& i) : item(i) {
            if constexpr (build_options.debug_mode) log_exception(item);
        };

        runtime_error() = delete;    //!<  Delete default constructor.
        ~runtime_error() = default;  //!<  Default destructor.

        /*!
         * \brief Returns the description of the thrown exception.
         * \return Description of thrown exception.
         */
        const char* what() const noexcept override { return item.description; };

        /*!
         * \brief Return the location the exception occured.
         * \return Location of thrown exception.
         */
        const char* where() const noexcept{ return item.location; };

        /*!
         * \brief Return the time the exception occured.
         * \return Time of thrown exception.
         */
        const int64_t when() const noexcept { return item.time; };

    private:
        const exception_item item;
        //  Log exception to file when debugging is enabled.
        inline void log_exception(const exception_item& item) {};
};

/*!
 * \class exception
 * \brief Throws an internal engine exception.
 *
 * Exceptions thrown this way will be displayed as an alert in the engine. \n
 * If debugging is enabled, they will also be logged to file.
 */
class exception final : public std::exception {
    public:
        /*!
         * \brief Create a wte exception.
         * \param desc Description of thrown exception.
         * \param loc Location exception was thrown.
         * \param t Time exception was thrown.
         */
        inline exception(const exception_item& i) : item(i) {
            //if constexpr (build_options.debug_mode) log_exception(i);
        };

        exception() = delete;    //!<  Delete default constructor.
        ~exception() = default;  //!<  Default destructor.

        /*!
         * \brief Returns the description of the thrown exception.
         * \return Description of thrown exception.
         */
        const char* what() const noexcept override { return item.description; };

        /*!
         * \brief Return the location the exception occured.
         * \return Location of thrown exception.
         */
        const char* where() const noexcept{ return item.location; };

        /*!
         * \brief Return the time the exception occured.
         * \return Time of thrown exception.
         */
        const int64_t when() const noexcept { return item.time; };

    private:
        const exception_item item;
        //  Log exception to file when debugging is enabled.
        inline void log_exception() {
            //logger::add(item);
        };
};

}  //  end namespace wte

#endif