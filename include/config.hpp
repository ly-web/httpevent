#ifndef CONFIG_HPP
#define CONFIG_HPP


#include "httpevent.hpp"
#include "spdlog/spdlog.h"
#include "filter.hpp"
#include "route.hpp"
#include "plugin.hpp"
#include "session.hpp"
#include "cache.hpp"


static const char* LAST_MODIFIED_HEAD = "Last-Modified";
static const char* IF_MODIFIED_SINCE_HEAD = "If-Modified-Since";
static const char* ETAG_HEAD = "Etag";
static const char* ETAG_VALUE = "HEHE";
static const char* IF_NONE_MATCH_HEAD = "If-None-Match";
static const char* REFERER_HEAD = "Referer";
static const char* SERVER_HEAD = "Server";
static const char* X_POWERED_BY_HEAD = "X-Powered-By";
static const char* CACHE_CONTROL_HEAD = "Cache-Control";
static const char* CONTENT_TYPE_HEAD = "Content-Type";

static const char* HTTPERR_404 = "Not Found";
static const char* HTTPERR_403 = "Forbidden";
static const char* HTTPERR_405 = "Method Not Allowed";
static const char* HTTP_STATUS_304 = "Not Modified";
static const char* HTTP_STATUS_200 = "OK";
static const char* HTTPERR_5xx = "Server Error";
static const char* SESSION_ID_KEY = "HTTPEVENTSESSIONID";

static std::string HOST;
static std::string DEFAULT_CONTENT_TYPE;
static std::string SERVER_NAME;
static std::string X_Powered_By;
static std::string REAL_IP_HEAD;
static std::string MATCH_HOTLINKING;
static std::string DOC_DIRECTORY;
static std::string IP_DENY_FILE;
static std::string ROUTE_FILE;
static std::string PLUGIN_DIRECTORY;
static std::string LOG_DIRECTORY;
static std::string CERT_CERTIFICATE_FILE;
static std::string CERT_PRIVATE_KEY_FILE;
static std::string LUA_DIRECTORY;
static std::string MIME_SRC_FILE;


static int SERVER_TYPE;
static int PORT;
static int TIMEOUT;
static int LOG_FILE_SIZE;
static int LOG_PURGE_COUNT;
static int HTTP_EXPIRES;


static long IP_MAX_ACCESS_COUNT;
static long IP_DENY_EXPIRE;
static long IP_ACCESS_INTERVAl;
static long CACHE_EXPIRES;
static long CACHE_CLIENT_EXPIRES;
static long SESSION_EXPIRES;
static long UPDATE_INTERVAL;


static bool HTTP_PROXY_USED;
static bool ENABLE_HOTLINKING;
static bool IP_ENABLE_CHECK;
static bool ENABLE_SSL;
static bool ENABLE_LOGGER;


static std::map<std::string, std::string> MIME_SRC;
//static std::map<std::string, httpevent::view*> HANDLER;
static Poco::Util::LayeredConfiguration* CONFIG = 0;
static std::map<std::string, std::map<std::string, std::string>> CONFIG_MAP;
static Poco::ClassLoader<httpevent::view>* CLASS_LOADER = 0;
static httpevent::plugin* PLUGIN = 0;
static httpevent::filter* FILTER = 0;
static httpevent::route* ROUTER = 0;
static httpevent::session* SESSION = 0;
static httpevent::cache* CACHE = 0;
static spdlog::logger* LOGGER = 0;
static Poco::MD5Engine MD5_ENGINE;
static kaguya::State LUA_STATE;

typedef void (*CB_FUNC)(struct evhttp_request *, void *);
static CB_FUNC CB = 0;
static struct event_base *BASE = 0;
static struct evhttp *SERVER = 0;
static SSL_CTX *CTX = 0;
static EC_KEY *ECDH = 0;



#endif /* CONFIG_HPP */

