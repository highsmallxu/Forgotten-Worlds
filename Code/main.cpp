
#include <cstdlib>			// standard definitions
#include <iostream>			// C++ I/O
#include <cstdio>			// C I/O (for sprintf)
#include <cmath>			// standard definitions
#include <GLUT/GLUT.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <new>
#include "mesh.h"
#include <dirent.h>
#include "imageloader.h"
#include "traqueboule.h"

using namespace std;			// make std accessible
#define PI 3.14159265
#define HERO_SIZE 10

//****define variables****//

///////background//////////

GLuint background_texture;
float backgroundX;

///////objects////////////

//Mesh heros[HERO_SIZE];
std::vector<Mesh> heros;
Mesh hero;
Mesh bird;
Mesh boss;
Mesh axe;
Mesh sun;

///////lighting////////

float lpos[] = { 1, 0.8, 1, 0 };
std::vector<Vec3Df> lighting;
std::vector<Vec3Df> LightPos;
std::vector<Vec3Df> LightColor;
Vec3Df CamPos = Vec3Df(0.0f,0.0f,4.0f);

///////texture///////

GLuint _textureId1;
GLuint _textureId2;
GLuint _textureId3;
GLuint _textureId4;
GLuint texture;
GLUquadric *quad;

///////move control///////

float x=0;
float bird_up=0;
float bird_on=3;
float boss_on=3;
float r;
float count_number=0;
bool up=true;
bool down=false;
bool bird1 = true;
bool bird2 = true;
bool bird3 = true;
bool boss_stop = false;
bool new_bullet = false;
double direction = 90;
double updown = 0;
double leftright = 0;


//****load bmp [background]****
GLuint bmp_texture_load(const char *filename, int width, int height)
{
    GLuint texture;
    unsigned char * data;
    FILE * file;
    file = fopen( filename, "rb" );
    if ( file == NULL )
    {
        std::cout<<"File not found";
        return 0;
    }
    data = (unsigned char *)malloc( width * height * 3 );
    //int size = fseek(file,);
    fread( data, width * height * 3, 1, file );
    fclose( file );
    for(int i = 0; i < width * height ; ++i)
    {
        int index = i*3;
        unsigned char B,R;
        B = data[index];
        R = data[index+2];
        
        data[index] = R;
        data[index+2] = B;
    }
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,GL_RGB, GL_UNSIGNED_BYTE, data );
    free( data );
    std::cout<<"Texture ID: "<<texture;
    return texture;
}

void init_background()
{
    background_texture = bmp_texture_load("background.bmp", 2040, 768);
    backgroundX = 0.0;
}
//0.3-compute lighting


Vec3Df computeLighting(Vec3Df & vertexPos, Vec3Df & normal, unsigned int light, unsigned int index)
{
    
    Vec3Df lightv = LightPos[0] - vertexPos;
    lightv.normalize();
    
    Vec3Df V = getCameraPosition() - vertexPos;
    V.normalize();
    Vec3Df H = (V + lightv) / 2;
    H.normalize();
    float dLighting = Vec3Df::dotProduct(normal, lightv);
    float sLighting = Vec3Df::dotProduct(H, normal);
    sLighting = pow(sLighting, 3);
    
    return Vec3Df(dLighting+sLighting,dLighting+sLighting,dLighting+sLighting);
}
void computeLighting(Mesh mesh_c)
{
    std::vector<Vec3Df> *result=&lighting;
    for (unsigned int i=0; i<mesh_c.vertices.size();++i)
    {
        (*result)[i]=Vec3Df();
        for (int l=0; l<LightPos.size();++l)
            (*result)[i]+=computeLighting(mesh_c.vertices[i].p, mesh_c.vertices[i].n, l, i);
    }
}

std::vector<Vec3Df> LightPos_sun;
std::vector<Vec3Df> lighting_sun;
//0.3-compute lighting - sun
Vec3Df computeLighting_sun(Vec3Df & vertexPos, Vec3Df & normal, unsigned int light, unsigned int index)
{
    
    Vec3Df lightv = LightPos_sun[0] - vertexPos;
    Vec3Df camerav = CamPos - vertexPos;
    Vec3Df h = (lightv + camerav)/2;
    h.normalize();
    float  H = Vec3Df::dotProduct(h, normal);
    float  H_e = pow(H,2); //higher-focus
    return Vec3Df(H_e,H_e,H_e);
}
void computeLighting_sun(Mesh mesh_c)
{
    std::vector<Vec3Df> *result=&lighting_sun;
    for (unsigned int i=0; i<mesh_c.vertices.size();++i)
    {
        (*result)[i]=Vec3Df();
        for (int l=0; l<LightPos_sun.size();++l)
            (*result)[i]+=computeLighting_sun(mesh_c.vertices[i].p, mesh_c.vertices[i].n, l, i);
    }
}





//1-load hero,gun

void init_bird(const char * fileName){
    bird.loadMesh(fileName);
}
void init_boss(const char * fileName){
    boss.loadMesh(fileName);
    lighting.resize(boss.vertices.size());
    LightPos.push_back(Vec3Df(1,2,7));
    LightColor.push_back(Vec3Df(0,1,0));
    computeLighting(boss);
}
void init_axe(const char * fileName){
    axe.loadMesh(fileName);
}
void init_sun(const char * fileName){
    sun.loadMesh(fileName);
    lighting_sun.resize(sun.vertices.size());
    LightPos_sun.push_back(Vec3Df(100,100,100));
    computeLighting_sun(sun);
}


DIR *dir;
struct dirent *ent;
vector<string> hero_names;
string path = "animation/";
int file_no = 1;
int hero_n = 1;
Mesh hero2;
Mesh *heross = new Mesh[100];
Mesh test;
void init_hero(){
    if((dir = opendir("animation"))!=NULL){
        while ((ent = readdir (dir)) != NULL) {
            if(file_no>3){
                string hero_name = path + ent->d_name;
                hero_names.push_back(hero_name);
                hero.loadMesh(hero_name.c_str());
//                test.loadMesh("hero.obj");

            }
            file_no+=1;
        }
        closedir (dir);
    }
    for (hero_n=1;hero_n<=hero_names.size();hero_n++){
        heross[hero_n].loadMesh(hero_names[hero_n].c_str());
        heross[hero_n]=heross[hero_n];
    }
    
    

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

void init_bullet_texture(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    
    quad = gluNewQuadric();
    Image* image1 = loadBMP("bullet1.bmp"); //grey
    Image* image2 = loadBMP("bullet2.bmp"); //red
    Image* image3 = loadBMP("hero.bmp"); //hero
    Image* image4 = loadBMP("skull.bmp");//earth
    _textureId1 = loadTexture(image1);  //grey
    _textureId2 = loadTexture(image2);  //red
    _textureId3 = loadTexture(image3); //hero
    _textureId4 = loadTexture(image4); //earth
    delete image1;
    delete image2;
    delete image3;
    delete image4;
}


//3-bullet control


void update(int value)
{
    
    //read hero animation
    if(hero_n<40){
        hero_n+=1;
    }
    else{
        hero_n=1;
    }
    
    if(hero_n==34){
        new_bullet = true;
        x=0;
    }

    
    //bullet keep rotating
    r+=2.0f;
    if(r>360.f)
    {
        r-=360;
    }
    
    //bullet keep moving forward
    x+=0.2f;
    
    //bullet change texture
    if(texture != _textureId1)
    {
        texture = _textureId1;
    }
    else
    {
        texture = _textureId2;
    }
    
    //backgfround keep moving
    backgroundX += 0.0015;
    
    
    //bird keep moving up and down
    if(up && !down)
    {
        bird_up+=0.02;
        if(bird_up>1){
            up=false;
            down=true;
        }
    }
    else if (!up && down)
    {
        bird_up-=0.02;
        if(bird_up<-1){
            up=true;
            down=false;
        }
    }
    
    //bird show up and moving forward
    if(count_number>10){
        bird_on-=0.02;
        if(bird_on<0){
            bird_on=0;
        }
    }
    
    //bird disapper
    double bullet_angle = tan ( direction * PI / 180.0 );
    if(bird_up/1.6-bullet_angle<0.1 && bird_up/1.6-bullet_angle>-0.1){
        bird1=false;
    }
    else if(bird_up/2.6-bullet_angle<0.1 && bird_up/2.6-bullet_angle>-0.1){
        bird2=false;
    }
    else if(bird_up/3.6-bullet_angle<0.1 && bird_up/3.6-bullet_angle>-0.1){
        bird3=false;
    }
    
    //boss show up
    if(!bird1 && !bird2 && !bird3){
        if(!boss_stop){
        boss_on-=0.02;
            if(boss_on<1.6){
                boss_on=1.6;
                boss_stop=true;
            }
        }
    }

    count_number+=1;
    glutPostRedisplay();
    glutTimerFunc(30,update,0);
}


//4-display function
//4.0-draw light
void draw_light(){
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    //turn on the light
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, _textureId1);
    
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 5.0 };
    GLfloat light_position[] = { -0.5, 0.0, 1.0, 0.0 };
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_SMOOTH);
    
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    
    glPushMatrix();
    glColor3f(1, 1, 0);
        glTranslatef(0.7,0.8,1);
        glRotated(r, 1, 0, 0);
        glScaled(0.1, 0.1, 0.1);
        sun.drawSmooth();
    glPopMatrix();
    
    glPopAttrib();
}
//4.1-load bullet
void load_bullet(){
    
    if(new_bullet){
        //round bullet
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        glTranslated(-1.5, 0.3, -1);
        glRotated(direction,0,0,1);
        glTranslatef(x,0,0);
        glScaled(0.05f, 0.02f, 0.02f);
        gluQuadricTexture(quad,3);
        gluSphere(quad,2,20,20);
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
    }

}

//4.2-load hero
Mesh hero_d;
void load_hero(){
    glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, _textureId3);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTranslated(-1.6,0,0);
        glRotated(100, 0, 1, 0);

        hero_d = heross[hero_n];
        hero_d.drawSmooth();
    glPopMatrix();
}


//4.3-load bird
void load_bird(){
    if(bird1){
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glPushMatrix();
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, _textureId4);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);
            glTranslated(bird_on,bird_up,0);
            glRotatef(270,0.0f,1.0f,0.0f);
            glScaled(0.3, 0.3, 0.3);
//            bird.drawWithColors(lighting_sun, 1);
        bird.drawSmooth();
        glPopMatrix();
        glPopAttrib();
    }
    if(bird2){
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glPushMatrix();
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, _textureId4);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTranslated(bird_on+1,bird_up-0.5,0);
            glRotatef(270,0.0f,1.0f,0.0f);
            glScaled(0.3, 0.3, 0.3);
            bird.drawSmooth();
        glPopMatrix();
        glPopAttrib();
    }
    if(bird3){
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glPushMatrix();
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, _textureId4);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTranslated(bird_on+1.5,bird_up+0.5,0);
            glRotatef(270,0.0f,1.0f,0.0f);
            glScaled(0.3, 0.3, 0.3);
            bird.drawSmooth();
        glPopMatrix();
        glPopAttrib();
    }
}

//4.4-load boss
void load_boss(){
    if(!bird1 && !bird2 && !bird3){
        glPushMatrix();
            glRotated(0, 0, 1, 0);
            glTranslated(boss_on, 0, 0);
            boss.drawWithColors(lighting,1);
        glPopMatrix();
    }
}

//4.5-load axe
void load_axe(){
    if(!bird1 && !bird2 && !bird3){
        glPushMatrix();
            glTranslated(boss_on, 1,0);
            glRotatef(r, 0, 1, 0);
            axe.drawWithColors(lighting, 1);
        glPopMatrix();
    }
}

//4.5-load background
void load_background()
{
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glLoadIdentity();
        glDisable(GL_LIGHTING);
        glColor3f(1,1,1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, background_texture);
    
    // Draw a textured quad
    glBegin(GL_QUADS);
    glTexCoord2d(backgroundX, 0); glVertex2f(-1.0,-1.0);
    glTexCoord2d(backgroundX + 1, 0); glVertex2f(1.0,-1.0);
    glTexCoord2d(backgroundX + 1, 1); glVertex2f(1.0, 1.0);
    glTexCoord2d(backgroundX, 1); glVertex2f(-1.0, 1.0);
    glEnd();
    
    
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
}

void drawScene() {
    
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT |		// clear the frame buffer (color)
            GL_DEPTH_BUFFER_BIT);		// clear the depth buffer (depths)
    load_background();
    glEnable(GL_DEPTH_TEST);


    draw_light();
    
    glEnable(GL_LIGHTING);
    GLfloat light1_position[] = {1.0,1.0,1.0,1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, light1_position);
    glEnable(GL_LIGHT0);
    load_hero();
    load_bullet();
    
    if(count_number>10){
        load_bird();
    }

    load_boss();
    load_axe();
    
    glFlush();
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

//7-intialization
void init()
{
    glClearColor(0, 0, 0, 0);		// background color
    glClearDepth(1.0);			// background depth value
    
    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
    gluPerspective(60, 1, 1, 100);	// setup a perspective projection
    
    glMatrixMode(GL_MODELVIEW);  //tricky!!!!!!
//    glLoadIdentity();
    
    gluLookAt(				// set up the camera
              0.0, 0.0, 4.0,		// eye position
              0.0, 0.0, 0.0,		// lookat position
              0.0, 1.0, 0.0);		// up direction
    
    glEnable(GL_DEPTH_TEST);		// enable hidden surface removal

}

int main(int argc, char * argv[])
{
    
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200,800);
    glutInitWindowPosition(20, 20);
    glutCreateWindow(argv[0]);
    
    //0-load background
    init_background();
    
    //1-load hero,gun
    init_hero();
    init_bird(argc == 2 ? argv[1] : "bird.obj");
    init_boss(argc == 2 ? argv[1] : "boss.obj");
    init_axe(argc == 2 ? argv[1] : "axe.obj");
    init_sun(argc == 2 ? argv[1] : "sun.obj");
    
    //2-load bullet texture mapping
    init_bullet_texture();
    
    //3-bullet keep rotating
    glutTimerFunc(30,update,0);
    
    //4-display function
    glutDisplayFunc(drawScene);
    
    //5-keyboard function
    glutKeyboardFunc(keyboard);
    
    //6-reshape function
    glutReshapeFunc(handleResize);
    glutIdleFunc(drawScene);
    
    //7-intialization
    init();
    
    //    glutIdleFunc(bul_move);
    
    glutMainLoop();
    return 0; 
}