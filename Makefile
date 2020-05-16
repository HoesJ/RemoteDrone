create_bin:
	mkdir -p ./bin



cpy_drone_key: create_bin
	cp ./params/privDrone.txt ./bin/privDrone.txt
	cp ./params/pkxBS.txt ./bin/pkxBS.txt
	cp ./params/pkyBS.txt ./bin/pkyBS.txt

build_drone_8: cpy_drone_key
	gcc -Wall -pedantic -ansi -D DRONE -D SIZE_8 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_drone
	@echo "Build Drone complete!"

build_drone_16: cpy_drone_key
	gcc -Wall -pedantic -ansi -D DRONE -D SIZE_16 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_drone
	@echo "Build Drone complete!"

build_drone_32: cpy_drone_key
	gcc -Wall -pedantic -ansi -D DRONE -D SIZE_32 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_drone
	@echo "Build Drone complete!"

build_drone_64: cpy_drone_key
	gcc -Wall -pedantic -ansi -D DRONE -D SIZE_64 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_drone
	@echo "Build Drone complete!"

run_drone:
	./bin/main_drone



cpy_bs_key: create_bin
	cp ./params/privBS.txt ./bin/privBS.txt
	cp ./params/pkxDrone.txt ./bin/pkxDrone.txt
	cp ./params/pkyDrone.txt ./bin/pkyDrone.txt

build_bs_8: cpy_bs_key
	gcc -Wall -pedantic -ansi -D BS -D SIZE_8 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_bs
	@echo "Build BS complete!"

build_bs_16: cpy_bs_key
	gcc -Wall -pedantic -ansi -D BS -D SIZE_16 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_bs
	@echo "Build BS complete!"
	
build_bs_32: cpy_bs_key
	gcc -Wall -pedantic -ansi -D BS -D SIZE_32 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_bs
	@echo "Build BS complete!"
	
build_bs_64: cpy_bs_key
	gcc -Wall -pedantic -ansi -D BS -D SIZE_64 ./src/crypto/*.c ./src/general/*.c ./src/main/*.c ./src/sm/*.c -lncurses -o ./bin/main_bs
	@echo "Build BS complete!"

run_bs:
	./bin/main_bs