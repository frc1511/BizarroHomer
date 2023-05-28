template<typename T>
bool IPCSender::send_msg(T& msg) {
  return send_msg(&msg, sizeof(T) - sizeof(long));
}
