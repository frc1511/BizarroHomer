template<typename T>
bool IPCReceiver::recv_msg(T* msg) {
  return recv_msg(msg, sizeof(T) - sizeof(long));
}
