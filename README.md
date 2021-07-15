# mls-mpm
`g++ -lGL -lglfw -ldl -lassimp src/*.cpp lib/glad/glad.c -I lib/ -I src/ -o mls-mpm -Ofast -march=native -mavx2 -mfma -fopenmp -g && perf record -g ./mls-mpm`
