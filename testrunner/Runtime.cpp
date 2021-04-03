#include "Runtime.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <modernOpc/BrowseResult.h>
#include <modernOpc/UnresolvedNodeId.h>
#include <modernOpc/Variant.h>
#include <modernOpc/types/NodeId.h>
#include <modernOpc/types/QualifiedName.h>
#include <thread>

using chaiscript::constructor;
using chaiscript::fun;
using modernopc::BrowseResult;
using modernopc::NodeId;
using modernopc::QualifiedName;
using opctest::testrunner::Runtime;

void wait(int delay)
{
   std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

void Runtime::load()
{

   client = std::make_unique<modernopc::Client>(m_uri);
   client->connect();

   m_read = [&](const modernopc::UnresolvedNodeId& id) {
      auto resolvedId = client->resolve(id);
      return client->read(resolvedId);
   };

   m_write = [&](const modernopc::UnresolvedNodeId& id, const modernopc::Variant& var) {
      auto resolvedId = client->resolve(id);
      client->write(resolvedId, var);
   };

   m_browse = [&](const NodeId& id) { return client->browse(id); };

   chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());


   m->add(chaiscript::fun(&wait), "wait");
   m->add(chaiscript::user_type<modernopc::UnresolvedNodeId>(), "UriId");
   m->add(chaiscript::constructor<modernopc::UnresolvedNodeId(const std::string& ns, const std::string identifier)>(), "UriId");
   m->add(chaiscript::constructor<modernopc::UnresolvedNodeId(const modernopc::UnresolvedNodeId& other)>(), "UriId");

   m->add(chaiscript::user_type<modernopc::Variant>(), "Variant");
   m->add(chaiscript::constructor<modernopc::Variant(int32_t val)>(), "Int32");
   m->add(chaiscript::constructor<modernopc::Variant(int64_t val)>(), "Int64");
   m->add(chaiscript::fun(m_read), "read");
   m->add(chaiscript::fun(m_write), "write");
   m->add(fun(m_browse), "browse");

   chaiscript::utility::add_class<BrowseResult>(*m,
                                                "BrowseResult",
                                                { constructor<BrowseResult(const BrowseResult&)>(), constructor<BrowseResult(BrowseResult &&)>() },
                                                {
                                                { fun(&BrowseResult::Id), "Id" },
                                                { fun(&BrowseResult::Name), "Name" },
                                                });

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

void Runtime::eval()
{
   try
   {
      chai.eval_file(m_script);
   }
   catch (chaiscript::exception::eval_error& e)
   {
      logger->error(e.pretty_print());
      throw;
   }
}