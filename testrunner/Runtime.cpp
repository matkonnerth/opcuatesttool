#include "Runtime.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

using opctest::testrunner::Runtime;

void Runtime::load()
{
   opc.registerNamespace(chai, m_uri);
   //load lib
   chai.eval_file(m_scriptDir + "/libOpc.chai");
}

void Runtime::eval()
{
   try
   {
      chai.eval_file(m_scriptDir + "/" + m_script);
   }
   catch (chaiscript::exception::eval_error& e)
   {
      logger->error(e.pretty_print());
      throw;
   }
}