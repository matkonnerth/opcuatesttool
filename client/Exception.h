#pragma once
#include <exception>
#include <string>

namespace opctest::client
{
class OpcException : public std::runtime_error
{

public:
   OpcException(const std::string& msg)
   : std::runtime_error(msg)
   {}

   virtual ~OpcException() = default;
};
}