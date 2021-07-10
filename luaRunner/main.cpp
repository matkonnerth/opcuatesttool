#define SOL_ALL_SAFETIES_ON 1
#include <modernopc/client/Client.h>
#include <modernopc/types/NodeId.h>
#include <sol/sol.hpp>

using modernopc::BrowseResult;
using modernopc::Client;
using modernopc::NodeId;
using modernopc::Variant;
using modernopc::NodeType;

std::vector<Variant> VariantVector()
{
   return {};
}

int main(int, char*[])
{

   sol::state lua;
   lua.open_libraries(sol::lib::base);

   const std::string uri{ "opc.tcp://debian:4840" };
   auto client = std::make_unique<Client>(uri);

   lua.set_function("connect", &Client::connect, client.get());
   lua.set_function("disconnect", &Client::disconnect, client.get());
   lua.set_function("read", &Client::read, client.get());
   lua.set_function("write", &Client::write, client.get());


   // make usertype metatable
   auto NodeId_type = lua.new_usertype<NodeId>("NodeId",
                                               // 3 constructors
                                               sol::constructors<NodeId(uint16_t nsIdx, int id), NodeId(uint16_t nsIdx, const std::string& id), NodeId(const NodeId&)>());

   auto Variant_Type = lua.new_usertype<Variant>("Variant", sol::constructors<Variant(int32_t val), Variant(int64_t val), Variant(std::string val)>());
   Variant_Type["toString"] = &Variant::toString;

   auto BrowseResult_Type = lua.new_usertype<BrowseResult>("BrowseResult", sol::constructors<BrowseResult(const BrowseResult&), BrowseResult(BrowseResult&&)>());
   BrowseResult_Type["Id"] = &BrowseResult::Id;
   BrowseResult_Type["Name"] = &BrowseResult::Name;
   lua["BrowseResult"]["IsVariable"] = [](BrowseResult& br) { return (br.Type() == modernopc::NodeType::VARIABLE); };

   auto m_resolveUri = [&](const std::string& namespaceUri) { return client->resolveNamespaceUri(namespaceUri); };
   lua.set_function("resolveUri", m_resolveUri);

   lua.set_function("call", &Client::call, client.get());
   lua.set_function("browse", &Client::browse, client.get());

   lua.set_function("VariantVector", &VariantVector);

   try
   {
      lua.script(R"(
                connect()
                --disconnect()
                id1 = NodeId:new(0, 2255)
                print(id1)
                val = read(id1)
                print(val:toString())
                print(resolveUri("http://opcfoundation.org/UA/"))
                --
                inputs = VariantVector()
                inputs:add(Variant:new("mk"))
                outputs = call(NodeId:new(0,85), NodeId:new(1,62541), inputs);
                print(outputs[1]:toString())
                if outputs[1]==Variant:new("Hello mk") then
                    print("variant comparision ok")
                end

                --browse
                br = browse(NodeId:new(0,85))
                for i,v in ipairs(br) do
                    print(v:Name())
                end

                function browseRecursive (id)
                    br = browse(id)
                    for i,v in ipairs(br) do
                        print(v:Name())
                        if v:IsVariable() then
                            print("isVariable")
                        end
                        browseRecursive(v:Id())
                    end
                end

                browseRecursive(NodeId:new(0, 85))

                disconnect()
                ---exception handling
                function try(f, catch_f)
                    local status, exception = pcall(f)
                    if not status then
                        catch_f(exception)
                    end
                end
                try(function()
                    -- Try block
                    --
                    val = read(NodeId.new(1,1))
                end, function(e)
                    -- Except block. E.g.:
                    -- Use e for conditional catch
                    -- Re-raise with error(e)
                    print("error on calling abc")
                end)
                print("after try catch")
                connect()
                disconnect()
        )");
   }
   catch(const std::exception& e)
   {
       std::cerr << e.what() << '\n';
   }
   
   


   return 0;
}