#ifndef HEAD_HPP
#define HEAD_HPP


#include "libpoco.hpp"
#include "libevent2.hpp"
#include "kvmap.hpp"
#include "request.hpp"


namespace httpevent {

    class head : public kvmap {
    public:
        head() = delete;

        head(const httpevent::request& req) :
        kvmap(), req(req.req) {
            this->parse();
        }
        virtual ~head() = default;

        head(const head& other) :
        kvmap(other), req(other.req) {
        }

        head& operator=(const head& right) {
            if (this == &right) {
                return *this;
            } else {
                kvmap::operator=(right);
                this->req = right.req;
            }
            return *this;
        }


    private:

        void parse() {
            struct evkeyvalq *headers = evhttp_request_get_input_headers(this->req);
            for (struct evkeyval *header = headers->tqh_first; header; header = header->next.tqe_next) {
                kvmap::operator[](header->key) = std::string(header->value);
            }
        }

    private:
        struct evhttp_request* req;
    };
}


#endif /* HEAD_HPP */

