#ifndef REQUEST_HPP
#define REQUEST_HPP


#include "libpoco.hpp"
#include "libevent2.hpp"

namespace httpevent {
    class head;
    class body;
    class form;

    class request {
    public:
        friend class httpevent::head;
        friend class httpevent::body;
        friend class httpevent::form;
    public:

        request() = delete;

        request(struct evhttp_request *req) :
        client()
        , method()
        , uri()
        , user_agent()
        , req(req) {
            this->client = this->req->remote_host;

            this->parse_method();

            this->uri = evhttp_request_get_uri(this->req);
            

            const char* tmp = evhttp_find_header(req->input_headers, "User-Agent");
            this->user_agent = tmp ? tmp : "xxx";
        }

        request(const request& other) :
        client(other.client)
        , method(other.method)
        , uri(other.uri)
        , req(other.req) {
        }

        request& operator=(const request& right) {
            if (this == &right) {
                return *this;
            } else {
                this->client = right.client;
                this->method = right.method;
                this->uri = right.uri;
                this->req = right.req;
            }

            return *this;
        }

        virtual ~request() = default;
    public:

        const std::string& get_client()const {
            return this->client;
        }

        const std::string& get_method()const {
            return this->method;
        }

        const std::string& get_uri()const {
            return this->uri;
        }

        const std::string& get_user_agent()const {
            return this->user_agent;
        }
    private:

        void parse_method() {
            enum evhttp_cmd_type req_method = evhttp_request_get_command(this->req);
            switch (req_method) {
                case EVHTTP_REQ_GET:
                    this->method = "GET";
                    break;
                case EVHTTP_REQ_POST:
                    this->method = "POST";
                    break;
                case EVHTTP_REQ_HEAD:
                    this->method = "HEAD";
                    break;
                case EVHTTP_REQ_DELETE:
                    this->method = "DELETE";
                    break;
                case EVHTTP_REQ_PUT:
                    this->method = "PUT";
                    break;
                default:
                    this->method = "unknown";
                    break;
            }
        }

    private:
        std::string client, method, uri, user_agent;
        struct evhttp_request * req;

    };

}



#endif /* REQUEST_HPP */

