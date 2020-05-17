create_bin:
	mkdir -p ./bin



build_drone_8: create_bin
	gcc -Wall -pedantic -ansi -D DRONE -D SIZE_8 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_drone
	@echo "Build Drone complete!"

build_drone_16: create_bin
	gcc -Wall -pedantic -ansi -D DRONE -D SIZE_16 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_drone
	@echo "Build Drone complete!"

build_drone_32: create_bin
	gcc -Wall -pedantic -ansi -D DRONE -D SIZE_32 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_drone
	@echo "Build Drone complete!"

build_drone_64: create_bin
	gcc -Wall -pedantic -ansi -D DRONE -D SIZE_64 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_drone
	@echo "Build Drone complete!"

run_drone:
	./bin/main_drone



build_bs_8: create_bin
	gcc -Wall -pedantic -ansi -D BS -D SIZE_8 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_bs
	@echo "Build BS complete!"

build_bs_16: create_bin
	gcc -Wall -pedantic -ansi -D BS -D SIZE_16 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_bs
	@echo "Build BS complete!"
	
build_bs_32: create_bin
	gcc -Wall -pedantic -ansi -D BS -D SIZE_32 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_bs
	@echo "Build BS complete!"
	
build_bs_64: create_bin
	gcc -Wall -pedantic -ansi -D BS -D SIZE_64 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_bs
	@echo "Build BS complete!"

run_bs:
	./bin/main_bs