#include"Game.h"
#include<Shader.h>
int main() {
	//Shader outshader("./shader/Particle.vs", "./shader/Particle.fs");
	Game game = Game(1280, 760);
	game.render();
	return 0;
}