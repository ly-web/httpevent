#ifndef CACHE_HPP
#define CACHE_HPP

#include <string>
#include <Poco/ExpireLRUCache.h>

namespace httpevent {
    typedef Poco::ExpireLRUCache<std::string, std::string> cache;

}



#endif /* CACHE_HPP */

