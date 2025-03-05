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

    for (const auto &[header, value] : req->headers())
#if __has_include(<format>)
        LOG_INFO << std::format("%20s", header) << ": " << std::format("%-20s", value);
#else
        LOG_INFO << header << ": " << value;
#endif

    for (const auto &[param, value] : req->parameters())
#if __has_include(<format>)
        LOG_INFO << std::format("%20s", param) << ": " << std::format("%-20s", value);
#else
        LOG_INFO << param << ": " << value;
#endif

    if (app().getCustomConfig()["simple-auth"].asString() != req->getHeader("x-drogon-auth"))
    {
        //Check failed
        LOG_ERROR << "failed check!";
        auto res = drogon::HttpResponse::newHttpResponse();
        res->setStatusCode(k401Unauthorized);
        fcb(res);
        return;
    }
    LOG_INFO << "passed the post!";
    //Passed
    fccb();
}
