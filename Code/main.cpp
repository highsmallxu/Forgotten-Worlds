
#include <cstdlib>			// standard definitions
#include <iostream>			// C++ I/O
#include <cstdio>			// C I/O (for sprintf)
#include <cmath>			// standard definitions
#include <GLUT/GLUT.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "mesh.h"
#include "imageloader.h"
#include "glm/glm.hpp"



using namespace std;			// make std accessible



//1-load hero,gun
Mesh hero;
Mesh gun;
void init_hero(const char * fileName){
    hero.loadMesh(fileName);
}
void init_gun(const char * fileName){
    gun.loadMesh(fileName);
}

//2-load bullet texture mapping
//2.1-loadTexture

GLuint loadTexture(Image* image) {
    GLuint textureId;
    glGenTextures(1, &textureId); //Make room for our texture
    glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
    //Map the image to the texture
    glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
                 0,                            //0 for now
                 GL_RGB,                       //Format OpenGL uses for image
                 image->width, image->height,  //Width and height
                 0,                            //The border of the image
                 GL_RGB, //GL_RGB, because pixels are stored in RGB format
                 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
                 //as unsigned numbers
                 image->pixels);               //The actual pixel data
    return textureId; //Returns the id of the texture
}
//2.2-load Image
GLuint _textureId1;
GLuint _textureId2;
GLuint _textureId3;
GLUquadric *quad;
void init_bullet_texture(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    
    quad = gluNewQuadric();
    Image* image1 = loadBMP("bullet1.bmp"); //grey
    Image* image2 = loadBMP("bullet2.bmp"); //red
    Image* image3 = loadBMP("bullet2.bmp"); //hero
    _textureId1 = loadTexture(image1);  //grey
    _textureId2 = loadTexture(image2);  //red
    _textureId3 = loadTexture(image3); //hero
    delete image1;
    delete image2;
    delete image3;
}

//3-bullet control
float x=1.8;
float r;
GLuint texture;
void update(int value)
{
    r+=2.0f; //make bullet keep rotating
    if(r>360.f)
    {
        r-=360;
    }
    x+=0.1f;  //make bullet keep moving forward
    if(x>5.f)
    {
        x=1.8f;
    }
    if(texture != _textureId1) //change texture of bullet from frame to frame
    {
        texture = _textureId1;
    }
    else
    {
        texture = _textureId2;
    }
    
    
    glutPostRedisplay();
    glutTimerFunc(25,update,0);
}


//4-display function
//4.1-load bullet
double direction = 0;
double updown = 0;
double leftright = 0;

void load_bullet(){
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glRotated(direction,0,0,-10);
    glTranslatef(x,updown,-10);
    glRotatef(r,0.0f,0.0f,1.0f);
    glScaled(0.1f, 0.1f, 0.1f);
    gluQuadricTexture(quad,1);
    gluSphere(quad,2,20,20);
    
    glPopMatrix();
    

}

//4.2-load hero
void load_hero(){
    glPushMatrix();
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureId3);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glDisable(GL_TEXTURE_2D);


    glTranslatef(leftright, updown, -5);

    glRotated(45, 0, 1, 0);
    hero.draw();
    
    glPopMatrix();
}

//4.3-load gun
void load_gun(){
    glPushMatrix();
    glRotated(direction,0,0,-5);
    glTranslatef(leftright, updown,-5);
    glScaled(0.5f, 0.5f, 0.5f);
    gun.draw();
    
    glPopMatrix();
    
}


void drawScene() {
    
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //load bullet

        load_bullet();
        std::cout << "Hello, World!\n";

    glDisable(GL_TEXTURE_2D);


    //load hero
    load_hero();
    load_gun();
    
    glutSwapBuffers();
}

//5-keyboard function
void keyboard(unsigned char k, int x, int y)
{
    switch (k)
    {
        case 'a':
            direction += 5;
            break;
        case 'w':
            direction -= 5;
            break;
        case 'g':
             updown +=0.3;
            break;
        case 'h':
            updown -=0.3;
            break;
        case 'e':
            leftright +=0.3;
            break;
        case 'r':
            leftright -=0.3;
            break;
        case 'q':
            exit(0);
    }
    glutPostRedisplay();
}

//6-reshape function
void handleResize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40, (float)w / (float)h, 0.5, 20.0);
}



int main(int argc, char * argv[])
{

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800,800);
    glutInitWindowPosition(20, 20);
    glutCreateWindow(argv[0]);
    
    //1-load hero,gun
    init_hero(argc == 2 ? argv[1] : "hero.obj");
    init_gun(argc == 2 ? argv[1] : "gun.obj");
    
    //2-load bullet texture mapping
    init_bullet_texture();
    
    //3-bullet keep rotating
    glutTimerFunc(25,update,0);
    
    //4-display function
    glutDisplayFunc(drawScene);
    
    //5-keyboard function
    glutKeyboardFunc(keyboard);
    
    //6-reshape function
    glutReshapeFunc(handleResize);

//    glutIdleFunc(bul_move);
    
    glutMainLoop();
    return 0; 
}