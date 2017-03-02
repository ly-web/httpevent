#ifndef DEMO_HPP
#define DEMO_HPP

#include "../include/httpevent.hpp"

namespace httpevent {

    class hello : public view {

        void handler(const request& req, response& res) {
            res.send_head("Content-Type", "text/plain;charset=UTF-8")
                    .send_body("hello,world");
        }

    };

    class aform : public view {

        void handler(const request& req, response& res) {
            httpevent::upload_handler upload_handler("upload");
            httpevent::form form(req, &upload_handler);
            res.send_head("Content-Type", "text/plain;charset=UTF-8");
            for (auto& item : form) {
                res.send_body(item.first + "=" + item.second + "\n");
            }
            res.submit();
        }

    };

    class upload : public view {

        void handler(const request& req, response& res) {
            httpevent::upload_handler upload_handler("upload");
            httpevent::form form(req, &upload_handler);
            auto result = upload_handler.get_data();
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
    public:

        void handler(const request& req, response& res) {
            res.send_head("Content-Type", "text/plain;charset=UTF-8")
                    .send_body("route_data:\r\n");
            for (auto & item : * this->route_data) {
                res.send_body(item).send_body("\r\n");
            }
            if (this->cookie_data) {
                res.send_body("cookie_data:\r\n");
                for (auto& item : * this->cookie_data) {
                    res.send_body(item.first + ":\t" + item.second + "\r\n");
                }
            }

            std::string session_value = "0", session_key = "TEST";
            if (this->session_data) {
                res.send_body("session variable\r\n");
                if (this->session_data->find(session_key) == this->session_data->end()) {
                    this->session_data->insert(std::make_pair(session_key, session_value));
                } else {
                    session_value = (*this->session_data)[session_key];
                    (*this->session_data)[session_key] = this->inc(session_value);
                }
                res.send_body(session_key + "\t=" + session_value)
                        .submit();
            }
        }
    private:

        std::string inc(const std::string & num) {
            int n = Poco::NumberParser::parse(num);
            return Poco::NumberFormatter::format(++n);
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

