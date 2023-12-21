#include <iostream>
#include <thread>
#include <zmq.hpp>


int main (int argc, char *argv[]) {

    zmq::context_t ctx{1};

    zmq::socket_t socket{ctx,zmq::socket_type::rep};

    socket.bind("tcp://0.0.0.0:3000");

    while(true) {
        zmq::message_t rq;

        auto result = socket.recv(rq,zmq::recv_flags::none);

        std::cout << rq.to_string() << '\n';

        std::string str_res = "Server recebeu: " + rq.to_string();

        zmq::message_t reply{str_res.length()};

        std::memcpy(reply.data(),str_res.c_str(),str_res.length());

        socket.send(reply,zmq::send_flags::none);
    }
    return 0;
}
