#!/bin/bash

sudo systemctl stop durex
sudo systemctl disable durex
sudo rm -rf /etc/systemd/system/durex.service
sudo systemctl daemon-reload
sudo systemctl reset-failed
sudo rm -rf /bin/Durex