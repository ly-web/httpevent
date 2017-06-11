#ifndef LUA_TOOL_HPP
#define LUA_TOOL_HPP

#include "response.hpp"
#include "cookie.hpp"
#include "form.hpp"
#include "kaguya.hpp"

namespace httpevent {
    namespace lua_tool {

        class cookie_tool {
        public:

            cookie_tool(httpevent::response* res, httpevent::cookies* cookie_data)
            : res(res)
            , cookie_data(cookie_data) {
            }

            void set(const std::string& k, const std::string& v, const std::string& path, int expires, bool secure) {
                httpevent::cookie cookie;
                cookie.setName(k);
                cookie.setValue(v);
                cookie.setPath(path);
                cookie.setMaxAge(expires);
                cookie.setSecure(secure);
                cookie.setHttpOnly(true);
                this->res->send_cookie(cookie);
            }

            std::string get(const std::string& k) {
                return this->cookie_data ? (*this->cookie_data)[k] : "";
            }

            bool has(const std::string& k) {
                return this->cookie_data ? this->cookie_data->find(k) != this->cookie_data->end() : false;
            }

            static void register_cookie_tool_class(kaguya::State& state) {
                if (!state["_cookie_tool_"]) {
                    state["_cookie_tool_"].setClass(kaguya::UserdataMetatable<httpevent::lua_tool::cookie_tool>()
                            .setConstructors < httpevent::lua_tool::cookie_tool(
                            httpevent::response*
                            , httpevent::cookies*)>()
                            .addFunction("set", &httpevent::lua_tool::cookie_tool::set)
                            .addFunction("get", &httpevent::lua_tool::cookie_tool::get)
                            .addFunction("has", &httpevent::lua_tool::cookie_tool::has)
                            );
                }
            }

        private:
            httpevent::response * res;
            httpevent::cookies* cookie_data;
        };

        class form_tool {
        public:

            form_tool(httpevent::form* form) : form_data(form) {
            }

            std::string get(const std::string& k) {
                return (*this->form_data)[k];
            }

            bool has(const std::string& k) {
                return this->form_data->find(k) != this->form_data->end();
            }

            static void register_form_tool_class(kaguya::State& state) {
                if (!state["_form_tool_"]) {
                    state["_form_tool_"].setClass(kaguya::UserdataMetatable<httpevent::lua_tool::form_tool>()
                            .setConstructors < httpevent::lua_tool::form_tool(
                            httpevent::form*)>()
                            .addFunction("get", &httpevent::lua_tool::form_tool::get)
                            .addFunction("has", &httpevent::lua_tool::form_tool::has)
                            );
                }
            }

        private:
            httpevent::form *form_data;
        };

        class session_tool {
        public:

            session_tool(std::map<std::string, std::string>* session_data)
            : session_data(session_data) {

            }

            void set(const std::string& k, const std::string& v) {
                if (this->session_data)
                    (*this->session_data)[k] = v;
            }

            std::string get(const std::string& k) {
                return this->session_data ? (*this->session_data)[k] : "";
            }

            bool has(const std::string& k) {
                return this->session_data ? this->session_data->find(k) != this->session_data->end() : false;
            }

            static void register_session_tool_class(kaguya::State& state) {
                if (!state["_session_tool_"]) {
                    state["_session_tool_"].setClass(kaguya::UserdataMetatable<httpevent::lua_tool::session_tool>()
                            .setConstructors < httpevent::lua_tool::session_tool(
                            std::map<std::string, std::string>*)>()
                            .addFunction("set", &httpevent::lua_tool::session_tool::set)
                            .addFunction("get", &httpevent::lua_tool::session_tool::get)
                            .addFunction("has", &httpevent::lua_tool::session_tool::has)
                            );
                }
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

            void submit(int code, const std::string& text) {
                this->res->submit(code, text);
            }

            static void register_util_tool_class(kaguya::State& state) {
                if (!state["_util_tool_"]) {
                    state["_util_tool_"].setClass(kaguya::UserdataMetatable<httpevent::lua_tool::util_tool>()
                            .setConstructors < httpevent::lua_tool::util_tool(
                            httpevent::response*)>()
                            .addFunction("redirect", &httpevent::lua_tool::util_tool::redirect)
                            .addFunction("error", &httpevent::lua_tool::util_tool::error)
                            .addFunction("submit", &httpevent::lua_tool::util_tool::submit)
                            );
                }
            }

        private:
            httpevent::response* res;
        };
    }
}

#endif /* LUA_TOOL_HPP */

