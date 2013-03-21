#!/bin/bash
cd /opt/shutdown-on-idle
su -c "/usr/bin/node starter.js &" root
