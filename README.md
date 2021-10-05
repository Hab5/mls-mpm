# mls-mpm

https://user-images.githubusercontent.com/20952474/125779959-7cd6cfad-ff79-463d-8c8c-fcef89bf0386.mp4

I'll make a proper readme and a makefile at some point.  
In the meantime, I just need to state that this is basically a C++, 3D, OpenGL port of https://nialltl.neocities.org/articles/mpm_guide.html
The simulation code is in `main_glm.cpp` (in free functions), the rest of the code is just opengl utilities I ported over from some other projects of mine.

#### So I don't spend 10min figuring it out next time:
`g++ -lGL -lglfw -ldl -lassimp src/*.cpp lib/glad/glad.c -I lib/ -I src/ -o mls-mpm -Ofast -march=native -mavx2 -mfma -fopenmp -g && ./mls-mpm`
