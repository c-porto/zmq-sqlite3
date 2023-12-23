#include <sqlite3.h>

#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>
#include <zmq.hpp>

#include "src/logs.hh"
#include "src/message.hh"

int main(int argc, char *argv[]) {
  const std::string log_path{"logs/msg.log"};
  logs::Logger log{std::move(log_path)};
  zmq::context_t ctx{1};
  zmq::socket_t socket{ctx, zmq::socket_type::rep};
  socket.bind("tcp://0.0.0.0:42068");
  while (true) {
    zmq::message_t rq{6};
    std::stringstream ss;
    auto __result = socket.recv(rq, zmq::recv_flags::none);
    std::cout << "Received: " << rq.to_string() << " from client" << '\n';
    auto msg = message::Message::deserialize(rq);
    if (msg.err() == message::DeserializeErr::kWrongProtocol) {
      zmq::message_t err_msg{1};
      const uint8_t err =
          static_cast<uint8_t>(message::ServerResponse::kServerInvalidMsg);
      std::memcpy(err_msg.data(), &err, sizeof(err));
      socket.send(err_msg, zmq::send_flags::none);
      std::cerr << "Packet did not follow to choosen protocol\n";
      continue;
    }
    if (!msg.test_type() || !msg.test_result()) {
      zmq::message_t err_msg{1};
      const uint8_t err =
          static_cast<uint8_t>(message::ServerResponse::kServerDeserializeErr);
      std::memcpy(err_msg.data(), &err, sizeof(err));
      socket.send(err_msg, zmq::send_flags::none);
      std::cerr << "Error in deserializing data";
      continue;
    }
    ss << msg.test_type().value() << "\nReading:  " << msg.value()
       << "\nResult: " << msg.test_result().value() << '\n';
    log << ss.str();
    zmq::message_t reply{1};
    const uint8_t res_ok =
        static_cast<uint8_t>(message::ServerResponse::kServerOk);
    std::memcpy(reply.data(), &res_ok, sizeof(res_ok));
    socket.send(reply, zmq::send_flags::none);
  }
  return 0;
}
