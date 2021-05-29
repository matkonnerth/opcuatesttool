#pragma once
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <thread>
#include <unistd.h>

namespace opctest {
class Repl
{
public:
   Repl(const std::string& workingDir, std::function<void(const std::string&, const std::string&)> callback): m_workingDir{workingDir}, m_replFinishedCallback{callback}
   {
      auto logger = spdlog::get("TestService");
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

         // pipe stdout also to pipe
         dup2(pipefd[1], 1);

         // the argv list first argument should point to
         // filename associated with file being executed
         // the array pointer must be terminated by NULL
         // pointer
         char* const argv_list[] = { "./repl", nullptr };

         // the execv() only return if error occured.
         // The return value is -1
         std::string exePath = m_workingDir + "/repl";
         execv(exePath.c_str(), argv_list);
         exit(0);
      }
      else
      {
         // parent
         logger->info("repl forked, pid {}", pid);

         close(pipefd[1]); /* Close unused write end */
      }

      pipeListener = std::thread{[&] {
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
               std::cout << "print"
                         << "\n";
               write(STDOUT_FILENO, &buf, bytes);
               m_replFinishedCallback("ReplResponseEvent", std::string{ buf, bytes });
            }
         }
      }};
   }

   void newLine(const std::string& line)
   {
      std::string message = line + "\n";
      for (auto it = message.cbegin(); it != message.cend(); ++it)
      {
         if (ioctl(STDIN_FILENO, TIOCSTI, it))
         {
            perror("ioctl");
         }
      }
   }

private:
    const std::string m_workingDir;
    int pipefd[2];
    std::function<void(const std::string&, const std::string&)> m_replFinishedCallback;
    std::thread pipeListener;
};
}; // namespace opctest