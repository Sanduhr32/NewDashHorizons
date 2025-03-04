#include "api_RawJSON.h"

using namespace api;

// Add definition of your processing function here
void RawJSON::get(const drogon::HttpRequestPtr &req, Callback &&callback) {
    if (json == nullptr)
        json = std::make_shared<Json::Value>();

    auto response = drogon::HttpResponse::newHttpJsonResponse(*json);
    callback(response);
}

void RawJSON::post(const drogon::HttpRequestPtr &req, Callback &&callback) {

//    if (0) {
//        auto response = drogon::HttpResponse::newHttpResponse();
//        response->setStatusCode(drogon::k401Unauthorized);
//        callback(response);
//        return;
//    }

    auto newJson = req->getJsonObject();
    if (newJson == nullptr) {
        auto response = drogon::HttpResponse::newHttpResponse();
        response->setStatusCode(k500InternalServerError);
        response->setBody(req->getJsonError());
        callback(response);
        return;
    }

    if (json != nullptr)
        json->clear();

    json = req->getJsonObject();

    auto response = drogon::HttpResponse::newHttpResponse();
    callback(response);
}