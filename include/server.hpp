#ifndef SERVER_HPP
#define SERVER_HPP

#include "httpevent.hpp"
#include "config.hpp"
#include "callback.hpp"
#include "ssl.hpp"
#include "spdlog/spdlog.h"

namespace httpevent {

    class server : public Poco::Util::ServerApplication {
    public:

        server() : helpRequested(false) {
        }
        ~server() = default;
    protected:

        void initialize(Poco::Util::Application& self) {
            Poco::Util::ServerApplication::initialize(self);
        }

        void uninitialize() {
            Poco::Util::ServerApplication::uninitialize();
        }

        void defineOptions(Poco::Util::OptionSet& options) {
            Poco::Util::ServerApplication::defineOptions(options);
            options.addOption(
                    Poco::Util::Option("help", "h", "display help information on command line arguments")
                    .required(false)
                    .repeatable(false)
                    .callback(Poco::Util::OptionCallback<httpevent::server>(this, &httpevent::server::handleHelp)));
            
            options.addOption(
                    Poco::Util::Option("config", "c", "load configuration data from a file")
                    .required(true)
                    .repeatable(true)
                    .argument("file")
                    .callback(Poco::Util::OptionCallback<httpevent::server>(this, &httpevent::server::handleConfig)));

        }

        void handleHelp(const std::string& name, const std::string& value) {
            this->helpRequested = true;
            this->displayHelp();
            this->stopOptionsProcessing();
        }

        void handleConfig(const std::string& name, const std::string& value) {
            this->loadConfiguration(value);
        }

        int main(const std::vector<std::string>& args) {
            if (!helpRequested) {
                CONFIG = &(this->config());
                init_callback_config(CONFIG);
                init_mime_type(MIME_SRC_FILE);
                init_lua_state();

                std::string logger_file = LOG_DIRECTORY + "/log";
                spdlog::set_level(spdlog::level::info);
                spdlog::set_async_mode(4096);
                auto file_logger = spdlog::rotating_logger_mt("file_logger"
                        , logger_file
                        , 1024 * 1024 * LOG_FILE_SIZE
                        , LOG_PURGE_COUNT);
                LOGGER = file_logger.get();


                CLASS_LOADER = new Poco::ClassLoader<httpevent::view>;
                PLUGIN = new httpevent::plugin(PLUGIN_DIRECTORY, CLASS_LOADER);
                FILTER = new httpevent::filter(IP_DENY_EXPIRE, IP_ACCESS_INTERVAl, IP_DENY_FILE);
                ROUTER = new httpevent::route(ROUTE_FILE);
                SESSION = new httpevent::session(SESSION_EXPIRES);
                CACHE = new httpevent::cache(CACHE_EXPIRES);



                BASE = event_base_new();

                SERVER = evhttp_new(BASE);


                if (ENABLE_SSL) {

                    if (!initailize_ssl(CTX, ECDH, SERVER, CERT_CERTIFICATE_FILE.c_str(), CERT_PRIVATE_KEY_FILE.c_str(), LOGGER)) {
                        evhttp_free(SERVER);
                        event_base_free(BASE);
                        return 0;
                    }
                }



                evhttp_bind_socket(SERVER, HOST.c_str(), PORT);

                switch (SERVER_TYPE) {
                    case 0:
                        CB = document_request_handler;
                        break;
                    case 1:
                        CB = simple_request_handler;
                        break;
                    case 2:
                        CB = session_request_handler;
                        break;
                    case 3:
                        CB = cache_request_handler;
                        break;
                    case 4:
                        CB = cache_session_request_handler;
                        break;
                    case 5:
                        CB = generic_request_handler;
                        break;
                    default:
                        CB = generic_request_handler;
                }
                evhttp_set_gencb(SERVER, CB, NULL);
                evhttp_set_default_content_type(SERVER, DEFAULT_CONTENT_TYPE.c_str());
                evhttp_set_timeout(SERVER, TIMEOUT);


                signal(SIGHUP, signal_normal_cb);
                signal(SIGTERM, signal_normal_cb);
                signal(SIGINT, signal_normal_cb);
                signal(SIGQUIT, signal_normal_cb);
                signal(SIGKILL, signal_normal_cb);

                LOGGER->info("server is ready");


                struct event ev_update;
                event_assign(&ev_update, BASE, -1, EV_PERSIST, update_cb, 0);
                struct timeval tv;
                evutil_timerclear(&tv);
                tv.tv_sec = UPDATE_INTERVAL;
                event_add(&ev_update, &tv);
                LOGGER->info("timer is ready.");


                event_base_dispatch(BASE);

                LOGGER->info("server will close");


                evhttp_free(SERVER);
                LOGGER->info("free server");

                event_base_free(BASE);
                LOGGER->info("free base");

                if (ECDH) {
                    EC_KEY_free(ECDH);
                    LOGGER->info("free EC_KEY");
                }
                if (CTX) {
                    SSL_CTX_free(CTX);
                    LOGGER->info("free SSL_CTX");
                }


                delete_handler();
                delete_static_variable();

                LOGGER->info("server closed");
                spdlog::drop_all();
                LOGGER->info("drop LOGGER");


            }
            return Poco::Util::Application::EXIT_OK;
        }
    private:
        bool helpRequested;

        void displayHelp() {
            Poco::Util::HelpFormatter helpFormatter(options());
            helpFormatter.setCommand(commandName());
            helpFormatter.setUsage("OPTIONS");
            helpFormatter.setHeader(
                    "A web server, based on libevent and POCO C++ library."
                    "\nThe configuration file(s) must be located in the same directory."
                    "\nAnd must have the same base name as the executable,"
                    "\nwith one of the following extensions :.properties, .ini or .xml."
                    );
            helpFormatter.setFooter(
                    "\nEmail:admin@webcpp.net"
                    "\nSite:https://www.webcpp.net"
                    );
            helpFormatter.format(std::cout);
        }

    };

}

#endif /* SERVER_HPP */

