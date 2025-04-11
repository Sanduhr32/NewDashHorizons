#include <drogon/drogon.h>

namespace drogon {
    using Callback = std::function<void(const HttpResponsePtr &)>;
}

int main() {
    //Load config file
    drogon::app().loadConfigFile("config.json");
    //drogon::app().loadConfigFile("../config.yaml");
    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().registerHandler("/stop", [](const drogon::HttpRequestPtr &req, drogon::Callback &&call) {
        auto response = drogon::HttpResponse::newHttpResponse();
        response->setStatusCode(drogon::k200OK);
        response->setBody("shutting");
        call(response);
        drogon::app().quit();
    },{drogon::Get, "api::ProxyHeaderFilter", "drogon::LocalHostFilter"});
    drogon::app().registerHandler("/init-db", [](const drogon::HttpRequestPtr &req, drogon::Callback &&call) {
        auto db = drogon::app().getDbClient();
        auto response = drogon::HttpResponse::newHttpResponse();

        LOG_INFO << db->connectionInfo();

        auto f = db->execSqlAsyncFuture("CREATE TABLE IF NOT EXISTS \"power_metrics\" ("
                        "id BIGSERIAL primary key,"
                        "name varchar(255) not null,"
                        "eu BIGINT not null,"
                        "eu_max BIGINT not null,"
                        "eu_in BIGINT not null,"
                        "eu_out BIGINT not null"
                        "time TIMESTAMP default (now() at time zone 'utc')"
                        ");");

        try {
            auto res = f.get();
            if (res.empty())
                LOG_INFO << "empty table create result";
            else
                LOG_INFO << "probably was successful?";
            response->setStatusCode(drogon::k200OK);
            call(response);
        } catch (const drogon::orm::DrogonDbException &e) {
            LOG_ERROR << e.base().what();
            response->setStatusCode(drogon::k500InternalServerError);
            call(response);
        }

    },{drogon::Get, "api::ProxyHeaderFilter", "drogon::LocalHostFilter"});

    drogon::app().run();
    return 0;
}
