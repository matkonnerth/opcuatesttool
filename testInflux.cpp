#include <InfluxDBFactory.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>

int main(int argc, char*argv[])
{
   auto influxdb = influxdb::InfluxDBFactory::Get("http://localhost:8086?db=mydb");
   influxdb->write(influxdb::Point{ "mydb" }.addField("value", 10).addField("value", 20).addField("value", 100LL).addTag("host", "adampc"));
   influxdb->write(influxdb::Point{ "mydb" }.addField("cnt", 1).addTag("counter", "cnt"));
   auto points = influxdb->query("SELECT * FROM \"mydb\"");
   
   auto now = std::chrono::system_clock::now();
   influxdb->write(influxdb::Point{"mydb"}.addField("pressure", 192.2).setTimestamp(now - std::chrono::hours(24*7)));

   for (auto p : points)
   {
      std::time_t ts = std::chrono::system_clock::to_time_t(p.getTimestamp());
      std::cout <<  std::put_time(std::localtime(&ts), "%c") << ": " << p.getFields() << "\n";
   }
   return 0;
}