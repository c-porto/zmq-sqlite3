#ifndef LOGS_H_
#define LOGS_H_

#include <filesystem>
#include <fstream>
#include <ios>

namespace logs {
class Logger final {
 public:
  Logger(std::string const &&path) : path_{std::move(path)} {
    if (!std::filesystem::exists("./logs")) {
      std::filesystem::create_directory("./logs");
    }
    file_ = std::ofstream{path_, std::ios::app};
  }
  void operator<<(std::string const &str) {
    file_ << str << '\n';
    file_.flush();
  };

 private:
  std::filesystem::path path_;
  std::ofstream file_;
};
}  // namespace logs

#endif  // !DEBUG
