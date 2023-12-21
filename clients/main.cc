#include <iostream>
#include <thread>
#include <zmq.hpp>


int main (int argc, char *argv[]) {

    zmq::context_t ctx{1};

    zmq::socket_t socket{ctx,zmq::socket_type::req};

    socket.connect("tcp://0.0.0.0:3000");

    for(int i{0}; i < 10;++i) {
        zmq::message_t request{"EAE",3};

        std::puts("Sending EAE\n");

        socket.send(request,zmq::send_flags::none);

        zmq::message_t rp;

        socket.recv(rp,zmq::recv_flags::none);

        std::cout << "Received: " << rp.to_string_view() << '\n';

    }

    return 0;
}
