#ifndef SESSION_HPP
#define SESSION_HPP

#include <string>
#include <map>
#include <Poco/DynamicAny.h>
#include <Poco/ExpireCache.h>

namespace httpevent {
    typedef Poco::ExpireCache<std::string, std::map<std::string, Poco::DynamicAny>> session;


}

#endif /* SESSION_HPP */

