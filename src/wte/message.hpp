/*
  WTEngine
  By:  Matthew Evans
  File:  message.hpp

  See LICENSE.txt for copyright information

  Define the message object
*/

#ifndef WTE_MSG_MESSAGE_HPP
#define WTE_MSG_MESSAGE_HPP

#include <string>
#include <sstream>
#include <vector>

namespace wte
{

typedef std::vector<std::string> msg_arg_list;

//! Message class
/*!
  Define individual message objects
*/
class message {
    public:
        inline message() {};
        inline ~message() {};

        //!  Create a non-timed message
        inline message(std::string s, std::string c, std::string a) :
                timer(-1), sys(s), to(""), from(""), cmd(c), args(a) {
            std::stringstream arg_stream(args);
            std::string segment;

            while(std::getline(arg_stream, segment, ';')) {
                arglist.push_back(segment);
            }
        };

        //!  Create a timed message
        inline message(int64_t e, std::string s, std::string c, std::string a) :
                timer(e), sys(s), to(""), from(""), cmd(c), args(a) {
            std::stringstream arg_stream(args);
            std::string segment;

            while(std::getline(arg_stream, segment, ';')) {
                arglist.push_back(segment);
            }
        };

        //!  Create a non-timed message with a to & from
        inline message(std::string s, std::string t, std::string f, std::string c, std::string a) :
                timer(-1), sys(s), to(t), from(f), cmd(c), args(a) {
            std::stringstream arg_stream(args);
            std::string segment;

            while(std::getline(arg_stream, segment, ';')) {
                arglist.push_back(segment);
            }
        };

        //!  Create a timed message with a to & from
        inline message(int64_t e, std::string s, std::string t, std::string f, std::string c, std::string a) :
                timer(e), sys(s), to(t), from(f), cmd(c), args(a) {
            std::stringstream arg_stream(args);
            std::string segment;

            while(std::getline(arg_stream, segment, ';')) {
                arglist.push_back(segment);
            }
        };

        //!  Return the value of timer
        inline const int64_t get_timer(void) const { return timer; };
        //!  Return the value of sys
        inline const std::string get_sys(void) const { return sys; };
        //!  Return the value of to
        inline const std::string get_to(void) const { return to; };
        //!  Return the value of from
        inline const std::string get_from(void) const { return from; };
        //!  Return the value of cmd
        inline const std::string get_cmd(void) const { return cmd; };
        //!  Return the value of args
        inline const std::string get_args(void) const { return args; };

        //!  Split the args up into a vector of strings
        //!  Returns a vector of the arguments, delimited by ;
        inline const msg_arg_list get_split_args(void) const { return arglist; }

        //!  Get a single argument
        //!  Returns a string by index from the argument list, delimited by ;
        inline const std::string get_split_args(const std::size_t pos) const {
            if(pos > arglist.size()-1 || pos < 0) return "";  //  Out of range, return empty string
            return arglist[pos];
        }

        //!  Check if the event is synced to the timer
        //!  Returns false if the timer value is -1
        inline const bool is_timed_event(void) const {
            if(timer == -1) return false;
            else return true;
        }

        //!  Used to sort by timer value
        bool operator<(const message& a) const {
            return timer < a.timer;
        }

    private:
        int64_t timer;      //  Timer value that the message will be processed at
        std::string sys;    //  System that will process the message
        std::string to;     //  Message to entity field
        std::string from;   //  Message from entity field
        std::string cmd;    //  Message command
        std::string args;   //  Message arguments
        msg_arg_list arglist;
};

} //  namespace wte

#endif
