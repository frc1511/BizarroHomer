#include <BizarroHomer/Util/Song.hpp>

#define MUSIC_PATH "/var/frc1511/BizarroHomer/music/"

static const std::filesystem::path music_dir(MUSIC_PATH);

Song::Song(std::filesystem::path _primary_chrp, std::chrono::milliseconds _length)
: Song("/var/frc1511/BizarroHomer/music/" + _primary_chrp.string(), "/var/frc1511/BizarroHomer/music/" + _primary_chrp.string(), _length) { }

Song::Song(std::filesystem::path _primary_chrp, std::filesystem::path _secondary_chrp, std::chrono::milliseconds _length)
: primary_chrp(music_dir / _primary_chrp), secondary_chrp(music_dir / _secondary_chrp), length(_length) { }
