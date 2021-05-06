#pragma once
#include "Request.h"
#include <httplib.h>
#include <iostream>
#include <unordered_map>


namespace opctest::api {
class Server
{
public:
   Server(std::string ip, int port)
   : ip{ std::move(ip) }
   , port{ port }
   {
      srv.Options("/(.*)", [&](const httplib::Request& /*req*/, httplib::Response& res) {
         res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
         res.set_header("Content-Type", "text/html; charset=utf-8");
         res.set_header("Access-Control-Allow-Headers", "X-Requested-With, Content-Type, Accept");
         res.set_header("Access-Control-Allow-Origin", "*");
         res.set_header("Connection", "close");
      });
      // getJobs
      srv.Get("/api/jobs", [&](const httplib::Request& httpReq, httplib::Response& httpRes) {
         httpRes.set_header("Access-Control-Allow-Origin", "*");

         GetJobsRequest req{};
         if(httpReq.has_param("from"))
         {
            req.from = stoi(httpReq.get_param_value("from"));
         }
         if (httpReq.has_param("max"))
         {
            req.max = stoi(httpReq.get_param_value("max"));
         }

         RequestVariant varReq{ req };
         ResponseVariant varResp{};
         callback(varReq, varResp);

         using nlohmann::json;
         json jResp = json::object();
         jResp["ok"] = std::get<GetJobResponse>(varResp).ok;
         jResp["response"] = json::parse(std::get<GetJobResponse>(varResp).data);
         httpRes.set_content(jResp.dump(), "application/json");
      });

      srv.Get("/api/scripts", [&](const httplib::Request& /*httpReq*/, httplib::Response& httpRes) {
         httpRes.set_header("Access-Control-Allow-Origin", "*");

         GetScriptsRequest req{};
         RequestVariant varReq{ req };
         ResponseVariant varResp{};
         callback(varReq, varResp);

         using nlohmann::json;
         json jResp = json::object();
         jResp["ok"] = std::get<GetScriptsResponse>(varResp).ok;
         jResp["response"] = json::parse(std::get<GetScriptsResponse>(varResp).data);
         httpRes.set_content(jResp.dump(), "application/json");
      });

      srv.Get(R"(/api/scripts/(\w+))", [&](const httplib::Request& httpReq, httplib::Response& httpRes) {
         httpRes.set_header("Access-Control-Allow-Origin", "*");

         GetScriptRequest req{};
         req.name = httpReq.matches[1];
         RequestVariant varReq{ req };
         ResponseVariant varResp{};
         callback(varReq, varResp);
         httpRes.set_content(std::get<GetScriptResponse>(varResp).content, "text/plain");
      });

      srv.Post(R"(/api/scripts/(\w+))", [&](const httplib::Request& httpReq, httplib::Response& httpRes) {
         httpRes.set_header("Access-Control-Allow-Origin", "*");

         UpdateScriptRequest req{};
         req.name = httpReq.matches[1];
         req.content = httpReq.body;
         RequestVariant varReq{ req };
         ResponseVariant varResp{};
         callback(varReq, varResp);
         using nlohmann::json;
         json jResp = json::object();
         jResp["ok"] = std::get<GetScriptsResponse>(varResp).ok;
         httpRes.set_content(jResp.dump(), "application/json");
      });

      // newJob
      srv.Post("/api/jobs", [&](const httplib::Request& httpReq, httplib::Response& httpRes) {
         httpRes.set_header("Access-Control-Allow-Origin", "*");

         NewJobRequest req{};
         req.jsonRequest = httpReq.body;

         RequestVariant varReq{ req };
         ResponseVariant varResp{};
         callback(varReq, varResp);

         using nlohmann::json;
         json jResp = std::get<NewJobResponse>(varResp);
         httpRes.set_content(jResp.dump(), "application/json");
      });

      // get finished job
      srv.Get(R"(/api/jobs/(\d+))", [&](const httplib::Request& httpReq, httplib::Response& httpRes) {
         httpRes.set_header("Access-Control-Allow-Origin", "*");

         GetJobRequest req{};
         req.id = stoi(httpReq.matches[1]);

         RequestVariant varReq{ req };
         ResponseVariant varResp{};
         callback(varReq, varResp);

         using nlohmann::json;
         json jResp = json::object();
         jResp["statusCode"] = std::get<GetJobResponse>(varResp).ok;
         jResp["response"] = json::parse(std::get<GetJobResponse>(varResp).data);
         httpRes.set_content(jResp.dump(), "application/json");
      });

      // get finished job
      srv.Get(R"(/api/jobs/logs/(\d+))", [&](const httplib::Request& httpReq, httplib::Response& httpRes) {
         httpRes.set_header("Access-Control-Allow-Origin", "*");

         GetJobLogRequest req{};
         req.id = stoi(httpReq.matches[1]);

         RequestVariant varReq{ req };
         ResponseVariant varResp{};
         callback(varReq, varResp);

         httpRes.set_content(std::get<GetJobLogResponse>(varResp).data, "text/plain");
      });
   }

   void listen()
   {
      srv.listen(ip.c_str(), port);
   }

   void setCallback(RequestCallback cb)
   {
      callback = cb;
   }

private:
   httplib::Server srv;
   std::string ip{ "0.0.0.0" };
   int port{ 9888 };
   RequestCallback callback{ nullptr };
};
}; // namespace opctest::api