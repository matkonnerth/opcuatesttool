#include <InfluxDBFactory.h>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdio.h>

using json = nlohmann::json;

/* holder for curl fetch */
struct curl_fetch_st
{
   char* payload;
   size_t size;
};


/* callback for curl fetch */
size_t curl_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
   size_t realsize = size * nmemb; /* calculate buffer size */
   struct curl_fetch_st* p = (struct curl_fetch_st*)userp; /* cast pointer to fetch struct */

   /* expand buffer */
   p->payload = (char*)realloc(p->payload, p->size + realsize + 1);

   /* check buffer */
   if (p->payload == NULL)
   {
      /* this isn't good */
      fprintf(stderr, "ERROR: Failed to expand buffer in curl_callback");
      /* free buffer */
      free(p->payload);
      /* return */
      return 1;
   }

   /* copy contents to buffer */
   memcpy(&(p->payload[p->size]), contents, realsize);

   /* set new buffer size */
   p->size += realsize;

   /* ensure null termination */
   p->payload[p->size] = 0;

   /* return size */
   return realsize;
}

/* fetch and return url body via curl */
CURLcode curl_fetch_url(CURL* ch, const char* url, struct curl_fetch_st* fetch)
{
   CURLcode rcode; /* curl result code */

   /* init payload */
   fetch->payload = (char*)calloc(1, sizeof(fetch->payload));

   /* check payload */
   if (fetch->payload == NULL)
   {
      /* log error */
      fprintf(stderr, "ERROR: Failed to allocate payload in curl_fetch_url");
      /* return error */
      return CURLE_FAILED_INIT;
   }

   /* init size */
   fetch->size = 0;

   /* set url to fetch */
   curl_easy_setopt(ch, CURLOPT_URL, url);

   /* set calback function */
   curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, curl_callback);

   /* pass fetch struct pointer */
   curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void*)fetch);

   /* set default user agent */
   curl_easy_setopt(ch, CURLOPT_USERAGENT, "libcurl-agent/1.0");

   /* set timeout */
   curl_easy_setopt(ch, CURLOPT_TIMEOUT, 15);

   /* enable location redirects */
   curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);

   /* set maximum allowed redirects */
   curl_easy_setopt(ch, CURLOPT_MAXREDIRS, 1);

   /* fetch the url */
   rcode = curl_easy_perform(ch);

   /* return */
   return rcode;
}

int main(void)
{
   CURL* curl;
   CURLcode res;

   curl_global_init(CURL_GLOBAL_DEFAULT);

   struct curl_fetch_st curlFinishedJobs;
   memset(&curlFinishedJobs, 0, sizeof(curl_fetch_st));

   curl = curl_easy_init();
   if (!curl)
   {
      std::cout << "curl init failed\n";
      curl_global_cleanup();
      return 1;
   }
   curl_easy_setopt(curl, CURLOPT_URL, "localhost:9080/jobs?finished=true");

   /* Perform the request, res will get the return code */
   res = curl_fetch_url(curl, "localhost:9080/jobs?finished=true", &curlFinishedJobs);
   /* Check for errors */
   if (res != CURLE_OK)
   {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      curl_easy_cleanup(curl);
      return 1;
   }

   auto j = json::parse(curlFinishedJobs.payload);

   auto influxdb = influxdb::InfluxDBFactory::Get("http://localhost:8086?db=mydb");

   for (auto& job : j.items())
   {
      int id = job.value().at("request").at("id").get<int>();
      std::string name = job.value().at("request").at("name").get<std::string>();
      double totalRuntime_ms = job.value().at("result").at("totalRuntime_ms").get<double>();
      int64_t ts_start = job.value().at("result").at("ts_start").get<int64_t>();
      int64_t ts_stop = job.value().at("result").at("ts_stop").get<int64_t>();

      // we insert a start and a stop event
      influxdb->write(influxdb::Point{ "mydb" }.addField("totalRuntime_ms", totalRuntime_ms).addTag("id", std::to_string(id)).setTimestamp(std::chrono::time_point<std::chrono::system_clock>(std::chrono::nanoseconds(ts_stop))));
      // influxdb->write(influxdb::Point{ "mydb" }.addField("cnt", 1).addTag("counter", "cnt"));
   }


   curl_easy_cleanup(curl);
   curl_global_cleanup();
   return 0;
}