#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <ranges>
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
  kServerDeserializeErr,
};

enum class DeserializeErr : uint8_t {
  kWrongProtocol,
  kOk,
};

class Message final {
public:
  Message(){};
  Message(TestType t, TestResult r, float value)
      : type_{t}, result_{r}, value_{value} {}
  auto &value() const { return value_; }
  auto &test_type() const { return test_type_str_; }
  auto &test_result() const { return test_result_str_; }
  DeserializeErr const &err() const { return err_; }
  static zmq::message_t serialize(Message &m) {
    std::string __msg = std::to_string(static_cast<uint8_t>(m.type_)) + '#' +
                        std::to_string(static_cast<uint8_t>(m.result_)) + '#' +
                        std::to_string(m.value_);
    zmq::message_t msg{__msg};
    return msg;
  }
  static Message deserialize(zmq::message_t &pkt) {
    Message msg;
    std::string __pkt = pkt.to_string();
    std::cout << __pkt << '\n';
    auto to_string = [](auto &&r) -> std::string {
      const auto data = &*r.begin();
      const auto size = static_cast<std::size_t>(std::ranges::distance(r));
      return std::string{data, size};
    };
    const auto __range = __pkt | std::ranges::views::split('#') |
                         std::ranges::views::transform(to_string);
    std::vector<std::string> __vec{std::ranges::begin(__range),
                                   std::ranges::end(__range)};
    if (__vec.size() != 3) {
      std::cerr << "Error Parsing Message" << '\n';
      std::cerr << "Protocol wasn't correct" << '\n';
      msg.err_ = DeserializeErr::kWrongProtocol;
      return msg;
    }
    if (__vec[0] ==
        std::to_string(static_cast<uint8_t>(TestType::kCurrentTest))) {
      msg.type_ = TestType::kCurrentTest;
      msg.test_type_str_ = "Current Test";
    } else if (__vec[0] ==
               std::to_string(static_cast<uint8_t>(TestType::kVoltageTest))) {
      msg.type_ = TestType::kVoltageTest;
      msg.test_type_str_ = "Voltage Test";
    } else if (__vec[0] == std::to_string(static_cast<uint8_t>(
                               TestType::kTemperatureTest))) {
      msg.type_ = TestType::kTemperatureTest;
      msg.test_type_str_ = "Temperature Test";
    } else {
      std::cerr << "Error in Test type parse" << '\n';
      msg.err_ = DeserializeErr::kWrongProtocol;
    }
    if (__vec[1] ==
        std::to_string(static_cast<uint8_t>(TestResult::kTestPassed))) {
      msg.result_ = TestResult::kTestPassed;
      msg.test_result_str_ = "Test Passed";
    } else if (__vec[1] ==
               std::to_string(static_cast<uint8_t>(TestResult::kTestFailed))) {
      msg.result_ = TestResult::kTestFailed;
      msg.test_result_str_ = "Test Failed";
    } else {
      std::cerr << "Error in Test result parse" << '\n';
      msg.err_ = DeserializeErr::kWrongProtocol;
    }
    try {
      msg.value_ = std::stof(__vec[2]);
    } catch (std::exception const &ex) {
      std::cerr << "Error Transforming float value" << '\n';
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
