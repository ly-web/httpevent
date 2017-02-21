#ifndef COOKIE_HPP
#define COOKIE_HPP

#include "libpoco.hpp"
#include "libevent2.hpp"

#include "head.hpp"

namespace httpevent {
    typedef Poco::Net::HTTPCookie cookie;
    typedef kvmap cookies;

    static void parse_cookie(const head& header, cookies& cookies) {
        if (header.find("Cookie") != header.end()) {
            std::string cookie_string = "cooke;" + header.at("Cookie").convert<std::string>(), tmp;
            Poco::Net::NameValueCollection nvc;
            Poco::Net::HTTPMessage::splitParameters(cookie_string, tmp, nvc);
            for (auto & item : nvc) {
                cookies[item.first] = item.second;
            }
        }
    }

}

#endif /* COOKIE_HPP */

