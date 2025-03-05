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

    if (json != nullptr && !json->empty()) {
        if (!param.has_value()) {
            LOG_INFO << "JSON not null, trying to clear it!";
            json->clear();
            LOG_INFO << "JSON cleared!";
            json = req->getJsonObject();
            LOG_INFO << "Copied posted json!";
        } else {
            LOG_INFO << "Trying to insert posted json into JSON";
            (*json)[param.value()] = *req->getJsonObject();
            LOG_INFO << "Inserted posted json!";
        }
    }

    auto response = drogon::HttpResponse::newHttpResponse();
    callback(response);

    std::ostringstream sfilename;

    LOG_INFO << "writing json to file!";

#if __has_include(<format>)
    sfilename << std::format("{:%Y_%m_%d_%H_%M_%S}", std::chrono::system_clock::now()) << ".json";
#else
    sfilename << count++ << ".json";
#endif

    std::string filename = sfilename.str();
    std::ofstream jsonfile(filename, std::ios::out);
    LOG_INFO << "file name is: " << filename;

    if (jsonfile.is_open()) {
        LOG_INFO << "file is open, writing!";
        jsonfile << *json << "\n";
        LOG_INFO << "written!";
        jsonfile.close();
        LOG_INFO << "closing";
    }
}