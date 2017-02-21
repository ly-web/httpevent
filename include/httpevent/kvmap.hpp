#ifndef KVMAP_HPP
#define KVMAP_HPP


#include <map>
#include <string>
#include <Poco/DynamicAny.h>

namespace httpevent {
    typedef std::map<std::string, Poco::DynamicAny> kvmap;
}


#endif /* KVMAP_HPP */

