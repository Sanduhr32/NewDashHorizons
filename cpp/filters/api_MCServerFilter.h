/**
 *
 *  MCServer.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
#include <drogon/HttpAppFramework.h>
#if __has_include(<format>)
# include <format>
#endif

using namespace drogon;

namespace api {
    class MCServerFilter : public HttpFilter<MCServerFilter> {
    private:
    public:
        virtual void doFilter(const HttpRequestPtr &req,
                      FilterCallback &&fcb,
                      FilterChainCallback &&fccb) override ;
    };
}
