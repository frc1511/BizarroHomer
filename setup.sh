#!/bin/bash

# Check if root.
if [ $EUID -ne 0 ]; then
  echo 'Setup script must be run as root'
  exit 1
fi

# Check machine.
if [ ! $(uname -o) = 'GNU/Linux' ] || [ ! $(uname -m) = 'armv7l' ]; then
  echo 'Must only be setup on a Raspberry Pi'
  exit 1
fi

# Install Required Tools.
sudo apt-get update && sudo apt-get install -y --no-install-recommends \
  bluez \
  libbluetooth-dev \
  libsdl2-dev

# Create work folder.
sudo mkdir -p /var/frc1511/BizarroHomer

# Give pi user permissions.
sudo chown pi: /var/frc1511
sudo chmod u+w /var/frc1511

# Create IPC Message Queue Key file.
touch /var/frc1511/BizarroHomer/ipc_msg_queue_key

# Start bluetooth
sudo systemctl enable bluetooth
sudo systemctl start bluetooth
