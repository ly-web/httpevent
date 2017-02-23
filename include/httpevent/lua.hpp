#ifndef LUA_HPP
#define LUA_HPP

#include "view.hpp"
#include "upload_handler.hpp"

namespace httpevent {

    class lua : public view {
    public:

        lua()
        : httpevent::view(), lua_directory(Poco::Util::Application::instance().config().getString("http.luaDirectory", "/var/httpevent/lua")) {

        }

        virtual~lua() = default;
    private:

        class lua_tool {
        public:

            lua_tool(httpevent::response* res
                    , httpevent::cookies* cookie_data
                    , std::map<std::string, Poco::DynamicAny>* session_data)
            : res(res)
            , cookie_data(cookie_data)
            , session_data(session_data) {
            }

            void set_cookie(const std::string& k, const std::string& v, const std::string& path, int expires, bool secure) {
                httpevent::cookie cookie;
                cookie.setName(k);
                cookie.setValue(v);
                cookie.setPath(path);
                cookie.setMaxAge(expires);
                cookie.setSecure(secure);
                this->res->send_cookie(cookie);
            }

            std::string get_cookie(const std::string& k) {
                return (*this->cookie_data)[k];
            }

            void set_session(const std::string& k, const std::string& v) {
                (*this->session_data)[k] = v;
            }

            std::string get_session(const std::string& k) {
                return (*this->session_data)[k].toString();
            }

            void redirect(const std::string& uri, int code) {
                this->res->redirect(uri, code);
            }

            void error(int code, const std::string& text) {
                this->res->error(code, text);
            }


        private:
            httpevent::response* res;
            httpevent::cookies* cookie_data;
            std::map<std::string, Poco::DynamicAny>* session_data;
        };

        void register_custom_class(kaguya::State& state) {
            if (!state["LUA_TOOL"]) {
                state["LUA_TOOL"].setClass(kaguya::UserdataMetatable<httpevent::lua::lua_tool>()
                        .setConstructors < httpevent::lua(httpevent::response*
                        , httpevent::cookies*
                        , std::map<std::string, Poco::DynamicAny>*)>()
                        .addFunction("set_cookie", &httpevent::lua::lua_tool::set_cookie)
                        .addFunction("get_cookie", &httpevent::lua::lua_tool::get_cookie)
                        .addFunction("set_session", &httpevent::lua::lua_tool::set_session)
                        .addFunction("get_session", &httpevent::lua::lua_tool::get_session)
                        .addFunction("redirect", &httpevent::lua::lua_tool::redirect)
                        .addFunction("error", &httpevent::lua::lua_tool::error)
                        );
            }
        }
        std::string lua_directory;
    public:

        void handler(const request& req, response& res) {
            Poco::URI uri(req.get_uri());
            Poco::File script(this->lua_directory + uri.getPath());
            if (script.exists()) {
                if (script.canExecute()) {
                    kaguya::State &state = *this->lua_state;
                    this->register_custom_class(state);

                    httpevent::lua::lua_tool lua_tool_instance(&res, this->cookie_data, this->session_data);
                    state["httpevent"]["lua_tool"] = &lua_tool_instance;


                    std::map<std::string, std::string> form_data;
                    httpevent::upload_handler upload_handler("upload");
                    httpevent::form form(req, &upload_handler);
                    auto upload_result = upload_handler.get_data();
                    for (auto & item : form) {
                        form_data[item.first] = item.second.toString();
                    }
                    for (auto & item : upload_result) {
                        if (item.ok) {
                            form_data[item.name] = item.webpath;
                        }
                    }
                    state["httpevent"]["FORM"] = form_data;


                    std::map<std::string, std::string> cookies_data;
                    for (auto& item : * this->cookie_data) {
                        cookies_data[item.first] = item.second.toString();
                    }
                    state["httpevent"]["COOKIE"] = cookies_data;


                    std::string err_msg;
                    state.setErrorHandler([&](int code, const char* msg) {
                        err_msg = msg;
                    });
                    state.dofile(script.path());
                    if (!err_msg.empty()) {
                        res.send_body(err_msg).submit();
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

