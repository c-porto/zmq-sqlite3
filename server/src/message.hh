#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <zmq.hpp>

namespace message {
enum class TestType : uint8_t {
  kCurrentTest,
  kVoltageTest,
  kTemperatureTest,
  kEmpty,
};

enum class TestResult : uint8_t {
  kTestPassed,
  kTestFailed,
  kEmpty,
};

enum class ServerResponse : uint8_t {
  kServerOk,
  kServerInvalidMsg,
};

enum class DeserializeErr : uint8_t {
  kWrongProtocol,
  kOk,
};

class Message final {
public:
  Message();
  Message(TestType t, TestResult r, float value)
      : type_{t}, result_{r}, value_{value} {}
  auto &value() const { return value_; }
  auto &test_type() const { return test_type_str_; }
  auto &test_result() const { return test_result_str_; }
  DeserializeErr const &err() const { return err_; }
  static zmq::message_t serialize(Message &m) {
    std::string __msg = std::to_string(static_cast<uint8_t>(m.type_)) +
                        std::to_string(static_cast<uint8_t>(m.result_)) +
                        std::to_string(m.value_);
    zmq::message_t msg{__msg};
    return msg;
  }
  static Message deserialize(zmq::message_t &pkt) {
    Message msg;
    char *__pkt;
    std::strcpy(__pkt, pkt.to_string().c_str());
    switch (static_cast<TestType>(*__pkt)) {
    case TestType::kCurrentTest:
      msg.type_ = TestType::kCurrentTest;
      msg.test_type_str_ = "Current Test";
      break;
    case TestType::kVoltageTest:
      msg.type_ = TestType::kVoltageTest;
      msg.test_type_str_ = "Voltage Test";
      break;
    case TestType::kTemperatureTest:
      msg.type_ = TestType::kTemperatureTest;
      msg.test_type_str_ = "Temperature Test";
      break;
    default:
      msg.err_ = DeserializeErr::kWrongProtocol;
    }
    ++__pkt;
    switch (static_cast<TestResult>(*__pkt)) {
    case TestResult::kTestPassed:
      msg.result_ = TestResult::kTestPassed;
      msg.test_result_str_ = "Test Passed";
      break;
    case TestResult::kTestFailed:
      msg.result_ = TestResult::kTestFailed;
      msg.test_result_str_ = "Test Failed";
      break;
    default:
      msg.err_ = DeserializeErr::kWrongProtocol;
    }
    std::string __tmp{++__pkt};
    try {
      msg.value_ = std::stof(__tmp);
    } catch (std::exception const &ex) {
      msg.err_ = DeserializeErr::kWrongProtocol;
    }
    return msg;
  }

private:
  TestType type_{TestType::kEmpty};
  TestResult result_{TestResult::kEmpty};
  float value_{0.0f};
  DeserializeErr err_{DeserializeErr::kOk};
  std::optional<std::string> test_type_str_;
  std::optional<std::string> test_result_str_;
};
} // namespace message
#endif
