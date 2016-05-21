
#include <cstdlib>			// standard definitions
#include <iostream>			// C++ I/O
#include <cstdio>			// C I/O (for sprintf)
#include <cmath>			// standard definitions
#include <GLUT/GLUT.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "traqueboule.h"
#include "mesh.h"

using namespace std;			// make std accessible
unsigned int W_fen = 800;  // largeur fenetre
unsigned int H_fen = 800;  // hauteur fenetre
Mesh hero;
Mesh bullet;


double rotAngle = 0;

void init_bull(const char * fileName){
    bullet.loadMesh(fileName);
}
void init_hero(const char * fileName){
    hero.loadMesh(fileName);
}


float bul_loc = -0.1;
void display()
{
    if(bul_loc<-1){
        bul_loc=-0.1;
    }

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glPushMatrix();
        glRotated(rotAngle, 1, 1, 1);
        glTranslatef(bul_loc, 0, 0);
        glScaled(0.05, 0.05, 0.05);
        glEnable(GL_COLOR_MATERIAL);
        glColor3f(1, 1, 1);
        bullet.draw();
        glPopMatrix();
        glFlush();
        
        std::cout << "Hello, World!\n";
        bul_loc = bul_loc - 0.02;
        

    

    
    glPushMatrix();			// save the current camera transform
    glRotated(rotAngle, 1, 1, 1);	// rotate by rotAngle about y-axis
    glScaled(0.3, 0.3, 0.3);
    glEnable(GL_COLOR_MATERIAL);	// specify object color
    glColor3f(1.0, 0.5, 0.3);		// redish
    hero.draw();		// draw the teapot
    glPopMatrix();			// restore the modelview matrix
    glFlush();				// force OpenGL to render now
    
    glutSwapBuffers();			// make the image visible
    

}



void bul_move()
{
    display();
}


void keyboard(unsigned char k, int x, int y)
{
    switch (k)
    {
        case 'a':
            rotAngle += 5;			// increase rotation by 5 degrees
            break;
        case 'l':
            rotAngle -= 5;			// decrease rotation by 5 degrees
            break;
        case 'q':
            exit(0);			// exit
    }
    
    glutPostRedisplay();		// redraw the image now
}


int main(int argc, char * argv[])
{

    glutInit(&argc,argv);
    
    init_bull(argc == 2 ? argv[1] : "bullet.obj");
    init_hero(argc == 2 ? argv[1] : "hero.obj");

    glutInitDisplayMode(GLUT_DOUBLE|GLUT_DEPTH|GLUT_RGB );
    
    glutInitWindowPosition(200, 100);
    glutInitWindowSize(W_fen,H_fen);
    glutCreateWindow(argv[0]);
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(bul_move);
    

    glutMainLoop();
    return 0; 
}