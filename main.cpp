#include <GL/glut.h>
#include <stdio.h>
#include <vector>
#include <math.h>

using namespace std;

#define SCREEN_WIDTH 	680
#define SCREEN_HEIGHT 	440
#define DEBUG			true

class Point{
	private:
		float x, y, z;

	public:
		Point(){
			this->x = this->y = this->z = 0.0;
		}

		Point(float x, float y, float z){
			this->x = x;
			this->y = y;
			this->z = z;
		}
};

class Object{
	private:
		Point position;

	public:
		Object(float x, float y, float z){
			this->position = Point(x, y, z);
		}

		virtual void draw(){
			if(DEBUG)
				printf("[Object] Virtual draw method called.\n");
		}
};

class Wall : public Object{
	public:
		Wall(float x, float y, float z) : Object(x, y, z){
			if(DEBUG)
				printf("[Object] Criando uma parede em (%f, %f, %f).\n", x, y, z);
		}

		void draw(){
			if(DEBUG)
				printf("[Wall] Desenhando uma parede!\n");
		}
};

class Ground : public Object{
	public:
		Ground(float x, float y, float z) : Object(x, y - 1.0, z){
			if(DEBUG)
				printf("[Object] Criando um chão em (%f, %f, %f).\n", x, y - 1.0, z);	
		}
};

class Box : public Object{
	public:
		Box(float x, float y, float z) : Object(x, y, z){
			if(DEBUG)
				printf("[Object] Criando uma caixa em (%f, %f, %f).\n", x, y - 1.0, z);		
		}	
};

class Player : public Object{
public:
		Player(float x, float y, float z) : Object(x, y, z){
			if(DEBUG)
				printf("[Object] Criando o jogador em (%f, %f, %f).\n", x, y - 1.0, z);		
		}
};

class Target : public Object{
public:
		Target(float x, float y, float z) : Object(x, y, z){
			if(DEBUG)
				printf("[Object] Criando objetivo em (%f, %f, %f).\n", x, y - 1.0, z);		
		}	
};

class Scene{
	private:
		vector <Object*> objects;
		static Scene* instance;

		Scene(){}

	public:
		static Scene* getInstance(){
			if(instance == 0)
				instance = new Scene();

			return instance;
		}

		void init(char *fileName){
			FILE *file = fopen(fileName, "r+");
			objects = vector <Object*> ();

			if(file != NULL){
				int width, height;
				fscanf(file, "%d %d", &width, &height);
				for(int i = 0; i < width; i++){
					for(int j = 0; j < height; j++){
						float x = (i * 1) + 0.5;
						float z = (j * 1) + 0.5;
						float y = 0.5;

						int objectType;

						fscanf(file, "%d", &objectType);

						if(objectType == 1)
							objects.push_back(new Wall(x, y, z));
						else if(objectType == 2)
							objects.push_back(new Box(x, y, z));
						else if(objectType == 3)
							objects.push_back(new Player(x, y, z));
						else if(objectType == 4)
							objects.push_back(new Ground(x, y, z));
						
					}
				}
				fclose(file);
			}
		}

		void draw(){
			for(int i = 0; i < objects.size(); i++)
				objects[i]->draw();
		}
};

Scene* Scene::instance = 0;

void draw(){
	glClear(GL_COLOR_BUFFER_BIT);
	
	Scene *scene = Scene::getInstance();
	scene->draw();

	glFlush();
}

void initScene(){
	glMatrixMode(GL_PROJECTION);

	glOrtho(-5.0, 20.0, -5.0, 20.0, -5.0, 20.0);

	glClearColor(0.0, 0.0, 0.0, 1.0);
}

int main(int argc, char *argv[]){
	glutInit(&argc, argv);
	if(argc != 2){
		printf("Uso correto: %s [mapa-da-fase.txt]\n", argv[0]);
		return 1;
	}
	else{
		Scene *scene = Scene::getInstance();
		scene->init(argv[1]);
	}

	int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitWindowPosition((screenWidth - SCREEN_WIDTH) / 2, (screenHeight - SCREEN_HEIGHT) / 2);

	glutInitWindowSize(680, 440);
	glutCreateWindow("Sokoban - OpenGL");
	
	glutDisplayFunc(draw);
	
	initScene();
	glutMainLoop();
    return 0;
}