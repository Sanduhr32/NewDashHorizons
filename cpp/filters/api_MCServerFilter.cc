/**
 *
 *  MCServer.cc
 *
 */

#include "api_MCServerFilter.h"

using namespace drogon;
using namespace api;

int MCServerFilter::lookup(const std::string& host, int family, std::vector<std::string>& out) {
    addrinfo hints{AI_CANONNAME, family, SOCK_STREAM}, *res, *result;
    if (getaddrinfo(host.c_str(), nullptr, &hints, &result))
    {
        LOG_ERROR << "could not resolve " << host << " as family " << family;
        return 1;
    }
    res = result;
    while (res) {
        if (res->ai_family != family) {
            res = res->ai_next;
            continue;
        }
        if (res->ai_family == AF_INET) {
                auto *addr_in = reinterpret_cast<sockaddr_in *>(res->ai_addr);
                out.emplace_back(inet_ntoa(addr_in->sin_addr));
        }
    }
    freeaddrinfo(result);
    return 0;
}

void MCServerFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    LOG_INFO << "Header: " << req->getHeader("x-forwarded-for");

    std::string realIp = req->getHeader("x-forwarded-for");
    std::vector<std::string> dnsIps;
    lookup(app().getCustomConfig()["server-host"].asString(), AF_INET, dnsIps);

    if ((dnsIps | std::views::filter([&](auto& ip){return realIp == ip; })).empty())
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
