#pragma once

#include <BizarroHomer/Hardware/HardwareComponent.hpp>
#include <BizarroHomer/Util/Song.hpp>
#include <ctre/phoenix/music/Orchestra.h>
#include <optional>
#include <functional>
#include <vector>
#include <utility>

namespace thunder {

class TalonFX;

class HardwareManager {
public:
  static inline HardwareManager* get() {
    return &instance;
  }
  
  HardwareManager(HardwareManager const&) = delete;
  HardwareManager& operator=(HardwareManager const&) = delete;
  
  //
  // Registers a hardware component with the HardwareManager.
  //
  void register_hardware(HardwareComponent* hardware);
  void register_talon_fx(TalonFX* talon_fx);
  
  void stop_all_hardware();
  
  //
  // Begins playing a song using the Falcon 500 motor controllers.
  //
  // Tthe Falon 500s will be disabled for the duration of the song.
  //
  void start_song(Song song);
  
  //
  // Processes the orchestra.
  //
  void process_orchestra();
  
  //
  // Whether a song is currently playing.
  //
  bool is_song_playing();
  
  inline static const Song HOME_DEPOT_BEAT {
    "home_depot_beat_primary.chrp",
    "home_depot_beat_secondary.chrp",
    30s
  };
  inline static const Song THUNDERSTRUCK {
    "thunderstruck.chrp",
    30s
  };
  
private:
  HardwareManager();
  ~HardwareManager();

  void set_falcons_enabled(bool enabled);
  
  std::vector<HardwareComponent*> m_all_hardware;
  std::vector<TalonFX*> m_talon_fxs;
  
  ctre::phoenix::music::Orchestra m_orchestra_primary;
  ctre::phoenix::music::Orchestra m_orchestra_secondary;
  std::optional<Song> m_current_song = std::nullopt;
  
  static HardwareManager instance;
};

} // namespace thunder
