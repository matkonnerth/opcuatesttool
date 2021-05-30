#include "../api/Server.h"
#include "../api/Request.h"
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <thread>
#include <unistd.h>

// just for testing
// curl - X POST - H "Content-Type: application/text" -d "print("Hello world from lua")" http : // localhost:9888/api/scripts/test.lua


void setupLogger()
{
   std::vector<spdlog::sink_ptr> sinks;
   sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
   // sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_st>(workingDir + "/logs/TestService.log", 1048576 * 5, 10));
   auto combined_logger = std::make_shared<spdlog::logger>("ReplService", begin(sinks), end(sinks));
   // register it if you need to access it globally
   spdlog::register_logger(combined_logger);
}


bool apiCallback(const opctest::api::RequestVariant& req, opctest::api::ResponseVariant& resp)
{
   std::visit(
   [&](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, opctest::api::NewLineReplRequest>)
      {
         using namespace std::chrono_literals;
         std::string message = arg.content + "\n";
         // std::cout << message;
         for (auto it = message.cbegin(); it != message.cend(); ++it)
         {
            if (ioctl(STDIN_FILENO, TIOCSTI, it))
            {
               perror("ioctl");
               resp = opctest::api::Response{ false };
            }
         }
         resp = opctest::api::Response{true};
      }
      else
      {
         // static_assert(always_false_v<T>, "non-exhaustive visitor!");
         resp = opctest::api::Response{ false };
      }
   },
   req);

   return true;
}

int main(int argc, char*argv[])
{
   setupLogger();
   auto logger = spdlog::get("ReplService");

   std::string binaryPath = argv[0];
   auto pos = binaryPath.find_last_of('/');
   binaryPath.erase(pos);

   int pipefd[2];
   if (pipe(pipefd) == -1)
   {
      perror("pipe");
      exit(EXIT_FAILURE);
   }
   else
   {
      std::cout << "pipe fd[0] = " << pipefd[0] << " fd[1] " << pipefd[1] << "\n";
   }

   int pid = fork();

   if (pid == -1)
   {
      // pid == -1 means error occured
      logger->error("failed to fork Repl, exit");
      exit(EXIT_FAILURE);
   }
   else if (pid == 0)
   {
      // child process

      close(pipefd[0]); /* Close unused read end */

      std::string m{ "test" };
      write(pipefd[1], m.c_str(), m.size());

      // close file descriptors, let stdin/stdout/stderror and pipe open
      for (int fd = 6; fd < 256; fd++)
      {
         close(fd);
      }

        //pipe stdout also to pipe
      dup2(pipefd[1], 1);

      // the argv list first argument should point to
      // filename associated with file being executed
      // the array pointer must be terminated by NULL
      // pointer
      char* const argv_list[] = { "./repl", nullptr };

      // the execv() only return if error occured.
      // The return value is -1
      execv("./bin/repl", argv_list);
      exit(0);
   }
   else
   {
      // parent
      logger->info("repl forked, pid {}", pid);

      close(pipefd[1]); /* Close unused write end */
   }
   opctest::api::Server server{ "0.0.0.0", 9888, binaryPath + "/dist/testToolApp" };


   auto cb = [](const opctest::api::RequestVariant& req, opctest::api::ResponseVariant& resp) { return apiCallback(req, resp); };

   std::thread pipeListener{ [&] {
      char buf[1024];
      while (true)
      {
         int bytes = read(pipefd[0], &buf, 1024);
         if (bytes == 0)
         {
            std::cout << "child closed pipe\n";
            break;
         }
         else
         {
            std::cout << "print" << "\n";
            write(STDOUT_FILENO, &buf, bytes);
            //auto f = server.getEventCallback();
            //f("ReplResponseEvent", std::string{buf, bytes});
         }
      }
   } };

   // service.setJobFinishedCallback(server.getEventCallback());

   server.setCallback(cb);
   server.listen();
}