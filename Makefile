build_drone_8:
	gcc -Wall -pedantic -ansi -D DRONE -D SIZE_8 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_drone

build_drone_16:
	gcc -Wall -pedantic -ansi -D DRONE -D SIZE_16 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_drone

build_drone_32:
	gcc -Wall -pedantic -ansi -D DRONE -D SIZE_32 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_drone

build_drone_64:
	gcc -Wall -pedantic -ansi -D DRONE -D SIZE_64 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_drone

run_drone:
	./bin/main_drone



build_bs_8:
	gcc -Wall -pedantic -ansi -D BS -D SIZE_8 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_bs

build_bs_16:
	gcc -Wall -pedantic -ansi -D BS -D SIZE_16 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_bs
	
build_bs_32:
	gcc -Wall -pedantic -ansi -D BS -D SIZE_32 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_bs
	
build_bs_64:
	gcc -Wall -pedantic -ansi -D BS -D SIZE_64 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_bs

run_bs:
	./bin/main_bs