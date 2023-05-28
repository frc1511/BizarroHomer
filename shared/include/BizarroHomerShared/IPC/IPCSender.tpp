template<typename T>
void IPCSender::send_msg(T& msg) {
  send_msg(*msg, sizeof(T));
}
