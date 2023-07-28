#!/bin/bash

if [ $EUID -ne 0 ]; then
  echo 'Must install as root'
  exit 1
fi

# Create directory.
sudo mkdir -p /var/frc1511/BizarroHomer

# Give the pi user access to it.
sudo chown pi: /var/frc1511
sudo chmod u+w /var/frc1511

# Create IPC message queue key file.
touch /var/frc1511/BizarroHomer/ipc_msg_queue_key

echo 'install.sh finished successfully'
