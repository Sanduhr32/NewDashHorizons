/**
 *
 *  MCServer.cc
 *
 */

#include "api_MCServerFilter.h"

using namespace drogon;
using namespace api;

void MCServerFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{

    if (app().getCustomConfig()["simple-auth"].asString() != req->getHeader("x-drogon-auth")
        && !req->getHeader("user-agent").starts_with("Java"))
    {
        //Check failed
        auto res = drogon::HttpResponse::newHttpResponse();
        res->setStatusCode(k401Unauthorized);
        fcb(res);
        return;
    }
    //Passed
    fccb();
}
