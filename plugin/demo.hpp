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
            if (this->session_data) {
                res.send_body("session variable\r\n");
                if (this->session_data->find("TEST") == this->session_data->end()) {
                    this->session_data->insert(std::make_pair("TEST", "0"));
                } else {
                    (*this->session_data)["TEST"] = this->inc((*this->session_data)["TEST"]);
                }
                res.send_body("TEST:\t" + this->session_data->at("TEST"))
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

