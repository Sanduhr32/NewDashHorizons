/**
 *
 *  MCServer.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
#include <drogon/HttpAppFramework.h>
#include <ranges>
#include <netdb.h>
#include <vector>

using namespace drogon;

namespace api {
    class MCServerFilter : public HttpFilter<MCServerFilter> {
    private:
        int lookup(const std::string& host, int family, std::vector<std::string>& out) ;
    public:
        virtual void doFilter(const HttpRequestPtr &req,
                      FilterCallback &&fcb,
                      FilterChainCallback &&fccb) override ;
    };
}
