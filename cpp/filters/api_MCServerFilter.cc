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
    auto dnsAddr = std::vector<trantor::InetAddress>();
    app().getResolver()->resolve(app().getCustomConfig()["server-host"].asString(),
                 [&dnsAddr](const std::vector<trantor::InetAddress> &addresses) {
                     for (const auto &address: addresses) {
                         dnsAddr.push_back(address);
                     }
    });

//    for (auto &[header, value]: req->headers()) {
//        LOG_INFO << header << ": " << value;
//    }

    // block thread while async dns resolve is in progress
    // with maximum total timeout of around 1s
    size_t iterations = 0;
    while (!dnsAddr.empty() && 5 * iterations < 1000) {
        iterations++;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    for (const auto &entry: dnsAddr)
        LOG_INFO << "DNS: " << entry.toIp();
    LOG_INFO << "Header: " << req->getHeader("x-forwarded-for");

    std::string realIp = req->getHeader("x-forwarded-for");

    if ((dnsAddr | std::views::transform([](auto x){ return x.toIp(); })
                | std::views::filter([&](auto x){return x == realIp; })).empty())
    {
        //Check failed
        LOG_ERROR << "failed check!";
        auto res = drogon::HttpResponse::newHttpResponse();
        res->setStatusCode(k401Unauthorized);
        fcb(res);
        return;
    }
    //Passed
    fccb();
}
