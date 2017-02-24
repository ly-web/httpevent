#ifndef LUA_HPP
#define LUA_HPP

#include "view.hpp"
#include "upload_handler.hpp"
#include "form.hpp"

namespace httpevent {

    class lua : public view {
    public:

        lua()
        : httpevent::view()
        , lua_directory(Poco::Util::Application::instance().config().getString("http.luaDirectory", "/var/httpevent/lua")) {

        }

        virtual~lua() = default;
    private:

        class cookie_tool {
        public:

            cookie_tool(httpevent::response* res, httpevent::cookies* cookie_data)
            : res(res)
            , cookie_data(cookie_data) {
            }

            void set_cookie(const std::string& k, const std::string& v, const std::string& path, int expires, bool secure) {
                httpevent::cookie cookie;
                cookie.setName(k);
                cookie.setValue(v);
                cookie.setPath(path);
                cookie.setMaxAge(expires);
                cookie.setSecure(secure);
                cookie.setHttpOnly(true);
                this->res->send_cookie(cookie);
            }

            std::string get_cookie(const std::string& k) {
                return this->cookie_data->at(k);
            }

            bool has_cookie(const std::string& k) {
                return this->cookie_data->find(k) != this->cookie_data->end();
            }
        private:
            httpevent::response * res;
            httpevent::cookies* cookie_data;
        };

        class form_tool {
        public:

            form_tool(httpevent::form* form_data) : form_data(form_data) {
            }

            std::string get_form(const std::string& k) {
                return this->form_data->at(k);
            }

            bool has_form(const std::string& k) {
                return this->form_data->find(k) != this->form_data->end();
            }

        private:
            httpevent::form* form_data;
        };

        class session_tool {
        public:

            session_tool(std::map<std::string, std::string>* session_data)
            : session_data(session_data) {

            }

            void set_session(const std::string& k, const std::string& v) {
                (*this->session_data)[k] = v;
            }

            std::string get_session(const std::string& k) {
                return this->session_data->at(k);
            }

            bool has_session(const std::string& k) {
                return this->session_data->find(k) != this->session_data->end();
            }

        private:
            std::map<std::string, std::string>* session_data;
        };

        class util_tool {
        public:

            util_tool(httpevent::response* res)
            : res(res) {
            }

            void redirect(const std::string& uri, int code) {
                this->res->redirect(uri, code);
            }

            void error(int code, const std::string& text) {
                this->res->error(code, text);
            }


        private:
            httpevent::response* res;
        };

        void register_form_tool_class(kaguya::State& state) {
            if (!state["_FORM_TOOL_"]) {
                state["_FORM_TOOL_"].setClass(kaguya::UserdataMetatable<httpevent::lua::form_tool>()
                        .setConstructors < httpevent::lua::form_tool(
                        httpevent::form *)>()
                        .addFunction("get_form", &httpevent::lua::form_tool::get_form)
                        .addFunction("has_form", &httpevent::lua::form_tool::has_form)
                        );
            }
        }

        void register_session_tool_class(kaguya::State& state) {
            if (!state["_SESSION_TOOL_"]) {
                state["_SESSION_TOOL_"].setClass(kaguya::UserdataMetatable<httpevent::lua::session_tool>()
                        .setConstructors < httpevent::lua::session_tool(
                        std::map<std::string, std::string>*)>()
                        .addFunction("set_session", &httpevent::lua::session_tool::set_session)
                        .addFunction("get_session", &httpevent::lua::session_tool::get_session)
                        .addFunction("has_session", &httpevent::lua::session_tool::has_session)
                        );
            }
        }

        void register_util_tool_class(kaguya::State& state) {
            if (!state["_UTIL_TOOL_"]) {
                state["_UTIL_TOOL_"].setClass(kaguya::UserdataMetatable<httpevent::lua::util_tool>()
                        .setConstructors < httpevent::lua::util_tool(
                        httpevent::response*)>()
                        .addFunction("redirect", &httpevent::lua::util_tool::redirect)
                        .addFunction("error", &httpevent::lua::util_tool::error)
                        );
            }
        }

        void register_cookie_tool_class(kaguya::State& state) {
            if (!state["_COOKIE_TOOL_"]) {
                state["_COOKIE_TOOL_"].setClass(kaguya::UserdataMetatable<httpevent::lua::cookie_tool>()
                        .setConstructors < httpevent::lua::cookie_tool(
                        httpevent::response*
                        , httpevent::cookies*)>()
                        .addFunction("set_cookie", &httpevent::lua::cookie_tool::set_cookie)
                        .addFunction("get_cookie", &httpevent::lua::cookie_tool::get_cookie)
                        .addFunction("has_cookie", &httpevent::lua::cookie_tool::has_cookie)
                        );
            }
        }
        const std::string lua_directory;
    public:

        void handler(const request& req, response& res) {
            Poco::URI uri(req.get_uri());
            Poco::File script(this->lua_directory + uri.getPath());
            if (script.exists()) {
                if (script.canExecute() && this->lua_state) {
                    kaguya::State &state = *this->lua_state;
                    httpevent::lua::cookie_tool *cookie_tool_instance = 0;
                    if (this->cookie_data) {
                        this->register_cookie_tool_class(state);
                        cookie_tool_instance = new httpevent::lua::cookie_tool(&res, this->cookie_data);
                        state["httpevent"]["cookie_tool"] = cookie_tool_instance;
                    }
                    httpevent::lua::session_tool* session_tool_instance = 0;
                    if (this->session_data) {
                        this->register_session_tool_class(state);
                        session_tool_instance = new httpevent::lua::session_tool(this->session_data);
                        state["httpevent"]["session_tool"] = session_tool_instance;
                    }

                    this->register_util_tool_class(state);
                    httpevent::lua::util_tool util_tool_instance(&res);
                    state["httpevent"]["util_tool"] = &util_tool_instance;

                    httpevent::upload_handler upload_handler("upload");
                    httpevent::form form(req, &upload_handler);
                    auto upload_result = upload_handler.get_data();
                    for (auto & item : upload_result) {
                        if (item.ok) {
                            form[item.name] = item.webpath;
                        }
                    }

                    this->register_form_tool_class(state);
                    httpevent::lua::form_tool form_tool_instance(&form);
                    state["httpevent"]["form_tool"] = &form_tool_instance;


                    std::string err_msg;
                    state.setErrorHandler([&](int code, const char* msg) {
                        err_msg = msg;
                    });
                    state.dofile(script.path());
                    if (!err_msg.empty()) {
                        res.send_body(err_msg).submit();
                    }
                    if (cookie_tool_instance) {
                        delete cookie_tool_instance;
                    }
                    if (session_tool_instance) {
                        delete session_tool_instance;
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

