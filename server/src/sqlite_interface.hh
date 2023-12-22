#ifndef SQLITE_INTERFACE_H_
#define SQLITE_INTERFACE_H_

#include <filesystem>
#include <iostream>
#include <memory>
#include <sqlite3.h>
#include <unordered_map>
#include <vector>

namespace db {
enum class SqlOp {
  kCreateTable,
  kSelect,
  kInsert,
  kGeneric,
};

enum class SqliteErr {
  kOpenErr,
  kInvalidOp,
  kSqlExecErr,
  kOk,
};

using callback_fn = int (*)(void *, int, char **, char **);

template <typename ContainerType> class SqliteDb final {
public:
  SqliteDb(std::string &, std::string &);
  ~SqliteDb() { sqlite3_close(db_); };
  SqliteDb(SqliteDb &&) = delete;
  SqliteDb(const SqliteDb &) = delete;
  SqliteDb &operator=(SqliteDb &&) = delete;
  SqliteDb &operator=(const SqliteDb &) = delete;
  SqliteErr exec(SqlOp, std::string, ContainerType *);
  SqliteErr &err() { return err_type_; };

private:
  inline std::unordered_map<SqlOp, callback_fn> &&generate_callback_map();
  sqlite3 *db_;
  std::string path_;
  char *err_msg_;
  int db_err_;
  SqliteErr err_type_;
  std::unordered_map<SqlOp, callback_fn> callback_map_;
};

template <typename ContainerType>
SqliteDb<ContainerType>::SqliteDb(std::string &dir, std::string &name) {
  if ((dir != "") && !std::filesystem::exists(dir)) {
    std::filesystem::create_directory(dir);
  }
  path_ = dir + name;
  db_err_ = sqlite3_open(path_.c_str(), &db_);
  if (db_err_) {
    std::cerr << "Couldn't open database: " << sqlite3_errmsg(db_) << '\n';
    err_type_ = SqliteErr::kOpenErr;
  } else {
    callback_map_ = std::move(generate_callback_map());
  }
}

template <typename ContainerType>
inline std::unordered_map<SqlOp, callback_fn> &&
SqliteDb<ContainerType>::generate_callback_map() {
  // TODO
  return {};
}
} // namespace db
#endif // !DEBUG
