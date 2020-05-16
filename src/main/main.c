#include <termios.h>
#include <stdlib.h>
#include "./../../include/general/params.h"
#include "./../../include/general/word_arr_io.h"
#include "./../../include/main/main_drone.h"
#include "./../../include/main/main_base_station.h"
#include "./../../include/sm/udp.h"

/* Store IO settings */
static struct termios term_settings;

void RestoreKeyboardBlocking(struct termios *initial_settings) {
	tcsetattr(0, TCSANOW, initial_settings);
}

void SetKeyboardNonBlock(struct termios *initial_settings) {
    struct termios new_settings;
    tcgetattr(0, initial_settings);

    memcpy(&new_settings, initial_settings, sizeof(struct termios));
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_lflag &= ~ISIG;
    new_settings.c_cc[VMIN] = 0;
    new_settings.c_cc[VTIME] = 0;

    tcsetattr(0, TCSANOW, &new_settings);
}

int runBS() {
	char *ch;

	/* Load parameters necessary for key establishment */
	readWordArrFromFile(privBS,    "./bin/privBS.txt");
	readWordArrFromFile(pkxDrone,  "./bin/pkxDrone.txt");
	readWordArrFromFile(pkyDrone,  "./bin/pkyDrone.txt");

	/* Read parameters from terminal */
	printf("BS\t- process created!\n");
	printf("Please give the IP address of the drone computer: ");
	if (fgets(DEST_IP, sizeof(DEST_IP), stdin) == NULL) {
		printf("Input was invalid!");
		exit(0);
	}
	if ((ch = strrchr(DEST_IP, '\n')) != NULL)
		*ch = '\0';

	SetKeyboardNonBlock(&term_settings);
	init_socket(DRONE_PORT, BS_PORT, TIMEOUT_SOC_UNIX);
#if LIVE_FEED_PORT_IN
	init_live_feed(LIVE_FEED_PORT_OUT, 1, TIMEOUT_SOC_UNIX);
#endif
	main_base_station(DRONE_PORT, BS_PORT);
	close_sockets();
	return 0;
}

int runDrone() {
	char *ch;

	/* Load parameters necessary for key establishment */
	readWordArrFromFile(privDrone, "./bin/privDrone.txt");
	readWordArrFromFile(pkxBS, 	   "./bin/pkxBS.txt");
	readWordArrFromFile(pkyBS, 	   "./bin/pkyBS.txt");

	/* Read parameters from terminal */
	printf("Drone\t- process created!\n");
	printf("Please give the IP address of the base station computer: ");
	if (fgets(DEST_IP, sizeof(DEST_IP), stdin) == NULL) {
		printf("Input was invalid!");
		exit(0);
	}
	if ((ch = strrchr(DEST_IP, '\n')) != NULL)
		*ch = '\0';

	SetKeyboardNonBlock(&term_settings);
	init_socket(BS_PORT, DRONE_PORT, TIMEOUT_SOC_UNIX);
#if LIVE_FEED_PORT_IN
	init_live_feed(LIVE_FEED_PORT_IN, 0, TIMEOUT_SOC_UNIX);
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

	RestoreKeyboardBlocking(&term_settings);
	return ret;
}