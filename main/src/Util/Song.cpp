#include <BizarroHomer/Util/Song.hpp>

Song::Song(std::filesystem::path _primary_chrp, std::chrono::milliseconds _length)
: Song(_primary_chrp, _primary_chrp, _length) { }

Song::Song(std::filesystem::path _primary_chrp, std::filesystem::path _secondary_chrp, std::chrono::milliseconds _length)
: primary_chrp(_primary_chrp), secondary_chrp(_secondary_chrp), length(_length) { }
