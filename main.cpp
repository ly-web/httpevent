#include "include/server.hpp"


int main(int argc, char** argv) {
    httpevent::server server;
    return server.run(argc, argv);
}
