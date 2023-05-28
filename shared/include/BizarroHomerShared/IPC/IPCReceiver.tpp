template<typename T>
void IPCReceiver::recv_msg(T& msg) {
  recv_msg(*msg, sizeof(T));
}
