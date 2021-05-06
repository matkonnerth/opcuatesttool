#pragma once
#include <functional>
#include <nlohmann/json.hpp>
#include <string>
#include <variant>

namespace opctest::api {

struct Request
{
   std::string jsonRequest;
};

struct NewJobRequest : public Request
{};

struct GetJobRequest : public Request
{
   int id;
};

struct GetJobLogRequest : public Request
{
   int id;
};

struct GetJobsRequest : public Request
{
   int from;
   int max;
};

struct GetScriptsRequest : public Request
{};

struct GetScriptRequest : public Request
{
   std::string name;
};

struct UpdateScriptRequest : public Request
{
   std::string name;
   std::string content;
};

struct Response
{
   bool ok;
};

struct NewJobResponse : public Response
{
   int id;
};

struct GetJobResponse : public Response
{
   std::string data;
};

struct GetScriptsResponse : public Response
{
   std::string data;
};

struct GetScriptResponse : public Response
{
   std::string content;
};

struct GetJobLogResponse : public Response
{
   std::string data;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NewJobResponse, ok, id);

using RequestVariant = std::variant<NewJobRequest, GetJobRequest, GetJobsRequest, GetScriptsRequest, GetScriptRequest, UpdateScriptRequest, GetJobLogRequest>;
using ResponseVariant = std::variant<Response, NewJobResponse, GetJobResponse, GetScriptsResponse, GetScriptResponse, GetJobLogResponse>;

using RequestCallback = std::function<bool(const RequestVariant&, ResponseVariant&)>;

} // namespace opctest::api