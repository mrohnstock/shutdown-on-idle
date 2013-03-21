#!/bin/bash
cd /opt/shutdown-on-idle
su -c "/usr/bin/node start.js &" root
