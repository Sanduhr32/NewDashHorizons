/**
 *
 *  api_ProxyHeaderFilter.cc
 *
 */

#include "api_ProxyHeaderFilter.h"

using namespace drogon;
using namespace api;

void ProxyHeaderFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    auto forwarded = req->getHeader("Forwarded");
    //Edit your logic here
    if (forwarded.find("for=") != std::string::npos) {
        //Check failed
        auto res = drogon::HttpResponse::newHttpResponse();
        res->setStatusCode(k500InternalServerError);
        fcb(res);
        return;
    }
    //Passed
    fccb();
}
