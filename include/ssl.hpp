#ifndef SSL_HPP
#define SSL_HPP

#include "httpevent.hpp"

#include "spdlog/logger.h"

static void *my_zeroing_malloc(size_t howmuch) {
    return calloc(1, howmuch);
}

static void ssl_setup(spdlog::logger* file_logger) {
    signal(SIGPIPE, SIG_IGN);
    CRYPTO_set_mem_functions(my_zeroing_malloc, realloc, free);
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    file_logger->info("Using OpenSSL version \"{0}\"\nand libevent version \"{1}\"",
            SSLeay_version(SSLEAY_VERSION),
            event_get_version());
}

static struct bufferevent* bevcb(struct event_base *base, void *arg) {
    struct bufferevent* r;
    SSL_CTX *ctx = (SSL_CTX *) arg;

    r = bufferevent_openssl_socket_new(base,
            -1,
            SSL_new(ctx),
            BUFFEREVENT_SSL_ACCEPTING,
            BEV_OPT_CLOSE_ON_FREE);
    return r;
}

static int server_setup_certs(SSL_CTX *ctx,
        const char *certificate_chain,
        const char *private_key, spdlog::logger* file_logger) {
    if (1 != SSL_CTX_use_certificate_chain_file(ctx, certificate_chain)) {
        file_logger->info("SSL use_certificate_chain_file failed");
        return 0;
    }

    if (1 != SSL_CTX_use_PrivateKey_file(ctx, private_key, SSL_FILETYPE_PEM)) {
        file_logger->info("SSL use_PrivateKey_file failed");
        return 0;
    }

    if (1 != SSL_CTX_check_private_key(ctx)) {
        file_logger->info("SSL check_private_key failed");
        return 0;
    }
    return 1;
}

static int initailize_ssl(SSL_CTX *ctx, EC_KEY *ecdh, struct evhttp *server, const char *certificate_chain,
        const char *private_key, spdlog::logger* file_logger) {
    file_logger->info("SSL initailize starting...");
    ssl_setup(file_logger);
    ctx = SSL_CTX_new(SSLv23_server_method());
    if (!ctx) {
        file_logger->info("SSL CTX initailize failed.");
        return 0;
    }
    SSL_CTX_set_options(ctx,
            SSL_OP_SINGLE_DH_USE |
            SSL_OP_SINGLE_ECDH_USE |
            SSL_OP_NO_SSLv2);
    ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!ecdh) {
        file_logger->info("SSL EC_KEY initailize failed.");
        SSL_CTX_free(ctx);
        return 0;
    }
    if (1 != SSL_CTX_set_tmp_ecdh(ctx, ecdh)) {
        file_logger->info("SSL CTX set  failed.");
        SSL_CTX_free(ctx);
        EC_KEY_free(ecdh);
        return 0;
    }

    if (!server_setup_certs(ctx, certificate_chain, private_key, file_logger)) {
        file_logger->info("SSL setup failed");
        SSL_CTX_free(ctx);
        EC_KEY_free(ecdh);
        return 0;
    }
    file_logger->info("SSL initailize  successful");
    evhttp_set_bevcb(server, bevcb, ctx);
    return 1;
}

#endif /* SSL_HPP */

