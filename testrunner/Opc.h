#pragma once
#include <chaiscript/chaiscript.hpp>
#include <modernOpc/client/Client.h>
#include <vector>

namespace opctest::testrunner {
class Opc
{
public:
   void registerNamespace(chaiscript::ChaiScript& chai, const std::string& uri);

private:
   std::function<modernopc::Variant(const modernopc::NodeId&)> m_read;
   std::function<void(const modernopc::NodeId& id, const modernopc::Variant& var)> m_write;
   std::function<std::vector<modernopc::BrowseResult>(const modernopc::NodeId& id)> m_browse;
   std::function<bool(const modernopc::BrowseResult& res)> m_IsVariable;
   std::function<int(const std::string& uri)> m_resolveUri;
   std::function<std::vector<modernopc::Variant>(const modernopc::NodeId&, const modernopc::NodeId&, const std::vector<modernopc::Variant>&)> m_call;
   std::shared_ptr<modernopc::Client> client{};
};
} // namespace opctest::testrunner

// Import namespace in ChaiScript

// import("math") print(math.pi) // prints 3.14159