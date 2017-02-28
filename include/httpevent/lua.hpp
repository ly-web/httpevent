#ifndef LUA_HPP
#define LUA_HPP

#include "view.hpp"
#include "upload_handler.hpp"
#include "lua_tool.hpp"

namespace httpevent {

    class lua : public view {
    public:

        lua()
        : httpevent::view()
        , lua_directory(Poco::Util::Application::instance().config().getString("http.luaDirectory", "/var/httpevent/lua")) {

        }

        virtual~lua() = default;
    private:
        const std::string lua_directory;
    public:

        void handler(const request& req, response& res) {
            Poco::File mod_script(this->lua_directory + "/" + (*this->route_data)[1] + ".lua");
            if (mod_script.exists()) {
                if (mod_script.canExecute()) {
                    Poco::File script(this->lua_directory + "/index.lua");
                    if (script.exists()) {
                        if (script.canExecute() && this->lua_state) {

                            kaguya::State &state = *this->lua_state;

                            httpevent::lua_tool::cookie_tool::register_cookie_tool_class(state);
                            httpevent::lua_tool::cookie_tool cookie_tool_instance(&res, this->cookie_data);
                            state["httpevent"]["cookie_tool"] = &cookie_tool_instance;


                            httpevent::lua_tool::session_tool::register_session_tool_class(state);
                            httpevent::lua_tool::session_tool session_tool_instance(this->session_data);
                            state["httpevent"]["session_tool"] = &session_tool_instance;


                            httpevent::lua_tool::util_tool::register_util_tool_class(state);
                            httpevent::lua_tool::util_tool util_tool_instance(&res);
                            state["httpevent"]["util_tool"] = &util_tool_instance;


                            httpevent::upload_handler upload_handler("upload");
                            httpevent::form form(req, &upload_handler);
                            httpevent::lua_tool::form_tool::register_form_tool_class(state);
                            httpevent::lua_tool::form_tool form_tool_instance(&form);
                            state["httpevent"]["form_tool"] = &form_tool_instance;

                            std::string err_msg;
                            state.setErrorHandler([&](int code, const char* msg) {
                                err_msg = msg;
                            });
                            state.dofile(script.path());
                            if (!err_msg.empty()) {
                                res.send_body(err_msg).submit();
                            }
                            state.garbageCollect();

                        } else {
                            res.error(403, "Forbidden");
                        }
                    } else {
                        res.error(404, "Not found");
                    }
                } else {
                    res.error(403, "Forbidden");
                }
            } else {
                res.error(404, "Not found");
            }

        }

    };
}

#endif /* LUA_HPP */

