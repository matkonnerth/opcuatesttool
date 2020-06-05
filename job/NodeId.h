#pragma once
#include <string>

namespace tt
{
struct NodeId
{
public:
    const std::string namespaceUri;
    const std::string identifier;
friend bool operator<(const NodeId& lhs, const NodeId&rhs)
{
    if(lhs.namespaceUri!=rhs.namespaceUri)
    {
        return lhs.namespaceUri<rhs.namespaceUri;
    }
    return lhs.identifier < rhs.identifier;
}
};
}

