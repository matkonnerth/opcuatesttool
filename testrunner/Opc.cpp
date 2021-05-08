#include "Opc.h"

using chaiscript::constructor;
using chaiscript::fun;
using modernopc::BrowseResult;
using modernopc::NodeId;
using modernopc::QualifiedName;

namespace opctest::testrunner {
void wait(int delay)
{
   std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

void Opc::registerNamespace(chaiscript::ChaiScript& chai, const std::string& uri)
{
   chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

   // namespace basic
   m->add(chaiscript::fun(&wait), "wait");

   client = std::make_unique<modernopc::Client>(uri);
   client->connect();

   m_read = [&](const modernopc::NodeId& id) {
      // auto resolvedId = client->resolve(id);
      return client->read(id);
   };

   m_write = [&](const modernopc::NodeId& id, const modernopc::Variant& var) { client->write(id, var); };

   m_browse = [&](const NodeId& id) { return client->browse(id); };
   m_IsVariable = [&](const BrowseResult& res) { return (res.Type() == modernopc::NodeType::VARIABLE); };

   // namespace opc
   m->add(chaiscript::user_type<modernopc::UnresolvedNodeId>(), "UriId");
   m->add(chaiscript::constructor<modernopc::UnresolvedNodeId(const std::string& ns, const std::string identifier)>(), "UriId");
   m->add(chaiscript::constructor<modernopc::UnresolvedNodeId(const modernopc::UnresolvedNodeId& other)>(), "UriId");

   // opc variant
   m->add(chaiscript::user_type<modernopc::Variant>(), "Variant");
   m->add(chaiscript::constructor<modernopc::Variant(int32_t val)>(), "Int32");
   m->add(chaiscript::constructor<modernopc::Variant(int64_t val)>(), "Int64");
   // opc services
   m->add(chaiscript::fun(m_read), "read");
   m->add(chaiscript::fun(m_write), "write");
   m->add(fun(m_browse), "browse");
   m->add(fun(m_IsVariable), "isVariable");

   chaiscript::utility::add_class<BrowseResult>(*m, "BrowseResult", { constructor<BrowseResult(const BrowseResult&)>(), constructor<BrowseResult(BrowseResult &&)>() }, { { fun(&BrowseResult::Id), "Id" }, { fun(&BrowseResult::Name), "Name" } });

   using BrowseResultList = std::vector<BrowseResult>;
   chai.add(chaiscript::bootstrap::standard_library::vector_type<BrowseResultList>("BrowseResultList"));
   chaiscript::utility::add_class<QualifiedName>(*m,
                                                 "QualifiedName",
                                                 { constructor<QualifiedName(const QualifiedName&)>(), constructor<QualifiedName(QualifiedName &&)>() },
                                                 {
                                                 { fun(&QualifiedName::name), "Name" },
                                                 });

   chaiscript::utility::add_class<NodeId>(
   *m, "NodeId", { constructor<NodeId(uint16_t nsIdx, int id)>(), constructor<NodeId(uint16_t nsIdx, const std::string& id)>(), constructor<NodeId(const NodeId&)>(), constructor<BrowseResult(BrowseResult &&)>() }, {});

   //   chai->register_namespace(
   //   [](chaiscript::Namespace& opc) {
   //      math["pi"] = chaiscript::const_var(3.14159);
   //      math["sin"] = chaiscript::var(chaiscript::fun([](const double x) { return sin(x); }));
   //   },
   //   "opc");

   chai.add(m);
}
} // namespace opctest::testrunner