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
    auto source = req->peerAddr();
    auto dnsAddr = trantor::InetAddress{};
    app().getResolver()->resolve(app().getCustomConfig()["server-host"].asString(),
                 [&dnsAddr](const trantor::InetAddress &address) { dnsAddr = address;
    });

    // block thread while async dns resolve is in progress
    // with maximum total timeout of around 1s
    size_t iterations = 0;
    while (!dnsAddr.ipNetEndian() && 5 * iterations < 1000) {
        iterations++;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    if (source.ipNetEndian() != dnsAddr.ipNetEndian())
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
