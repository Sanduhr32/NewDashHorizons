#pragma once

#include <drogon/drogon.h>

using namespace drogon;
using Callback = std::function<void(const HttpResponsePtr &)>;

namespace api {
    class RawJSON : public drogon::HttpController<RawJSON> {
    private:
        std::shared_ptr<Json::Value> json {};
    public:
        METHOD_LIST_BEGIN
            // use METHOD_ADD to add your custom processing function here;
            ADD_METHOD_TO(RawJSON::get, "/api", Get);
            ADD_METHOD_TO(RawJSON::post, "/api", Post);

        METHOD_LIST_END

        // your declaration of processing function maybe like this:
        void get(const HttpRequestPtr &req, Callback &&callback);

        void post(const HttpRequestPtr &req, Callback &&callback);
    };
}
