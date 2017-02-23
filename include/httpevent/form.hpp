#ifndef FORM_HPP
#define FORM_HPP

#include "libpoco.hpp"
#include "libevent2.hpp"
#include "head.hpp"
#include "body.h"
#include "cookie.hpp"
#include "request.hpp"
#include "upload_handler.hpp"




namespace httpevent {

    class form : public kvmap {
    public:
        form() = delete;

        form(const request& req, upload_handler* handler = 0) :
        kvmap()
        , head_data(req)
        , body_data(req)
        , cookies_data() {
            this->parse_cookies();
            this->parse(req.get_method(), req.get_uri(), handler);
        }
        virtual ~form() = default;

        form(const form& other) :
        kvmap(other)
        , head_data(other.head_data)
        , body_data(other.body_data)
        , cookies_data(other.cookies_data) {
        }

        form& operator=(const form& right) {
            if (this == &right) {
                return *this;
            } else {
                kvmap::operator=(right);
                this->head_data = right.head_data;
                this->body_data = right.body_data;
                this->cookies_data = right.cookies_data;
            }

            return *this;
        }



    public:

        const head& get_head()const {
            return this->head_data;
        }

        const body& get_body()const {
            return this->body_data;
        }

        const cookies& get_cookies() const {
            return this->cookies_data;
        }
    private:

        void parse_cookies() {
            httpevent::parse_cookie(this->head_data, this->cookies_data);
        }

        void parse(const std::string& method, const std::string& uri, httpevent::upload_handler* handler = 0) {
            Poco::Net::HTTPRequest poco_req;
            poco_req.setMethod(method);
            poco_req.setURI(uri);
            poco_req.setVersion(Poco::Net::HTTPRequest::HTTP_1_1);
            if (this->head_data.find("Content-Type") != this->head_data.end()) {
                poco_req.setContentType(this->head_data.at("Content-Type").convert<std::string>());
            }

            Poco::Net::HTMLForm* poco_form = 0;
            if (method == "GET") {
                poco_form = new Poco::Net::HTMLForm(poco_req);
            } else if (method == "POST") {
                const std::string& BODY = this->body_data.get();
                std::istringstream IS(BODY);
                if (BODY.find("filename") == std::string::npos) {
                    poco_form = new Poco::Net::HTMLForm(poco_req, IS);
                } else if (handler) {
                    poco_form = new Poco::Net::HTMLForm(poco_req, IS, *handler);
                    auto result = handler->get_data();
                    for (auto & item : result) {
                        if (item.ok) {
                            kvmap::operator[](item.name) = item.savepath;
                        }
                    }
                }
            }
            if (poco_form) {
                for (auto & item : *poco_form) {
                    kvmap::operator[](item.first) = item.second;
                }
                delete poco_form;
            }
        }
    private:
        httpevent::head head_data;
        httpevent::body body_data;
        httpevent::cookies cookies_data;

    };
}

#endif /* FORM_HPP */

