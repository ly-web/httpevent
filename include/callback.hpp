#ifndef CALLBACK_HPP
#define CALLBACK_HPP


#include "config.hpp"

static void init_lua_state() {
    LUA_STATE["_response_"].setClass(
            kaguya::UserdataMetatable<httpevent::response>()
            .setConstructors < httpevent::response(struct evhttp_request *)>()
            .addFunction("send_body", &httpevent::response::send_body)
            .addFunction("send_head", &httpevent::response::send_head)
            );
    LUA_STATE["_request_"].setClass(
            kaguya::UserdataMetatable<httpevent::request>()
            .setConstructors < httpevent::request(struct evhttp_request *)>()
            .addFunction("get_client", &httpevent::request::get_client)
            .addFunction("get_method", &httpevent::request::get_method)
            .addFunction("get_uri", &httpevent::request::get_uri)
            .addFunction("get_user_agent", &httpevent::request::get_user_agent)
            );
    LUA_STATE["httpevent"] = kaguya::NewTable();
}

static void config_lua_state(httpevent::request* req, httpevent::response* res, std::vector<std::string>& route_data) {
    LUA_STATE["httpevent"]["response"] = res;
    LUA_STATE["httpevent"]["request"] = req;
    LUA_STATE["httpevent"]["ROUTE"] = route_data;
}

static void init_mime_type(const std::string& path) {
    Poco::File src(path);
    if (src.exists() && src.canRead()) {
        std::ifstream input(path);
        if (input) {
            std::string line;
            while (std::getline(input, line)) {
                if (line.front() != '#' && !line.empty()) {
                    Poco::StringTokenizer st(line, " ", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
                    if (st.count() >= 2) {
                        for (std::size_t i = st.count() - 1; i > 0; --i) {
                            MIME_SRC[st[i]] = st[0];
                        }
                    }
                }
            }
        }
    }
}

static void update_mime_type() {
    MIME_SRC.clear();
    init_mime_type(MIME_SRC_FILE);
}

static std::string get_mime_type(const std::string& file) {
    const std::string ext = Poco::Path(file).getExtension();
    if (ext.empty() || MIME_SRC.find(ext) == MIME_SRC.end()) {
        return MIME_SRC.at("*");
    }
    return MIME_SRC.at(ext);
}

static void init_callback_config(Poco::Util::LayeredConfiguration* config) {
    SERVER_TYPE = config->getInt("http.serverType", 3);
    MIME_SRC_FILE = config->getString("http.mime", "/etc/httpevent/mime.conf");
    ENABLE_SSL = config->getBool("http.enableSSL", true);
    HOST = config->getString("http.ip", "127.0.0.1");
    PORT = config->getInt("http.port", ENABLE_SSL ? 443 : 80);
    TIMEOUT = config->getInt("http.timeout", 60);
    DEFAULT_CONTENT_TYPE = config->getString("http.defaultContentType", "text/html;charset=UTF-8");
    SERVER_NAME = config->getString("http.serverName", "httpevent");
    X_Powered_By = "C++11";
    REAL_IP_HEAD = config->getString("http.proxyServerRealIpHeader", "X-Real-IP");
    MATCH_HOTLINKING = config->getString("http.matchHotlinking", "localhost");
    DOC_DIRECTORY = config->getString("http.docDirectory", "/var/httpevent/www");
    IP_MAX_ACCESS_COUNT = config->getInt("http.ipMaxAccessCount", 100);
    HTTP_EXPIRES = config->getInt("http.expires", 3600);
    SESSION_EXPIRES = config->getInt64("http.sessionExpires", 3600)*1000;
    CACHE_CLIENT_EXPIRES = config->getInt64("http.cacheExpires", 600);
    CACHE_EXPIRES = CACHE_CLIENT_EXPIRES * 1000;
    HTTP_PROXY_USED = config->getBool("http.proxyUsed", false);
    ENABLE_HOTLINKING = config->getBool("http.enableHotlinking", true);
    IP_ENABLE_CHECK = config->getBool("http.ipEnableCheck", true);
    ENABLE_LOGGER = config->getBool("http.enableLogger", true);
    IP_DENY_EXPIRE = config->getInt("http.ipDenyExpire", 3600)*1000;
    IP_ACCESS_INTERVAl = config->getInt("http.ipAccessInterval", 30)*1000;
    IP_DENY_FILE = config->getString("http.ipDenyFile", "/etc/httpevent/ipdeny.conf");
    ROUTE_FILE = config->getString("http.route", "/etc/httpevent/route.conf");
    PLUGIN_DIRECTORY = config->getString("http.modDirectory", "/var/httpevent/mod");
    LOG_DIRECTORY = config->getString("http.logDirectory", "/var/httpevent/log");
    LOG_FILE_SIZE = config->getInt("http.logFileSize", 1);
    LOG_PURGE_COUNT = config->getInt("http.logPurgeCount", 10);
    CERT_CERTIFICATE_FILE = config->getString("http.certCertificateFile", "/var/httpevent/cert/server.crt");
    CERT_PRIVATE_KEY_FILE = config->getString("http.certPrivateKeyFile", "/var/httpevent/cert/server.key");
    UPDATE_INTERVAL = config->getInt64("http.updateLibraryInterval", 300);
    LUA_DIRECTORY = config->getString("http.luaDirectory", "/var/httpevent/lua");
}

static void delete_handler() {
    for (auto item : HANDLER) {
        delete item.second;
        LOGGER->info("delete handler {0}", item.first);
    }
}

static void delete_static_variable() {
    delete CACHE;
    LOGGER->info("delete CACHE");
    delete SESSION;
    LOGGER->info("delete session");
    delete ROUTER;
    LOGGER->info("delete ROUTER");
    delete FILTER;
    LOGGER->info("delete FILTER");
    delete PLUGIN;
    LOGGER->info("delete PLUGIN");
    delete CLASS_LOADER;
    LOGGER->info("delete CLASS_LOADER");
}

static std::string generate_cache_key(const httpevent::request& req, const Poco::DynamicAny& any = "") {
    MD5_ENGINE.update(req.get_method() + Poco::URI(req.get_uri()).getPathAndQuery() + any.convert<std::string>());
    return Poco::MD5Engine::digestToHex(MD5_ENGINE.digest());
}

static void generate_session_cookie(httpevent::response& res, const std::string& session_id_key, const std::string& session_id_value, int expires, bool enableSSL) {
    Poco::Net::HTTPCookie cookie;
    cookie.setName(session_id_key);
    cookie.setValue(session_id_value);
    cookie.setMaxAge(expires);
    cookie.setPath("/");
    cookie.setHttpOnly(true);
    if (enableSSL) {
        cookie.setSecure(true);
    }
    res.send_cookie(cookie);
}

static bool check_not_expired(const httpevent::head& head, const Poco::DateTime& dt, long expires) {
    Poco::DateTime modifiedSince;
    int tzd;
    Poco::DateTimeParser::parse(head.at(IF_MODIFIED_SINCE_HEAD), modifiedSince, tzd);
    return (dt.timestamp().epochTime() - modifiedSince.timestamp().epochTime()) <= expires;

}

static void logger(const std::string& client_ip
        , const std::string& user_agent
        , const std::string& method
        , const std::string& uri
        , int code) {
    if (ENABLE_LOGGER) {
        LOGGER->info("{0} {1} {2} {3} {4}"
                , client_ip
                , user_agent
                , method
                , uri
                , code
                );
    }
}

static void document_request_handler(struct evhttp_request *req, void* arg) {
    if (evhttp_request_get_command(req) != EVHTTP_REQ_GET) {
        evhttp_send_error(req, 405, HTTPERR_405);
        return;
    }

    const char *docroot = DOC_DIRECTORY.c_str();
    const char *uri = evhttp_request_get_uri(req);
    struct evhttp_uri *decoded = NULL;
    const char *path;
    char *decoded_path;
    int fd = -1;
    struct stat st;

    decoded = evhttp_uri_parse(uri);
    if (!decoded) {
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
        return;
    }

    path = evhttp_uri_get_path(decoded);
    if (!path) path = "/";

    decoded_path = evhttp_uridecode(path, 0, NULL);
    if (decoded_path == NULL || strstr(decoded_path, "..")) {
        evhttp_send_error(req, 404, HTTPERR_404);
        return;
    }

    size_t len = strlen(decoded_path) + strlen(docroot) + 2;
    char whole_path[len];

    evutil_snprintf(whole_path, len, "%s/%s", docroot, decoded_path);

    if (stat(whole_path, &st) < 0) {
        evhttp_send_error(req, 404, HTTPERR_404);
        return;
    }


    if (S_ISREG(st.st_mode)) {
        if ((fd = open(whole_path, O_RDONLY)) < 0) {
            evhttp_send_error(req, 500, HTTPERR_5xx);
            if (fd >= 0)close(fd);
            return;
        }

        std::string mime_type = get_mime_type(whole_path);
        struct evkeyvalq * headers = evhttp_request_get_output_headers(req);

        evhttp_add_header(headers, CONTENT_TYPE_HEAD, mime_type.c_str());
        if (!SERVER_TYPE) {
            evhttp_add_header(headers, SERVER_HEAD, SERVER_NAME.c_str());
            evhttp_add_header(headers, X_POWERED_BY_HEAD, X_Powered_By.c_str());
        }
        if (SERVER_TYPE > 2) {
            Poco::DateTime dt;
            evhttp_add_header(headers, CACHE_CONTROL_HEAD, std::string("max-age=").append(Poco::NumberFormatter::format(HTTP_EXPIRES)).c_str());
            evhttp_add_header(headers, CACHE_CONTROL_HEAD, "must-revalidate");
            evhttp_add_header(headers, LAST_MODIFIED_HEAD, Poco::DateTimeFormatter::format(dt, Poco::DateTimeFormat::HTTP_FORMAT).c_str());
        }
        struct evbuffer * evb = evhttp_request_get_output_buffer(req);
        evbuffer_add_file(evb, fd, 0, st.st_size);
        evhttp_send_reply(req, 200, HTTP_STATUS_200, evb);

        if (decoded) {
            evhttp_uri_free(decoded);
        }
        if (decoded_path) {
            free(decoded_path);
        }
    } else {
        evhttp_send_error(req, 403, HTTPERR_403);
    }
}

static void simple_request_handler(struct evhttp_request * req, void* arg) {
    httpevent::request request(req);
    httpevent::response response(req);
    response.send_head(SERVER_HEAD, SERVER_NAME)
            .send_head(X_POWERED_BY_HEAD, X_Powered_By);

    httpevent::view* handler = 0;
    httpevent::route::route_result_t route_result = ROUTER->get_route(request.get_method(), request.get_uri());

    if (!route_result.second.empty()) {
        if (HANDLER.find(route_result.first) == HANDLER.end()) {
            auto finded = CLASS_LOADER->findClass(route_result.first);
            if (finded && finded->canCreate()) {
                handler = CLASS_LOADER->create(route_result.first);
                HANDLER[route_result.first] = handler;
            }
        } else {
            handler = HANDLER[route_result.first];
        }
    }
    if (handler) {
        handler->route_data = &route_result.second;
        config_lua_state(&request, &response, route_result.second);
        handler->lua_state = &LUA_STATE;
        handler->handler(request, response);
        if (!response.is_sent()) {
            response.submit(200, HTTP_STATUS_200);
        }
    } else {
        document_request_handler(req, NULL);
    }

}

static void cache_request_handler(struct evhttp_request *req, void *arg) {
    httpevent::request request(req);
    httpevent::response response(req);
    response.send_head(SERVER_HEAD, SERVER_NAME)
            .send_head(X_POWERED_BY_HEAD, X_Powered_By);
    httpevent::head head(request);

    Poco::DateTime dt;
    if (head.find(IF_MODIFIED_SINCE_HEAD) != head.end()) {
        if (head.find(IF_NONE_MATCH_HEAD) != head.end()) {
            if (check_not_expired(head, dt, CACHE_CLIENT_EXPIRES)) {
                response.submit(304, HTTP_STATUS_304);
                return;
            }
        }
        if (check_not_expired(head, dt, HTTP_EXPIRES)) {
            response.submit(304, HTTP_STATUS_304);
            return;
        }
    }

    httpevent::view* handler = 0;
    httpevent::route::route_result_t route_result = ROUTER->get_route(request.get_method(), request.get_uri());

    if (!route_result.second.empty()) {
        if (HANDLER.find(route_result.first) == HANDLER.end()) {
            auto finded = CLASS_LOADER->findClass(route_result.first);
            if (finded && finded->canCreate()) {
                handler = CLASS_LOADER->create(route_result.first);
                HANDLER[route_result.first] = handler;
            }
        } else {
            handler = HANDLER[route_result.first];
        }
    }
    if (handler) {
        response.send_head(ETAG_HEAD, ETAG_VALUE)
                .send_head(CACHE_CONTROL_HEAD, std::string("max-age=").append(Poco::NumberFormatter::format(CACHE_CLIENT_EXPIRES)).c_str())
                .send_head(CACHE_CONTROL_HEAD, "must-revalidate")
                .send_head(LAST_MODIFIED_HEAD, Poco::DateTimeFormatter::format(dt, Poco::DateTimeFormat::HTTP_FORMAT));
        std::string cache_body_id = generate_cache_key(request, "body")
                , cache_head_id = generate_cache_key(request, "head");
        if (CACHE->has(cache_body_id)) {
            response.send_head(CONTENT_TYPE_HEAD, *CACHE->get(cache_head_id))
                    .send_body(*CACHE->get(cache_body_id))
                    .submit(200);
            return;
        }
        handler->route_data = &route_result.second;
        config_lua_state(&request, &response, route_result.second);
        handler->lua_state = &LUA_STATE;
        handler->handler(request, response);
        if (!response.is_sent()) {
            CACHE->add(cache_head_id, evhttp_find_header(evhttp_request_get_output_headers(req), CONTENT_TYPE_HEAD));
            CACHE->add(cache_body_id, response.get_response_data());
            response.submit(200, HTTP_STATUS_200);
        }
    } else {
        document_request_handler(req, NULL);
    }

}

static void session_request_handler(struct evhttp_request *req, void *arg) {
    httpevent::request request(req);
    httpevent::response response(req);
    response.send_head(SERVER_HEAD, SERVER_NAME)
            .send_head(X_POWERED_BY_HEAD, X_Powered_By);

    httpevent::head head(request);
    httpevent::view* handler = 0;
    httpevent::route::route_result_t route_result = ROUTER->get_route(request.get_method(), request.get_uri());

    if (!route_result.second.empty()) {
        if (HANDLER.find(route_result.first) == HANDLER.end()) {
            auto finded = CLASS_LOADER->findClass(route_result.first);
            if (finded && finded->canCreate()) {
                handler = CLASS_LOADER->create(route_result.first);
                HANDLER[route_result.first] = handler;
            }
        } else {
            handler = HANDLER[route_result.first];
        }
    }
    if (handler) {
        httpevent::cookies cookies;
        httpevent::parse_cookie(head, cookies);
        std::string SESSION_ID_VALUE;
        if (cookies.find(SESSION_ID_KEY) != cookies.end()) {
            SESSION_ID_VALUE = cookies[SESSION_ID_KEY].convert<std::string>();
            if (!SESSION->has(SESSION_ID_VALUE)) {
                generate_session_cookie(response, SESSION_ID_KEY, SESSION_ID_VALUE, HTTP_EXPIRES, ENABLE_SSL);
                SESSION->add(SESSION_ID_VALUE, std::map<std::string, Poco::DynamicAny>());
            }
        } else {
            SESSION_ID_VALUE = Poco::UUIDGenerator::defaultGenerator().createRandom().toString();
            generate_session_cookie(response, SESSION_ID_KEY, SESSION_ID_VALUE, HTTP_EXPIRES, ENABLE_SSL);
            SESSION->add(SESSION_ID_VALUE, std::map<std::string, Poco::DynamicAny>());
        }
        handler->route_data = &route_result.second;
        handler->cookie_data = &cookies;
        handler->session_data = SESSION->get(SESSION_ID_VALUE).get();
        config_lua_state(&request, &response, route_result.second);
        handler->lua_state = &LUA_STATE;
        handler->handler(request, response);
        if (!response.is_sent()) {
            response.submit(200, HTTP_STATUS_200);
        }
    } else {
        document_request_handler(req, NULL);
    }
}

static void cache_session_request_handler(struct evhttp_request *req, void *arg) {
    httpevent::request request(req);
    httpevent::response response(req);
    response.send_head(SERVER_HEAD, SERVER_NAME)
            .send_head(X_POWERED_BY_HEAD, X_Powered_By);

    httpevent::head head(request);

    Poco::DateTime dt;
    if (head.find(IF_MODIFIED_SINCE_HEAD) != head.end()) {
        if (head.find(IF_NONE_MATCH_HEAD) != head.end()) {
            if (check_not_expired(head, dt, CACHE_CLIENT_EXPIRES)) {
                response.submit(304, HTTP_STATUS_304);
                return;
            }
        }
        if (check_not_expired(head, dt, HTTP_EXPIRES)) {
            response.submit(304, HTTP_STATUS_304);
            return;
        }

    }

    httpevent::view* handler = 0;
    httpevent::route::route_result_t route_result = ROUTER->get_route(request.get_method(), request.get_uri());

    if (!route_result.second.empty()) {
        if (HANDLER.find(route_result.first) == HANDLER.end()) {
            auto finded = CLASS_LOADER->findClass(route_result.first);
            if (finded && finded->canCreate()) {
                handler = CLASS_LOADER->create(route_result.first);
                HANDLER[route_result.first] = handler;
            }
        } else {
            handler = HANDLER[route_result.first];
        }
    }
    if (handler) {
        response.send_head(ETAG_HEAD, ETAG_VALUE)
                .send_head(CACHE_CONTROL_HEAD, std::string("max-age=").append(Poco::NumberFormatter::format(CACHE_CLIENT_EXPIRES)).c_str())
                .send_head(CACHE_CONTROL_HEAD, "must-revalidate")
                .send_head(LAST_MODIFIED_HEAD, Poco::DateTimeFormatter::format(dt, Poco::DateTimeFormat::HTTP_FORMAT));
        std::string cache_body_id = generate_cache_key(request, "body")
                , cache_head_id = generate_cache_key(request, "head");
        if (CACHE->has(cache_body_id)) {
            response.send_head(CONTENT_TYPE_HEAD, *CACHE->get(cache_head_id))
                    .send_body(*CACHE->get(cache_body_id))
                    .submit(200);
            return;
        }
        httpevent::cookies cookies;
        httpevent::parse_cookie(head, cookies);
        std::string SESSION_ID_VALUE;
        if (cookies.find(SESSION_ID_KEY) != cookies.end()) {
            SESSION_ID_VALUE = cookies[SESSION_ID_KEY].convert<std::string>();
            if (!SESSION->has(SESSION_ID_VALUE)) {
                generate_session_cookie(response, SESSION_ID_KEY, SESSION_ID_VALUE, HTTP_EXPIRES, ENABLE_SSL);
                SESSION->add(SESSION_ID_VALUE, std::map<std::string, Poco::DynamicAny>());
            }
        } else {
            SESSION_ID_VALUE = Poco::UUIDGenerator::defaultGenerator().createRandom().toString();
            generate_session_cookie(response, SESSION_ID_KEY, SESSION_ID_VALUE, HTTP_EXPIRES, ENABLE_SSL);
            SESSION->add(SESSION_ID_VALUE, std::map<std::string, Poco::DynamicAny>());
        }
        handler->route_data = &route_result.second;
        handler->cookie_data = &cookies;
        handler->session_data = SESSION->get(SESSION_ID_VALUE).get();
        config_lua_state(&request, &response, route_result.second);
        handler->lua_state = &LUA_STATE;
        handler->handler(request, response);
        if (!response.is_sent()) {
            CACHE->add(cache_head_id, evhttp_find_header(evhttp_request_get_output_headers(req), CONTENT_TYPE_HEAD));
            CACHE->add(cache_body_id, response.get_response_data());
            response.submit(200, HTTP_STATUS_200);
        }
    } else {
        document_request_handler(req, NULL);
    }
}

static void generic_request_handler(struct evhttp_request *req, void *arg) {
    httpevent::request request(req);
    httpevent::response response(req);
    response.send_head(SERVER_HEAD, SERVER_NAME)
            .send_head(X_POWERED_BY_HEAD, X_Powered_By);

    httpevent::head head(request);

    std::string client_ip = request.get_client();
    const std::string& user_agent = request.get_user_agent()
            , &uri = request.get_uri()
            , &method = request.get_method();
    if (HTTP_PROXY_USED) {
        const std::string& real_ip = head.at(REAL_IP_HEAD);
        if (!real_ip.empty()) {
            client_ip = real_ip;
        }
    }

    if (ENABLE_HOTLINKING && head.find(REFERER_HEAD) != head.end()) {
        Poco::RegularExpression hotlinkRegex(MATCH_HOTLINKING);
        if (!hotlinkRegex.match(Poco::URI(head.at(REFERER_HEAD).convert<std::string>()).getHost())) {
            evhttp_send_error(req, 403, HTTPERR_403);
            logger(client_ip
                    , user_agent
                    , method
                    , uri
                    , evhttp_request_get_response_code(req));
            return;
        }
    }

    if (FILTER->kill(client_ip)
            || (IP_ENABLE_CHECK
            && FILTER->deny(client_ip, IP_MAX_ACCESS_COUNT))) {
        evhttp_send_error(req, 403, HTTPERR_403);
        logger(client_ip
                , user_agent
                , method
                , uri
                , evhttp_request_get_response_code(req));
        return;
    }

    Poco::DateTime dt;
    if (head.find(IF_MODIFIED_SINCE_HEAD) != head.end()) {
        if (head.find(IF_NONE_MATCH_HEAD) != head.end()) {
            if (check_not_expired(head, dt, CACHE_CLIENT_EXPIRES)) {
                response.submit(304, HTTP_STATUS_304);
                logger(client_ip
                        , user_agent
                        , method
                        , uri
                        , evhttp_request_get_response_code(req));
                return;
            }
        }
        if (check_not_expired(head, dt, HTTP_EXPIRES)) {
            response.submit(304, HTTP_STATUS_304);
            logger(client_ip
                    , user_agent
                    , method
                    , uri
                    , evhttp_request_get_response_code(req));
            return;
        }
    }

    httpevent::view* handler = 0;
    httpevent::route::route_result_t route_result = ROUTER->get_route(request.get_method(), request.get_uri());

    if (!route_result.second.empty()) {
        if (HANDLER.find(route_result.first) == HANDLER.end()) {
            auto finded = CLASS_LOADER->findClass(route_result.first);
            if (finded && finded->canCreate()) {
                handler = CLASS_LOADER->create(route_result.first);
                HANDLER[route_result.first] = handler;
            }
        } else {
            handler = HANDLER[route_result.first];
        }
    }
    if (handler) {
        response.send_head(ETAG_HEAD, ETAG_VALUE)
                .send_head(CACHE_CONTROL_HEAD, std::string("max-age=").append(Poco::NumberFormatter::format(CACHE_CLIENT_EXPIRES)).c_str())
                .send_head(CACHE_CONTROL_HEAD, "must-revalidate")
                .send_head(LAST_MODIFIED_HEAD, Poco::DateTimeFormatter::format(dt, Poco::DateTimeFormat::HTTP_FORMAT));
        std::string cache_body_id = generate_cache_key(request, "body")
                , cache_head_id = generate_cache_key(request, "head");
        if (CACHE->has(cache_body_id)) {
            response.send_head(CONTENT_TYPE_HEAD, *CACHE->get(cache_head_id))
                    .send_body(*CACHE->get(cache_body_id))
                    .submit(200);
            logger(client_ip
                    , user_agent
                    , method
                    , uri
                    , evhttp_request_get_response_code(req));
            return;
        }
        httpevent::cookies cookies;
        httpevent::parse_cookie(head, cookies);
        std::string SESSION_ID_VALUE;
        if (cookies.find(SESSION_ID_KEY) != cookies.end()) {
            SESSION_ID_VALUE = cookies[SESSION_ID_KEY].convert<std::string>();
            if (!SESSION->has(SESSION_ID_VALUE)) {
                generate_session_cookie(response, SESSION_ID_KEY, SESSION_ID_VALUE, HTTP_EXPIRES, ENABLE_SSL);
                SESSION->add(SESSION_ID_VALUE, std::map<std::string, Poco::DynamicAny>());
            }
        } else {
            SESSION_ID_VALUE = Poco::UUIDGenerator::defaultGenerator().createRandom().toString();
            generate_session_cookie(response, SESSION_ID_KEY, SESSION_ID_VALUE, HTTP_EXPIRES, ENABLE_SSL);
            SESSION->add(SESSION_ID_VALUE, std::map<std::string, Poco::DynamicAny>());
        }
        handler->route_data = &route_result.second;
        handler->cookie_data = &cookies;
        handler->session_data = SESSION->get(SESSION_ID_VALUE).get();
        config_lua_state(&request, &response, route_result.second);
        handler->lua_state = &LUA_STATE;
        handler->handler(request, response);
        if (!response.is_sent()) {
            CACHE->add(cache_head_id, evhttp_find_header(evhttp_request_get_output_headers(req), CONTENT_TYPE_HEAD));
            CACHE->add(cache_body_id, response.get_response_data());
            response.submit(200, HTTP_STATUS_200);
        }
    } else {
        document_request_handler(req, NULL);
    }
    logger(client_ip
            , user_agent
            , method
            , uri
            , evhttp_request_get_response_code(req));
}

static void signal_normal_cb(int sig) {
    struct timeval delay = {3, 0};
    switch (sig) {
        case SIGTERM:
        case SIGHUP:
        case SIGQUIT:
        case SIGINT:
        case SIGKILL:
            if (BASE&&!event_base_loopexit(BASE, &delay)) {
            }
            break;
    }
}

static void update_cb(evutil_socket_t fd, short ev, void *arg) {
    PLUGIN->update();
    ROUTER->update();
    FILTER->update();
    update_mime_type();
}


#endif /* CALLBACK_HPP */

