#include "./../include/general/params.h"
#include "./../include/main/main_drone.h"
#include "./../include/main/main_base_station.h"

#if UNIX
int startProcesses() {
	int pipeToDrone[2];
	int pipeToBS[2];
	int pidBS, pidDrone;

	if (pipe(pipeToDrone) == -1) {
		printf("Error creating pipe to drone\n");
		return 1;
	}

	if (pipe(pipeToBS) == -1) {
		printf("Error creating pipe to base station\n");
		return 1;
	}

	/* Create base station process */
	pidBS = fork();
	if (pidBS == -1) {
		printf("Error spawning base station process\n");
		return 1;
	}
	else if (pidBS == 0) {
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

		printf("BS - process created!\n");
		main_base_station(pipeToDrone[1], pipeToBS[0]);     /* This is the BS process */
		return 0;                                           /* Exit BS process succesfully */
	}

	/* Create drone process */
	pidDrone = fork();
	if (pidDrone == -1) {
		printf("Error spawning drone process\n");
		return 1;
	}
	else if (pidDrone == 0) {
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

		printf("Drone - process created!\n");
		main_drone(pipeToBS[1], pipeToDrone[0]);            /* This is the drone process */
		return 0;                                           /* Exit the drone process succesfully */
	}

	/* We are in the parent process */

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

	wait(&pidBS);
	wait(&pidDrone);
	return 0;
}
#endif

#if WINDOWS
int startProcesses() {
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
}
#endif

int main(int argc, char const *argv[]) {
	return startProcesses();
}