#pragma once
#include "Request.h"
#include <httplib.h>
#include <iostream>
#include <unordered_map>
#include <mutex>


namespace opctest::api {

class EventDispatcher
{
public:
   EventDispatcher()
   {}

   void wait_event(httplib::DataSink* sink)
   {
      std::unique_lock<std::mutex> lk(m_);
      int id = id_;
      cv_.wait(lk, [&] { return cid_ == id; });
      if (sink->is_writable())
      {
         sink->write(message_.data(), message_.size());
      }
   }

   void send_event(const std::string& message)
   {
      std::lock_guard<std::mutex> lk(m_);
      cid_ = id_++;
      message_ = message;
      cv_.notify_all();
   }

private:
   std::mutex m_;
   std::condition_variable cv_;
   std::atomic_int id_ = 0;
   std::atomic_int cid_ = -1;
   std::string message_;
};
class Server
{
public:
   Server(std::string ip, int port, const std::string& pathToWebApp)
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

      auto ret = srv.set_mount_point("/", pathToWebApp.c_str());
      if (!ret)
      {
         std::cout << "cannot mount webapp " << pathToWebApp << "\n";
      }
      // getJobs
      srv.Get("/api/jobs", [&](const httplib::Request& httpReq, httplib::Response& httpRes) {
         httpRes.set_header("Access-Control-Allow-Origin", "*");

         GetJobsRequest req{};
         if (httpReq.has_param("from"))
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
      srv.Get(R"(/api/jobs/(\d+)/log)", [&](const httplib::Request& httpReq, httplib::Response& httpRes) {
         httpRes.set_header("Access-Control-Allow-Origin", "*");

         GetJobLogRequest req{};
         req.id = stoi(httpReq.matches[1]);

         RequestVariant varReq{ req };
         ResponseVariant varResp{};
         callback(varReq, varResp);

         httpRes.set_content(std::get<GetJobLogResponse>(varResp).data, "text/plain");
      });

      srv.Get("/event1", [&](const httplib::Request& /*req*/, httplib::Response& res) {
         std::cout << "register event" << "\n";
         res.set_chunked_content_provider("text/event-stream", [&](size_t /*offset*/, httplib::DataSink& sink) {
            ed.wait_event(&sink);
            return true;
         });
      });

      m_fEventCallback = [&](int id) { ed.send_event(std::to_string(id)); };
   }

   void listen()
   {
      srv.listen(ip.c_str(), port);
   }

   void setCallback(RequestCallback cb)
   {
      callback = cb;
   }

   auto getEventCallback()
   {
      return m_fEventCallback;
   }

private:
   httplib::Server srv;
   std::string ip{ "0.0.0.0" };
   int port{ 9888 };
   RequestCallback callback{ nullptr };
   EventDispatcher ed{};
   std::function<void(int)> m_fEventCallback{ nullptr };
};
}; // namespace opctest::api