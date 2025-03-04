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

    auto param = req->getOptionalParameter<std::string>("system");

    LOG_INFO << "Received a new JSON about " << param.value_or("generic") << " from Minecraft!";

    if (json != nullptr) {
        if (!param.has_value()) {
            json->clear();
            json = req->getJsonObject();
        } else {
            (*json)[param.value()] = *req->getJsonObject();
        }
    }

    auto response = drogon::HttpResponse::newHttpResponse();
    callback(response);

    std::ostringstream filename;
    filename << std::format("{:%Y_%m_%d_%H_%M_%S}", std::chrono::system_clock::now()) << ".json";
    std::ofstream jsonfile(filename.str(), std::ios::out);

    if (jsonfile.is_open()) {
        jsonfile << *json << "\n";
        jsonfile.close();
    }
}