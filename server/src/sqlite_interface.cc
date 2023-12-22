#include "sqlite_interface.hh"

template <>
SqliteErr SqliteDb<std::vector<std::string>>::exec(
    SqlOp op, std::string sql_exp, std::vector<std::string> *res_container) {
  auto __callback{callback_map_.find(op)};
  if (__callback == callback_map_.end()) {
    err_type_ = SqliteErr::kInvalidOp;
    return err_type_;
  }
  db_err_ = sqlite3_exec(db_, sql_exp.c_str(), __callback->second,
                         static_cast<void *>(res_container), &err_msg_);
  if (db_err_ != SQLITE_OK) {
    std::cerr << "Sql Error: " << err_msg_ << '\n';
    sqlite3_free(err_msg_);
    err_type_ = SqliteErr::kSqlExecErr;
    return err_type_;
  }
  return SqliteErr::kOk;
}
