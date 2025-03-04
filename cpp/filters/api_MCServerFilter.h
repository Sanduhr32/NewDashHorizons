/**
 *
 *  MCServer.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
#include <drogon/HttpAppFramework.h>

using namespace drogon;

namespace api {
    class MCServerFilter : public HttpFilter<MCServerFilter> {
    public:
        virtual void doFilter(const HttpRequestPtr &req,
                      FilterCallback &&fcb,
                      FilterChainCallback &&fccb) override ;
    };
}
