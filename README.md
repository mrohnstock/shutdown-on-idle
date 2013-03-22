Shutdown on Idle (SoI)
======================

A daemon, which listen on tcp/ip connections and logfiles, to determine, if server could get shutted down.

+ Install requirements ([node](https://github.com/joyent/node), [node-gyp](https://github.com/TooTallNate/node-gyp), [express](https://github.com/visionmedia/express), [node-mime](https://github.com/broofa/node-mime), [twig.js](https://github.com/justjohn/twig.js) and `g++`)
+ Compile with `make`
+ copy `soi.node`, `start.sh` and `start.js` to `/opt/shutdown-on-idle`
+ copy `soi.service` to `/usr/lib/systemd/system`
+ execute `systemctl --system daemon-reload` and `systemctl start soi`
+ navigate to `http://HOST:12345` to control SoI
