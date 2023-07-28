#include <BizarroHomer/Hardware/HardwareManager.hpp>
#include <BizarroHomer/Hardware/TalonFX.hpp>
#include <fmt/core.h>
#include <cstring>
#include <cerrno>
#include <chrono>

thunder::HardwareManager::HardwareManager() = default;

thunder::HardwareManager::~HardwareManager() = default;

void thunder::HardwareManager::register_hardware(HardwareComponent* hardware) {
  m_all_hardware.push_back(hardware);
}

void thunder::HardwareManager::register_talon_fx(TalonFX* talon_fx) {
  m_talon_fxs.push_back(talon_fx);
  
  register_hardware(reinterpret_cast<HardwareComponent*>(talon_fx));
  
  // Add to orchestra.
  if (m_talon_fxs.size() == 2 ) { // 3) {
    m_orchestra_secondary.AddInstrument(*m_talon_fxs.back()->get_talon());
  }
  else {
    m_orchestra_primary.AddInstrument(*m_talon_fxs.back()->get_talon());
  }
}

void thunder::HardwareManager::stop_all_hardware() {
  for (HardwareComponent* hardware : m_all_hardware) {
    hardware->stop();
  }
}

void thunder::HardwareManager::start_song(Song song) {
  if (m_current_song) {
    return;
  }
  
  // Set the current song.
  m_current_song = song;
  
  // Disable the falcons.
  set_falcons_enabled(false);
  
  // Load the music.
  m_orchestra_primary.LoadMusic(song.primary_chrp);
  m_orchestra_secondary.LoadMusic(song.secondary_chrp);
  
  // Play the music.
  m_orchestra_primary.Play();
  m_orchestra_secondary.Play();
}

void thunder::HardwareManager::process_orchestra() {
  if (!m_current_song) {
    return;
  }
  
  // Check the duration of the song.
  if (m_orchestra_primary.GetCurrentTime() < m_current_song->length.count()) {
    // Still playing, so keep the falcons disabled.
    set_falcons_enabled(false);
    return;
  }
  
  // Stop the music.
  m_orchestra_primary.Stop();
  m_orchestra_secondary.Stop();
  
  // Enable the falcons.
  set_falcons_enabled(true);
  
  // Clear the current song.
  m_current_song = std::nullopt;
}

bool thunder::HardwareManager::is_song_playing() {
  return m_current_song.has_value();
}

void thunder::HardwareManager::set_falcons_enabled(bool enabled) {
  for (TalonFX* talon_fx : m_talon_fxs) {
    talon_fx->set_enabled(enabled);
  }
}

thunder::HardwareManager thunder::HardwareManager::instance;
