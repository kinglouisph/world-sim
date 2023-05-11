compile:
	gcc main.c glad/src/glad.c -osim -O2 -lm -lglfw -lGL -lX11 -lpthread -ldl -Iglad/include