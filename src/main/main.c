#include "./../../include/general/params.h"
#include "./../../include/general/word_arr_io.h"
#include "./../../include/main/main_drone.h"
#include "./../../include/main/main_base_station.h"
#include "./../../include/sm/udp.h"

int runBS() {
#ifndef DEFAULT_IP
	char *ch;
#endif

	/* Load parameters necessary for key establishment */
	readWordArrFromFile(privBS,    "./params/privBS.txt");
	readWordArrFromFile(pkxDrone,  "./params/pkxDrone.txt");
	readWordArrFromFile(pkyDrone,  "./params/pkyDrone.txt");

	/* Read parameters from terminal */
#ifndef DEFAULT_IP
	printf("BS\t- Process created!\n");
	printf("Please give the IP address of the drone computer: ");
	if (fgets(DEST_IP, sizeof(DEST_IP), stdin) == NULL) {
		printf("Input was invalid!");
		exit(0);
	}
	if ((ch = strrchr(DEST_IP, '\n')) != NULL)
		*ch = '\0';
#endif

	init_socket(DRONE_PORT, BS_PORT, TIMEOUT_SOC_WIN);
#ifdef LIVE_FEED
	init_live_feed(LIVE_FEED_PORT_OUT, 1, TIMEOUT_SOC_WIN);
#endif
	main_base_station(DRONE_PORT, BS_PORT);
	close_sockets();
	return 0;
}

int runDrone() {
#ifndef DEFAULT_IP
	char *ch;
#endif

	/* Load parameters necessary for key establishment */
	readWordArrFromFile(privDrone, "./params/privDrone.txt");
	readWordArrFromFile(pkxBS, 	   "./params/pkxBS.txt");
	readWordArrFromFile(pkyBS, 	   "./params/pkyBS.txt");

	/* Read parameters from terminal */
#ifndef DEFAULT_IP
	printf("Drone\t- Process created!\n");
	printf("Please give the IP address of the base station computer: ");
	if (fgets(DEST_IP, sizeof(DEST_IP), stdin) == NULL) {
		printf("Input was invalid!");
		exit(0);
	}
	if ((ch = strrchr(DEST_IP, '\n')) != NULL)
		*ch = '\0';
#endif

	init_socket(BS_PORT, DRONE_PORT, TIMEOUT_SOC_WIN);
#ifdef LIVE_FEED
	init_live_feed(LIVE_FEED_PORT_IN, 0, TIMEOUT_SOC_WIN);
#endif
	main_drone(BS_PORT, DRONE_PORT);
	close_sockets();
	return 0;
}

int main(int argc, char const *argv[]) {
	int ret;

#ifdef BS
	ret = runBS();
#endif

#ifdef DRONE
	ret = runDrone();
#endif

	return ret;
}