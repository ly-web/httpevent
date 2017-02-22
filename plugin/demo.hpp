#ifndef DEMO_HPP
#define DEMO_HPP

#include "../include/httpevent.hpp"

namespace httpevent {

    class hello : public view {

        void handler(const request& req, response& res) {
            res.send_head("Content-Type", "text/plain;charset=UTF-8")
                    .send_body("hello,world").submit();
        }

    };

    class lua : public view {
    public:

        void add_lua_method() {
            kaguya::State &state = *(this->lua_state);
            if (!state["LUA_TEST"]) {
                state["LUA_TEST"].setClass(kaguya::UserdataMetatable<httpevent::lua>()
                        .setConstructors < httpevent::lua()>()
                        .addFunction("add", &httpevent::lua::add)
                        .addFunction("strstr", &httpevent::lua::strstr)
                        .addFunction("loop", &httpevent::lua::loop)
                        );
            }
            state["httpevent"]["lua_test"] = this;

        }

        void handler(const request& req, response& res) {
            Poco::Util::LayeredConfiguration& config = Poco::Util::Application::instance().config();
            Poco::URI uri(req.get_uri());
            Poco::File script(config.getString("http.luaDirectory", "/var/httpevent/lua") + uri.getPath());
            if (script.exists()) {
                if (script.canExecute()) {
                    this->add_lua_method();
                    httpevent::form form(req);
                    std::map<std::string, std::string> form_data;
                    for (auto & item : form) {
                        form_data[item.first] = item.second.convert<std::string>();
                    }
                    (*this->lua_state)["httpevent"]["form_data"] = form_data;
                    this->lua_state->dofile(script.path());
                } else {
                    res.error(403, "Forbidden");
                }
            } else {
                res.error(404, "Not found");
            }

        }

        int add(int a, int b) {
            return a + b;
        }

        std::string strstr(const std::string& a, const std::string& b) {
            return a + b;
        }

        int loop(int a) {
            int A = a;
            while (A > 0) {
                --A;
            }
            return A;
        }

    };

    class upload : public view {

        void handler(const request& req, response& res) {
            httpevent::upload_handler handler("upload");
            httpevent::form form(req, &handler);
            auto result = handler.get_data();
            for (auto & item : result) {
                if (item.ok) {
                    res.send_body("/upload" + item.webpath);
                } else {
                    res.send_body(item.message);
                }
            }
            res.submit();
        }

    };

    class data : public view {

        void handler(const request& req, response& res) {
            res.send_head("Content-Type", "text/plain;charset=UTF-8")
                    .send_body("route_data:\r\n");
            for (auto & item : * this->route_data) {
                res.send_body(item).send_body("\r\n");
            }
            if (this->cookie_data) {
                res.send_body("cookie_data:\r\n");
                for (auto& item : * this->cookie_data) {
                    res.send_body(item.first + ":\t" + item.second.convert<std::string>() + "\r\n");
                }
            }
            if (this->session_data) {
                res.send_body("session variable\r\n");
                if (this->session_data->find("TEST") == this->session_data->end()) {
                    this->session_data->insert(std::make_pair("TEST", 0));
                } else {
                    ++this->session_data->at("TEST");
                }
                res.send_body("TEST:\t" + this->session_data->at("TEST").convert<std::string>())
                        .submit();
            }
        }

    };

    class redirect : public view {

        void handler(const request& req, response& res) {
            res.redirect("/");
        }

    };

    class loop : public view {

        void handler(const request& req, response& res) {
            res.send_head("Content-Type", "text/plain;charset=UTF-8");
            int foo = 10000;
            while (foo > 0) {
                --foo;
            }
            res.send_body(Poco::format("%d", foo)).submit();
        }

    };
}


#endif /* DEMO_HPP */

