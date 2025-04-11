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

    auto newJson = req->getJsonObject();
    if (newJson == nullptr) {
        auto response = drogon::HttpResponse::newHttpResponse();
        response->setStatusCode(k500InternalServerError);
        response->setBody(req->getJsonError());
        callback(response);
        return;
    }

    auto mc_system = req->getOptionalParameter<std::string>("system");
    auto system_type = req->getOptionalParameter<std::string>("type");

    LOG_INFO << "Received a new JSON about " << mc_system.value_or("generic") << " from Minecraft!";

    if (json != nullptr) {
        if (!mc_system.has_value()) {
            LOG_INFO << "JSON not null, trying to clear it!";
            json->clear();
            json = req->getJsonObject();
        } else {
            LOG_INFO << "Trying to insert posted json into JSON";
            (*json)[mc_system.value()] = *req->getJsonObject();
        }
    } else {
        json = req->getJsonObject();
    }

    if (system_type.has_value() && system_type == "power") {
        auto db = drogon::app().getDbClient();
        auto data = req->getJsonObject();
        db->execSqlAsync("INSERT INTO \"power_metrics\" (name, eu, eu_max, eu_in, eu_out) VALUES ($1, $2, $3, $4, $5);",
            [](const drogon::orm::Result &res){
                LOG_DEBUG << "inserted a power metric";
            },
            [](const drogon::orm::DrogonDbException &e) {
                LOG_ERROR << e.base().what();
            },
            mc_system.value(), (*data)["eu"].asInt64(), (*data)["eu_max"].asInt64(), (*data)["eu_in"].asInt64(), (*data)["eu_out"].asInt64()
        );
    }

    auto response = drogon::HttpResponse::newHttpResponse();
    callback(response);
}