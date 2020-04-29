#include "./../include/general/params.h"
#include "./../include/main/main_drone.h"
#include "./../include/main/main_base_station.h"
#include "./../include/sm/udp.h"

#if UNIX
int startProcesses(int argc, char const *argv[]) {
	int pipeToDrone[2];
	int pipeToBS[2];
	int pidBS, pidDrone;

#if !UDP
	if (pipe(pipeToDrone) == -1) {
		printf("Error creating pipe to drone\n");
		return 1;
	}

	if (pipe(pipeToBS) == -1) {
		printf("Error creating pipe to base station\n");
		return 1;
	}
#endif

	/* Create base station process */
	pidBS = fork();
	if (pidBS == -1) {
		printf("Error spawning base station process\n");
		return 1;
	}
	else if (pidBS == 0) {
#if !UDP
		/* Close unused pipes -- reading end of drone pipe */
		if (close(pipeToDrone[0]) == -1) {
			printf("Error closing reading end of drone pipe at BS\n");
			return 1;
		}

		/* Close unused pipes -- writing end of BS pipe */
		if (close(pipeToBS[1]) == -1) {
			printf("Error closing writing end of drone pipe at BS\n");
			return 1;
		}
#endif

		printf("BS - process created!\n");
#if !UDP
		main_base_station(pipeToDrone[1], pipeToBS[0]);     /* This is the BS process */
#else
		init_socket(DRONE_PORT, BS_PORT, TIMEOUT_SOC);
		main_base_station(DRONE_PORT, BS_PORT);
		close_sockets();
#endif
		return 0;                                           /* Exit BS process succesfully */
	}

	/* Create drone process */
	pidDrone = fork();
	if (pidDrone == -1) {
		printf("Error spawning drone process\n");
		return 1;
	}
	else if (pidDrone == 0) {
#if !UDP
		/* Close unused pipes -- reading end of BS pipe */
		if (close(pipeToBS[0]) == -1) {
			printf("Error closing reading end of BS pipe at drone\n");
			return 1;
		}

		/* Close unused pipes -- writing end of drone pipe */
		if (close(pipeToDrone[1]) == -1) {
			printf("Error closing writing end of BS pipe at drone\n");
			return 1;
		}
#endif

		printf("Drone - process created!\n");
#if !UDP
		main_drone(pipeToBS[1], pipeToDrone[0]);            /* This is the drone process */
#else
		init_socket(BS_PORT, DRONE_PORT, TIMEOUT_SOC);
		main_drone(BS_PORT, DRONE_PORT);
		close_sockets();
#endif
		return 0;                                           /* Exit the drone process succesfully */
	}

	/* We are in the parent process */

#if !UDP
	/* Close unused pipes -- reading end of drone pipe */
	if (close(pipeToDrone[0]) == -1) {
		printf("Error closing reading end of drone pipe at parent\n");
		return 1;
	}

	/* Close unused pipes -- writing end of drone pipe */
	if (close(pipeToDrone[1]) == -1) {
		printf("Error closing writing end of drone pipe at parent\n");
		return 1;
	}

	/* Close unused pipes -- reading end of BS pipe */
	if (close(pipeToBS[0]) == -1) {
		printf("Error closing reading end of BS pipe at parent\n");
		return 1;
	}

	/* Close unused pipes -- writing end of BS pipe */
	if (close(pipeToBS[1]) == -1) {
		printf("Error closing writing end of BS pipe at parent\n");
		return 1;
	}
#endif

	wait(&pidBS);
	wait(&pidDrone);
	return 0;
}
#endif

#if WINDOWS

int startBS(uint8_t* useless) {
	system("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe -c F:\\Jochem\\Documents\\Sources\\C\\RemoteDrone\\Debug\\RemoteDrone.exe BS");
}

int startDrone(uint8_t* useless) {
	system("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe -c F:\\Jochem\\Documents\\Sources\\C\\RemoteDrone\\Debug\\RemoteDrone.exe DRONE");
}

int startProcesses(int argc, char const *argv[]) {
#if UDP
	if (argc == 1) { /* In main*/
	#if (RUN_DRONE && RUN_BS)
		int ptr = 5;
		uintptr_t bs = _beginthread(startBS, 0, &ptr);
		uintptr_t drone = _beginthread(startDrone, 0, &ptr);

		WaitForSingleObject(bs, INFINITE);
		WaitForSingleObject(drone, INFINITE);
	#elif (RUN_DRONE)
		main_drone();
	#elif (RUN_BS)
		main_base_station();
	#endif
	}
	else {
		printf("%d\n", argc);
		printf("%c\n", *argv[1]);
		if (*argv[1] == 'B') {
			printf("BS starting\n");
			udp_test_sender();
		}if (*argv[1] == 'D') {
			printf("DRONE starting\n");
			udp_test_receiver();
		}
	}
#else
	struct pipe pipeBStoDrone;
	struct pipe pipeDroneToBS;

	pipeBStoDrone.readOffset = 0;
	pipeBStoDrone.writeOffset = 0;
	pipeDroneToBS.readOffset = 0;
	pipeDroneToBS.writeOffset = 0;


	/* Start BS thread */
	struct threadParam BSparam;
	BSparam.txPipe = &pipeBStoDrone;
	BSparam.rxPipe = &pipeDroneToBS;
	uintptr_t bs = _beginthread(main_base_station_win, 0, &BSparam);


	/* Start drone thread */
	struct threadParam droneparam;
	droneparam.txPipe = &pipeDroneToBS;
	droneparam.rxPipe = &pipeBStoDrone;
	uintptr_t drone = _beginthread(main_drone_win, 0, &droneparam);

	WaitForSingleObject(bs, INFINITE);
	WaitForSingleObject(drone, INFINITE);
	printf("MAIN HERE");
	return 0;
#endif
}
#endif

int main(int argc, char const *argv[]) {
	return startProcesses(argc, argv);
}