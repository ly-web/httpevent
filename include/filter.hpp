#ifndef FILTER_HPP
#define FILTER_HPP

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <Poco/ExpireCache.h>
#include <Poco/RegularExpression.h>
#include <Poco/File.h>

namespace httpevent {

    class filter {
    public:
        filter() = delete;

        filter(long ipDenyExpire, long ipAccessInterval, const std::string& ipDenyFile = "")
        : ipDenyExpire(ipDenyExpire)
        , ipAccessInterval(ipAccessInterval)
        , cache(ipAccessInterval)
        , blackIp(ipDenyExpire)
        , denyIp()
        , denyPath(ipDenyFile) {
            this->init();
        }
        filter(const filter&) = delete;

        virtual ~filter() {
            this->cache.clear();
            this->blackIp.clear();
            this->denyIp.clear();
        }
    public:

        bool deny(const std::string& ip, int ipMaxAccessCount) {
            bool deny = true;
            if (this->blackIp.has(ip)) {
                return deny;
            }

            if (!this->cache.has(ip)) {
                this->cache.add(ip, std::make_pair(1, Poco::LocalDateTime().timestamp().epochTime()));
                deny = false;
            } else {
                auto old = this->cache.get(ip);
                time_t tdiff = Poco::LocalDateTime().timestamp().epochTime() - old->second;
                if (old->first >= ipMaxAccessCount && tdiff <= this->ipAccessInterval) {
                    this->blackIp.add(ip, 0);
                } else {
                    this->cache.update(ip, std::make_pair(old->first + 1, old->second));
                    deny = false;
                }
            }
            return deny;
        }

        bool kill(const std::string& ip) {
            for (auto& item : this->denyIp) {
                if (Poco::RegularExpression::match(ip, item)) {
                    return true;
                }
            }
            return false;
            //return std::find(this->denyIp.begin(), this->denyIp.end(), ip) != this->denyIp.end();
        }

        void update() {
            this->denyIp.clear();
            this->init();
        }
    private:

        void init() {
            if (!this->denyPath.empty()) {
                Poco::File file(this->denyPath);
                if (file.exists() && file.canRead()) {
                    std::ifstream input(this->denyPath);
                    if (input) {
                        std::string ip;
                        while (std::getline(input, ip)) {
                            this->denyIp.push_back(ip);
                        }
                    }
                }
            }
        }
    private:
        time_t ipDenyExpire, ipAccessInterval;
        Poco::ExpireCache<std::string, std::pair<long, time_t> > cache;
        Poco::ExpireCache<std::string, int> blackIp;
        std::vector<std::string> denyIp;
        std::string denyPath;
    };

}

#endif /* FILTER_HPP */

