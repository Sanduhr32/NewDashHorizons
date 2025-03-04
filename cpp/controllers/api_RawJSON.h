#pragma once

#include <drogon/drogon.h>
#include <fstream>
#include <sstream>
#if __has_include(<format>)
# include <format>
#endif
#include <chrono>

using namespace drogon;
using Callback = std::function<void(const HttpResponsePtr &)>;

namespace api {
    class RawJSON : public drogon::HttpController<RawJSON> {
    private:
        std::shared_ptr<Json::Value> json = std::make_shared<Json::Value>();
    public:
        METHOD_LIST_BEGIN
            // use METHOD_ADD to add your custom processing function here;
            ADD_METHOD_TO(RawJSON::get, "/api", Get);
            ADD_METHOD_TO(RawJSON::post, "/api", Post, "api::MCServerFilter");

        METHOD_LIST_END

        // your declaration of processing function maybe like this:
        void get(const HttpRequestPtr &req, Callback &&callback);

        void post(const HttpRequestPtr &req, Callback &&callback);
    };
}
