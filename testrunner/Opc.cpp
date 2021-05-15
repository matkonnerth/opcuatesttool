#include "Opc.h"
#include <string>

using chaiscript::constructor;
using chaiscript::fun;
using modernopc::BrowseResult;
using modernopc::NodeId;
using modernopc::QualifiedName;
using modernopc::Variant;

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

   m_read = [&](const NodeId& id) { return client->read(id); };

   m_resolveUri = [&](const std::string& uri) { return client->resolveNamespaceUri(uri); };

   m_write = [&](const NodeId& id, const Variant& var) { client->write(id, var); };

   m_browse = [&](const NodeId& id) { return client->browse(id); };
   m_IsVariable = [&](const BrowseResult& res) { return (res.Type() == modernopc::NodeType::VARIABLE); };

   m_call = [&](const NodeId& objId, const NodeId& methodId, const std::vector<Variant>& inputs) {
      return client->call(objId, methodId, inputs);
   };

   // opc variant
   m->add(chaiscript::user_type<Variant>(), "Variant");
   m->add(chaiscript::constructor<Variant(int32_t val)>(), "Variant");
   m->add(chaiscript::constructor<Variant(int64_t val)>(), "Variant");
   m->add(chaiscript::constructor<Variant(std::string val)>(), "Variant");
   m->add(fun(&Variant::operator==), "==");
   using VariantList = std::vector<Variant>;
   chai.add(chaiscript::bootstrap::standard_library::vector_type<VariantList>("VariantList"));
   // opc services
   m->add(chaiscript::fun(m_read), "read");
   m->add(chaiscript::fun(m_write), "write");
   m->add(fun(m_browse), "browse");
   m->add(fun(m_IsVariable), "isVariable");
   m->add(fun(m_resolveUri), "resolveUri");
   m->add(fun(m_call), "call");

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

   chai.add(m);
}
} // namespace opctest::testrunner