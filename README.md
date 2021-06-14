# SteamVR Launcher

### Objective
Make a push-button device that can turn a computer on if it is off, and, once turned on, launch SteamVR so that you can immediately put on the headset without needed to interact with the keyboard and monitor at all.

**Crucial Note:** Making SteamVR launch automatically with the same push-button is doable, but isn't seamless. If you're using a Valve Index, you can actually launch SteamVR just by turning your controllers on. I reccomend that approach if it's available to you.

This is very much a quick hobbyist project, so it's not a polished package. If you have questions, I'm happy to help where I can.

### Use
The software has two parts - code for a push-button device, and code to run on the receiving computer.

#### Device Side
Edit the .ino file in the _client_ folder to include your Wifi Network and password, as well as the host IP address and MAC address. Load the file onto a Wi-Fi capable, Arduino-compatible board. Connect a push-button and, if you'd like visual feedback, 4 LEDs as suggested by the code. All of the code runs at startup, and then the board goes into "Deep Sleep". It is reawoken by pushing the push button.

#### Server Side
First, edit your PC's BIOS to accept a Wake-On-LAN signal. Every computer is different, but guides online should get you close enough for most PCs. With this done, and the device side set up properly, you should be able to turn your computer on by pushing that button.

The device can also launch steam if the FastAPI server defined in _server/api.py_ is running. **However** Windows purposefully siloes services and interactive GUI-based apps meant to run in a user context (for understandable security reasons), so, while you can run the server immediately on startup (and before any user logs in), the server will error when you try to get it to launch the app. There are some workarounds here, but they have security vulnerabilites associated with them. I decided it wasn't worth it.

As noted above, you can launch SteamVR just by turning the controllers on. If you'd still like to use this server, here's my approach:

The Powershell script _install.ps1_ is meant to move the important files into a convenient semi-permanent place for later access. Run it to copy those files.
Then you can set _start\_server.bat_ to be run on user login. It should handle all the required packages, as long as you have python installed.