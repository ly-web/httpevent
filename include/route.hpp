#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <Poco//StringTokenizer.h>
#include <Poco/RegularExpression.h>

namespace httpevent {

    class route {
    public:
        typedef std::pair<std::string, std::vector<std::string>> route_result_t;
    private:

        class route_element {
        public:
            route_element() = delete;

            route_element(const std::string& method, const std::string& pattern, const std::string& class_name) :
            method(method), pattern(pattern), class_name(class_name) {
            }
            virtual ~route_element() = default;

            route_element(const route_element& other) :
            method(other.method), pattern(other.pattern), class_name(other.class_name) {
            }

            route_element& operator=(const route_element& right) {
                if (this == &right) {
                    return *this;
                } else {
                    this->method = right.method;
                    this->pattern = right.pattern;
                    this->class_name = right.class_name;
                }

                return *this;
            }

            const std::string& get_method()const {
                return this->method;
            }

            const std::string& get_pattern()const {
                return this->pattern;
            }

            const std::string& get_class_name()const {
                return this->class_name;
            }
        private:

            std::string method, pattern, class_name;
        };
    public:
        route() = delete;
        virtual~route() = default;
        route(const route& other) = delete;
        route& operator=(const route& right) = delete;

        route(const std::string& path)
        : route_path(path), route_data() {
            this->init();
        }
    public:

        void update() {
            this->route_data.clear();
            this->init();
        }

        route_result_t get_route(const std::string& method, const std::string& path) {
            route_result_t result;
            for (auto &item : this->route_data) {
                const std::string& M = item.get_method();
                const std::string& P = item.get_pattern();
                try {
                    Poco::RegularExpression regex(P);
                    if (method == M && regex.match(path)) {
                        regex.split(path, result.second);
                        result.first = item.get_class_name();
                        break;
                    }
                } catch (Poco::RegularExpressionException& e) {

                }
            }
            return result;
        }
    private:

        void init() {
            std::ifstream input(this->route_path);
            if (input) {
                std::string line;
                while (std::getline(input, line)) {
                    if (line.front() != '#' && !line.empty()) {
                        Poco::StringTokenizer st(line, ",;", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
                        if (st.count() == 3) {
                            this->route_data.push_back(route_element(st[0], st[1], st[2]));
                        }
                    }
                }
            }
        }
        std::string route_path;
        std::vector<route_element> route_data;
    };

}

#endif /* ROUTE_HPP */

