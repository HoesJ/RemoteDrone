This file describes how to build and run both base station and drone. We
support builds for 8, 16, 32 and 64 bit word sizes. Base station and drone
do not necessarily need the same word size. Public and private keys are
fixed and are available in the subfolder ./params. On the base station
computer, the public key of the base station and the private key of the drone
can be deleted. On the drone computer, the public key of the drone and the
private key of the base station can be deleted.

1) Build base station or drone: make build_<entity>_<word_size>
   -> Replace <entity> by bs or drone.
   -> Replace <word_size> by 8, 16, 32 or 64.

2) Run base station or drone: make run_<entity>
   -> Replace <entity> by bs or drone.
   -> Application will ask for IP address. Enter the IP address of the
      OTHER entity. E.g. if you run base station, provide the IP address of
      the drone.

3) Operations for base station and drone (by hitting keyboard):
   -> s: start the session. Because the base station initiates the key
         establishment, it is recommended to first start the drone session
         before the base station. 
   -> q: quit the session.
   -> e: exit the application. (Ctrl-C doesn't work)

4) Operations for base station (only valid when session is active):
   -> c: ask drone for status update.
   -> f: start video feed. Because video feed is only available when the OBS
         application is installed, we transfer an image (./feed.jpg) instead.
         You can transfer your own image by replacing the file ./feed.jpg.
         The base station stores the received file in ./rec_feed.jpg. Note
         that the received file may be incomplete because no retransmissions
         are done.
   -> g: stop video feed. This will of course have no effect when the
         image is already transferred completely. When f is entered
         again, the video feed is restarted.
_____________________________________________________________________________
EXAMPLE

BS$ make build_bs_64
BS$ make run_bs
BS$ 10.87.20.93
                                                Drone$ make build_drone_32
                                                Drone$ make run_drone
                                                Drone$ 10.87.20.96
                                                Drone$ s
BS$ s
BS$ c
BS$ f
BS$ q
                                                Drone$ q
                                                Drone$ e
BS$ e



This example builds drone and base station, runs the application and starts
a session. Then a status update is requested by the base station and the base
station starts an image transfer. Afterwards, the session is quitted and the
application is exited.
