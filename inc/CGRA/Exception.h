/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <cstdlib>
#include <exception>
#include <sstream>
#include <stdexcept>
#include <string>

struct cgrame_error : public std::runtime_error
{
    cgrame_error(std::string message)
        : std::runtime_error((message.insert(0,"[ERROR] "), std::move(message)))
    {}
};

struct cgrame_mapper_error : public cgrame_error
{
    cgrame_mapper_error(std::string message)
        : cgrame_error((message.insert(0,"[MAPPER] "), std::move(message)))
    {}
};

struct cgrame_visual_error : public cgrame_error
{
    cgrame_visual_error(std::string message)
        : cgrame_error((message.insert(0,"[VISUAL] "), std::move(message)))
    {}
};

struct cgrame_adl_error : public cgrame_error
{
    cgrame_adl_error(std::string message)
        : cgrame_error((message.insert(0,"[ADL] "), std::move(message)))
    {}
};

struct cgrame_model_error : public cgrame_error
{
    cgrame_model_error(std::string message)
        : cgrame_error((message.insert(0,"[MODEL] "), std::move(message)))
    {}
};

/**
 * A convenience function for throwing exceptions. Lets you do this:
 *
 * int x = computation()
 * if (x != 0) {
 *     make_and_throw([&](auto&& s) {
 *         s << "x was not zero! it was " << x;
 *     });
 * }
 */
template<typename EXCEPTION, typename FUNC>
[[noreturn]] void make_and_throw(FUNC func) {
    std::ostringstream os;
    func(os);
    throw EXCEPTION(os.str());
}

/**
 * A convenience function for constructing things that expect strings,
 * but when you would like to use a stream.
 * Such as throwing exceptions. Lets you do this:
 *
 * int x = computation()
 * if (x != 0) {
 *     throw make_from_stream<std::logic_error>([&](auto&& s) {
 *         s << "x was not zero! it was " << x;
 *     });
 * }
 */
template<typename T, typename FUNC>
auto make_from_stream(FUNC func) {
    std::ostringstream os;
    func(os);
    return T(os.str());
}

/**
 * Runs the given functor on a stringstream, and retuns the string
 */
template<typename F>
std::string string_from_stream(F&& f) {
    std::ostringstream stream;
    f(stream);
    return stream.str();
}

/**
 * Print the given @p eptr to @p os, reasonably safely
 * Intended for use in a destructor or something, when an throw exception cannot be tolerated.
 * Does not handle std::nested_exception specially.
 * Will not allocate, except if @p os does
 */
inline void tryToSafelyPrintExceptionInfo(std::exception_ptr eptr, std::ostream& os, const char* action_name) noexcept {
    try {
        if (eptr) {
            std::rethrow_exception(eptr);
        }
    } catch (const std::exception& e) {
        try {
            os << "std::exception thrown when " << action_name << ": .what() = " << e.what() << std::endl;
        } catch (...) {
            std::fprintf(stderr, "exception encountered when handling std::exception when %s", action_name);
            std::fflush(stderr);
        }
    } catch (...) {
        try {
            os << "unknown exception thrown when " << action_name << std::endl;
        } catch (...) {
            std::fprintf(stderr, "exception encountered when handling unknown exception when %s", action_name);
            std::fflush(stderr);
        }
    }
}

/**
 * Prints a message to both stderr and stdout derived from @p eptr
 * Intended to be used when an exception is going to be ignored.
 * Handles std::nested_exception properly, and will print messages from the nested exceptions.
 */
inline void printExceptionToOutAndErr(std::exception_ptr eptr, const char* line_prefix, const char* message) {
    std::stringstream msg;
    //msg
        //<< line_prefix << '\n'
        //<< line_prefix << ' ' << message << ". Exception chain:\n"
    //;

    int level = 0;
    while (eptr) {
        msg << line_prefix; for (int i = 0; i < level + 2; ++i) msg << ' '; // indentation

        // add a message based on the type of exception
        try {
            std::rethrow_exception(eptr);
        } catch (const std::exception& e) {
            msg <</* ".what(): " <<*/ e.what() << '\n';
        } catch (...) {
            msg << "Unknown exception\n";
        }

        // determine if we should keep looping
        try {
            std::rethrow_exception(eptr);
        } catch (const std::nested_exception& as_ne) {
            try {
                as_ne.rethrow_nested();
            } catch (...) {
                eptr = std::current_exception(); // move on to the nested exception
            }
        } catch (...) {
            eptr = nullptr; // stop looping
        }

        level += 1;
    }

    //msg << line_prefix;

    //fprintf(stdout, "[stdout]\n%s\n", msg.str().c_str());
    //fflush(stdout);
    fprintf(stderr, "[stderr]\n%s\n", msg.str().c_str());
    fflush(stderr);
}


#endif
