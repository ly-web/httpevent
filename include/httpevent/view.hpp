#ifndef VIEW_HPP
#define VIEW_HPP


#include "request.hpp"
#include "response.hpp"
#include "kaguya/kaguya.hpp"

namespace httpevent {

    class view {
    public:

        view()
        : route_data(0)
        , cookie_data(0)
        , session_data(0)
        , lua_state(0) {

        }

        virtual~view() = default;
    public:
        virtual void handler(const request& req, response& res) = 0;

        std::vector<std::string> *route_data;
        httpevent::cookies *cookie_data;
        std::map<std::string, std::string>* session_data;
        kaguya::State * lua_state;
    };
}

#endif /* VIEW_HPP */

