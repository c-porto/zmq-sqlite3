#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>
#include <zmq.hpp>

#include "message.hh"

int main(int argc, char *argv[]) {
  zmq::context_t ctx{1};
  zmq::socket_t socket{ctx, zmq::socket_type::req};
  socket.connect("tcp://0.0.0.0:42068");
  int counter = 0;
  for (;;) {
    if (counter % 2 == 0) {
      message::Message msg{message::TestType::kVoltageTest,
                           message::TestResult::kTestPassed, 3.3f};
      zmq::message_t pkt = message::Message::serialize(msg);
      socket.send(pkt, zmq::send_flags::none);
    } else if (counter % 3 == 0) {
      message::Message msg{message::TestType::kCurrentTest,
                           message::TestResult::kTestFailed, 0.500313f};
      zmq::message_t pkt = message::Message::serialize(msg);
      socket.send(pkt, zmq::send_flags::none);
    } else if (counter % 5 == 0) {
      message::Message msg{message::TestType::kTemperatureTest,
                           message::TestResult::kTestPassed, 30.30f};
      zmq::message_t pkt = message::Message::serialize(msg);
      socket.send(pkt, zmq::send_flags::none);
    } else {
      message::Message msg{message::TestType::kTemperatureTest,
                           message::TestResult::kTestFailed, -5.0f};
      zmq::message_t pkt = message::Message::serialize(msg);
      socket.send(pkt, zmq::send_flags::none);
    }
    zmq::message_t res{1};
    auto __tmp = socket.recv(res, zmq::recv_flags::none);
    auto __res = *(static_cast<uint8_t *>(res.data()));
    if (static_cast<message::ServerResponse>(__res) !=
        message::ServerResponse::kServerOk) {
      std::cerr << "Something went wrong" << '\n';
    }
    ++counter;
    std::this_thread::sleep_for(std::chrono::milliseconds{50});
  }
  return 0;
}
