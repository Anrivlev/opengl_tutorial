apt install g++ make freeglut3 freeglut3-dev libglew-dev libglew2.2 libmagick++-dev libassimp-dev libglfw3 libglfw3-dev libmeshoptimizer-dev libmeshoptimizer1d

g++ lesson2.cpp -o lesson2 -I./Include `pkg-config --libs glew` -lglut 