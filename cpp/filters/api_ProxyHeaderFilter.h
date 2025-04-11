/**
 *
 *  api_ProxyHeaderFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
#if __has_include(<format>)
# include <format>
#endif

using namespace drogon;
namespace api
{

class ProxyHeaderFilter : public HttpFilter<ProxyHeaderFilter>
{
  public:
    ProxyHeaderFilter() {}
    virtual void doFilter(const HttpRequestPtr &req,
                  FilterCallback &&fcb,
                  FilterChainCallback &&fccb) override;
};

}
