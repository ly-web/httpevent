#ifndef BODY_H
#define BODY_H

#include "libpoco.hpp"
#include "libevent2.hpp"

#include "request.hpp"


namespace httpevent {

    class body {
    public:
        body() = delete;

        body(const httpevent::request& req) : req(req.req), data() {
            this->parse();
        }
        virtual ~body() = default;

        body(const body& other) :
        req(other.req), data(other.data) {

        }

        body& operator=(const body& right) {
            if (this == &right) {
                return *this;
            } else {
                this->req = right.req;
                this->data = right.data;
            }

            return *this;
        }

        const std::string& get()const {
            return this->data;
        }
    private:

        void parse() {
            struct evbuffer *buf = evhttp_request_get_input_buffer(this->req);
            std::size_t buf_size = evbuffer_get_length(buf);
            if (buf_size) {
                char buf_data [buf_size + 1];
                ev_ssize_t n = evbuffer_copyout(buf, buf_data, buf_size + 1);
                if (n >= 0) {
                    this->data.assign(buf_data, n);
                }
            }
        }
    private:
        struct evhttp_request* req;
        std::string data;
    };
}

#endif /* BODY_H */

