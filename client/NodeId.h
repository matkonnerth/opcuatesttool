#pragma once
#include <string>

namespace opctest::client {
struct NodeId
{
public:
   NodeId(const std::string& ns, const std::string identifier)
   : namespaceUri{ ns }
   , identifier{ identifier }
   {}
   const std::string namespaceUri;
   const std::string identifier;
   friend bool operator<(const NodeId& lhs, const NodeId& rhs)
   {
      if (lhs.namespaceUri != rhs.namespaceUri)
      {
         return lhs.namespaceUri < rhs.namespaceUri;
      }
      return lhs.identifier < rhs.identifier;
}
};
} // namespace opctest::client
