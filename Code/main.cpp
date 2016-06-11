//
//  main.cpp
//  shooting
//
//  Created by GAOXiaoXu on 6/7/16.
//  Copyright © 2016 GAOXiaoXu. All rights reserved.
//

#include <iostream>
#include "headers.h"

GLuint elephant;
float elephantrot;
char ch='1';


void loadObj(char *fname)
{
    FILE *fp;
    int read;
    GLfloat x, y, z;
    char ch;
    elephant=glGenLists(1);
    fp=fopen(fname,"r");
    if (!fp)
    {
        printf("can't open file %s\n", fname);
        exit(1);
    }
    glPointSize(2.0);
    glNewList(elephant, GL_COMPILE);
    {
        glPushMatrix();
        glBegin(GL_POINTS);
        while(!(feof(fp)))
        {
            read=fscanf(fp,"%c %f %f %f",&ch,&x,&y,&z);
            if(read==4&&ch=='v')
            {
                glVertex3f(x,y,z);
            }
        }
        glEnd();
    }
    glPopMatrix();
    glEndList();
    fclose(fp);
}


void drawElephant()
{
    glPushMatrix();
    glTranslatef(0,-40.00,-105);
    glColor3f(1.0,0.23,0.27);
    glScalef(0.1,0.1,0.1);
    glRotatef(elephantrot,0,1,0);
    glCallList(elephant);
    glPopMatrix();
    elephantrot=elephantrot+0.6;
    if(elephantrot>360)elephantrot=elephantrot-360;
}

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

GLuint _textureId1;
GLUquadric *quad;
void init_bullet_texture(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    
    quad = gluNewQuadric();
    Image* image1 = loadBMP("hero.bmp"); //grey
    _textureId1 = loadTexture(image1);  //grey

    delete image1;

}


//////////////

Mesh hero;
void load_hero(const char * fileName){
    hero.loadMesh(fileName);
}


void init()
{
    glClearColor(0, 0, 0, 0);		// background color
    glClearDepth(1.0);			// background depth value
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 1000);	// setup a perspective projection
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluLookAt(				// set up the camera
              0.0, 0.0, 5.0,		// eye position
              0.0, 0.0, 0.0,		// lookat position
              0.0, 1.0, 0.0);		// up direction
    
    glEnable(GL_DEPTH_TEST);		// enable hidden surface removal
    
    glEnable(GL_LIGHTING);		// enable lighting
    glEnable(GL_LIGHT0);		// enable
    
    float lpos[] = { 5, 5, 5, 0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
    
    // glShadeModel(GL_FLAT);		// flat shading
    glShadeModel(GL_SMOOTH);		// smooth shading
}



 void display(){
     
     glClear(GL_COLOR_BUFFER_BIT |		// clear the frame buffer (color)
             GL_DEPTH_BUFFER_BIT);		// clear the depth buffer (depths)
     
     glPushMatrix();			// save the current camera transform
        glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, _textureId1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glDisable(GL_TEXTURE_2D);
        glRotated(7, 0, 1, 0);	// rotate by rotAngle about y-axis
        glEnable(GL_COLOR_MATERIAL);	// specify object color
     hero.draw();
     glPopMatrix();			// restore the modelview matrix
     
     glFlush();				// force OpenGL to render now
     glutSwapBuffers();			// make the image visible

}



int main(int argc, char * argv[]) {
    
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800,800);
    glutInitWindowPosition(20, 20);
    glutCreateWindow(argv[0]);
    
    load_hero(argc == 2 ? argv[1] : "hero.obj");

    glutDisplayFunc(display);
    glutIdleFunc(display);
    
    init();
    
    
    glutMainLoop();
    return 0;

}
