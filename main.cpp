#include <GL/glut.h>
#include <stdio.h>
#include <vector>
#include <math.h>

using namespace std;

#define SCREEN_WIDTH 	1280
#define SCREEN_HEIGHT 	720
#define DEBUG			(true)

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

		float getX(){
			return this->x;
		}

		float getY(){
			return this->y;
		}

		float getZ(){
			return this->z;
		}
};

class Object{
	private:
		Point position;

	public:
		Object(float x, float y, float z){
			this->position = Point(x, y, z);
		}

		virtual void draw() {}

		void drawCubeOnCurrentPosition(float size){
			glPushMatrix();
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();

				glTranslatef(position.getX(), position.getY(), position.getZ());
				glutSolidCube(size);
			glPopMatrix();
		}

		void drawSphereOnCurrentPosition(float radius){
			glPushMatrix();
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();

				glTranslatef(position.getX(), position.getY(), position.getZ());
				glutSolidSphere(radius, 20, 20);
			glPopMatrix();
		}

		Point getPosition(){
			return position;
		}
};

class Wall : public Object{
	public:
		Wall(float x, float y, float z) : Object(x, y, z) {}

		void draw(){
			glColor3f(0.41, 0.41, 0.41);
			this->drawCubeOnCurrentPosition(1.0);
		}
};

class Ground : public Object{
	public:
		Ground(float x, float y, float z) : Object(x, y - 1.0, z) {}

		void draw(){
			glColor3f(0.38, 0.38, 0.38);
			this->drawCubeOnCurrentPosition(1.0);
		}
};

class Box : public Object{
	public:
		Box(float x, float y, float z) : Object(x, y, z) {}

		void draw(){
			glColor3f(0.87, 0.72, 0.52);
			this->drawCubeOnCurrentPosition(1.0);
		}
};

class Player : public Object{
public:
		Player(float x, float y, float z) : Object(x, y, z) {}

		void draw(){
			glColor3f(0.19, 0.19, 0.19);
			this->drawSphereOnCurrentPosition(0.5);
		}
};

class Target : public Object{
public:
		Target(float x, float y, float z) : Object(x, y, z) {}
};

class LightSource{
	private:
		Point position;
		bool isPlayerLight;
		int id;

	public:
		LightSource(float x, float y, float z, int id, bool isPlayerLight){
			this->isPlayerLight = isPlayerLight;
			this->position = Point(x, y, z);
			this->id = id;

			glEnable(GL_LIGHT0 + this->id);
		}

		void draw(){
			glPushMatrix();
				GLfloat lightPosition[] = {position.getX(), position.getY() + 1.0f, position.getZ(), 0.0};
				GLfloat lightDiffuse[] = {1.0, 1.0, 1.0, 1.0};
				GLfloat matShininess[] = {100.0};

				glLightfv(GL_LIGHT0 + this->id, GL_DIFFUSE, lightDiffuse);
				glLightfv(GL_LIGHT0 + this->id, GL_POSITION, lightPosition);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);

				glEnable(GL_LIGHT0 + this->id);
			glPopMatrix();
		}
};

class Scene{
	private:
		vector <Object*> objects;
		vector <LightSource*> lightSources;
		Object *player;
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
			lightSources = vector <LightSource*> ();

			if(file != NULL){
				int width, height;
				fscanf(file, "%d %d", &width, &height);
				for(int i = 0; i < width; i++){
					for(int j = 0; j < height; j++){
						float x = (i * 1) + 0.5, y = 0.5, z = (j * 1) + 0.5;
						int objectType;

						fscanf(file, "%d", &objectType);

						if(objectType == 1)
							objects.push_back(new Wall(x, y, z));
						else if(objectType == 2)
							objects.push_back(new Box(x, y, z));
						else if(objectType == 3){
							player = new Player(x, y, z);
							objects.push_back(new Ground(x, y, z));
							objects.push_back(player);

							lightSources.push_back(new LightSource(x, y, z, lightSources.size(), true));
						}
						else if(objectType == 4)
							objects.push_back(new Ground(x, y, z));
						else{
							objects.push_back(new Ground(x, y, z));
							lightSources.push_back(new LightSource(x, y, z, lightSources.size(), false));	
						}
					}
				}
				fclose(file);
			}
		}

		void draw(){
			// glDisable(GL_LIGHTING);

			for(int i = 0; i < lightSources.size(); i++)
				lightSources[i]->draw();

			for(int i = 0; i < objects.size(); i++)
				objects[i]->draw();

			glEnable(GL_LIGHTING);
		}

		Point getPlayerPosition(){
			return player->getPosition();
		}

		Point getCameraPositionByReference(int cameraReference){
			Point playerPosition = getPlayerPosition();
			float cameraX = 0.0, cameraY = 6.0, cameraZ = 0.0;
			float defaultOffset = 7.0;

			if(cameraReference == 0){
				cameraX = playerPosition.getX();
				cameraZ = playerPosition.getZ() - defaultOffset;
			}
			else if(cameraReference == 1){
				cameraX = playerPosition.getX() + defaultOffset;
				cameraZ = playerPosition.getZ();
			}
			else if(cameraReference == 2){
				cameraX = playerPosition.getX();
				cameraZ = playerPosition.getZ() + defaultOffset;
			}
			else{
				cameraX = playerPosition.getX() - defaultOffset;
				cameraZ = playerPosition.getZ();
			}
			return Point(cameraX, cameraY, cameraZ);
		}

		Point getFrontPointByReference(int cameraReference){
			Point playerPosition = getPlayerPosition();
			float cameraX = 0.0, cameraY = 0.5, cameraZ = 0.0;
			if(cameraReference == 0){
				cameraX = playerPosition.getX();
				cameraZ = playerPosition.getZ() + 1.0;
			}
			else if(cameraReference == 1){
				cameraX = playerPosition.getX() - 1.0;
				cameraZ = playerPosition.getZ();
			}
			else if(cameraReference == 2){
				cameraX = playerPosition.getX();
				cameraZ = playerPosition.getZ() - 1.0;
			}
			else{
				cameraX = playerPosition.getX() + 1.0;
				cameraZ = playerPosition.getZ();
			}
			return Point(cameraX, cameraY, cameraZ);
		}
};

Scene* Scene::instance = 0;
int cameraReference = 1;

void setupCamera(){
	Scene *scene = Scene::getInstance();
	Point lookPointPosition = scene->getFrontPointByReference(cameraReference);
	Point cameraPosition = scene->getCameraPositionByReference(cameraReference);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(25.0f, 16.0/9.0, 0.1, 100.0);
	gluLookAt(cameraPosition.getX(), cameraPosition.getY(), cameraPosition.getZ(), lookPointPosition.getX(), lookPointPosition.getY(), lookPointPosition.getZ(), 0, 1, 0);
}

void draw(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setupCamera();
	
	Scene *scene = Scene::getInstance();
	scene->draw();

	glutSwapBuffers();
}

void initScene(){
	glMatrixMode(GL_PROJECTION);

	glOrtho(-100.0, 100.0, -100.0, 100.0, -100.0, 100.0);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
}

void specialInput(int key, int x, int y){
	switch(key){
		case GLUT_KEY_LEFT:{
			cameraReference -= 1;
			glutPostRedisplay();
			break;
		}
		case GLUT_KEY_RIGHT:{
			cameraReference += 1;
			glutPostRedisplay();
			break;
		}
	}
	cameraReference = (cameraReference < 0) ? 3 : (cameraReference % 4);
}

int main(int argc, char *argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

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

	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("Sokoban - OpenGL");
	
	glutSpecialFunc(specialInput);
	glutDisplayFunc(draw);
	
	initScene();
	glutMainLoop();
    return 0;
}