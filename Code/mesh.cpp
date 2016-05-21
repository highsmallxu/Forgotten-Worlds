#include "mesh.h"
#include <GLUT/GLUT.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>


Mesh::Mesh(){}
using namespace std;

/************************************************************
 * Fonction de calcul du cube englobant le maillage
 ************************************************************/
void Mesh::computeBoundingCube () {
    //A completer

    Vec3Df bbCenter = Vec3Df(0.,0.,0.);
    for  (unsigned int i = 0; i < vertices.size (); i++)
        bbCenter += vertices[i].p;
    bbCenter /= vertices.size ();
    float bbHalfEdge = Vec3Df::distance (vertices[0].p, bbCenter);
    for (unsigned int i = 0; i < vertices.size (); i++){
        float m = Vec3Df::distance (vertices[i].p, bbCenter);
        if (m > bbHalfEdge)
            bbHalfEdge = m;
    }

    //Elargissement
    bbHalfEdge += 0.01;

    bbMinPos = Vec3Df(bbCenter[0] - bbHalfEdge, bbCenter[1] - bbHalfEdge, bbCenter[2] - bbHalfEdge);
    bbEdgeSize = bbHalfEdge*2.;
    cout <<"bb min pos : " << bbMinPos << endl;
    cout <<"bb cube edge size : " << bbEdgeSize << endl;
}



/************************************************************
 * Fonctions de calcul des normales pour chaque sommet
 ************************************************************/
void Mesh::computeVertexNormals () {
    //initialisation des normales des vertex
    for (unsigned int i = 0; i < vertices.size (); i++)
        vertices[i].n = Vec3Df (0.0, 0.0, 0.0);

    //Somme des normales du 1 voisinage du vertex
    for (unsigned int i = 0; i < triangles.size (); i++) {
        Vec3Df edge01 = vertices[triangles[i].v[1]].p -  vertices[triangles[i].v[0]].p;
        Vec3Df edge02 = vertices[triangles[i].v[2]].p -  vertices[triangles[i].v[0]].p;
        Vec3Df n = Vec3Df::crossProduct (edge01, edge02);
        n.normalize ();
        for (unsigned int j = 0; j < 3; j++)
            vertices[triangles[i].v[j]].n += n;
    }

    //Normalisation
    for (unsigned int i = 0; i < vertices.size (); i++)
        vertices[i].n.normalize ();
}

/************************************************************
 * Recentre et ajuste la taille des sommets du maillage
 ************************************************************/
void Mesh::centerAndScaleToUnit () {
    Vec3Df c;
    for  (unsigned int i = 0; i < vertices.size (); i++)
        c += vertices[i].p;
    c /= vertices.size ();
    float maxD = Vec3Df::distance (vertices[0].p, c);
    for (unsigned int i = 0; i < vertices.size (); i++){
        float m = Vec3Df::distance (vertices[i].p, c);
        if (m > maxD)
            maxD = m;
    }
    for  (unsigned int i = 0; i < vertices.size (); i++)
        vertices[i].p = (vertices[i].p - c) / maxD;
}


/************************************************************
 * Fonctions de dessin
 ************************************************************/
void Mesh::drawSmooth(){

    glBegin(GL_TRIANGLES);

    for (int i=0;i<triangles.size();++i)
    {
        for(int v = 0; v < 3 ; v++){
            glNormal3f(vertices[triangles[i].v[v]].n[0], vertices[triangles[i].v[v]].n[1], vertices[triangles[i].v[v]].n[2]);
            glVertex3f(vertices[triangles[i].v[v]].p[0], vertices[triangles[i].v[v]].p[1] , vertices[triangles[i].v[v]].p[2]);
        }

    }
    glEnd();
}

void Mesh::drawWithColors(const std::vector<Vec3Df> & colors){

    glBegin(GL_TRIANGLES);

    for (int i=0;i<triangles.size();++i)
    {
        for(int v = 0; v < 3 ; v++){
			const Vec3Df & color = colors[triangles[i].v[v]];
            glColor3f(color[0], color[1], color[2]);
			glNormal3f(vertices[triangles[i].v[v]].n[0], vertices[triangles[i].v[v]].n[1], vertices[triangles[i].v[v]].n[2]);
            glVertex3f(vertices[triangles[i].v[v]].p[0], vertices[triangles[i].v[v]].p[1] , vertices[triangles[i].v[v]].p[2]);
        }

    }
    glEnd();
}


float det(const Vec3Df & v0, const Vec3Df & v1, const Vec3Df & v2)

{
	return Vec3Df::dotProduct(Vec3Df::crossProduct(v0,v1), v2);
}

bool testIntersect(const Vec3Df & src, const Vec3Df & dir,  
					  const Vec3Df & p0, const Vec3Df & p1, const Vec3Df & p2, 
					  float * beta, float * gamma, float * t)
{

	const float EPSILON=0.00001f;
	float A= det(p0-p1,p0-p2,dir);

	*beta= det(p0-src,p0-p2,dir)/A;

	if (*beta<-EPSILON)
		return false;
	else if (*beta<0)
		*beta=0;
	else if (*beta>1+EPSILON)
		return false;
	else if (*beta>1)
		*beta=1;

	*gamma= det(p0-p1, p0-src, dir)/A;

	if (*gamma<-EPSILON)
		return false;
	else if (*gamma<0)
		*gamma=0;
	else if (*gamma>1+EPSILON)
		return false;
	else if (*gamma>1)
		*gamma=1;


	if (*gamma+*beta>1+EPSILON)
		return false;
	else if (*gamma+*beta>1.0)
	{
		//hack something reasonable
		float temp(float(1.0)-*gamma-*beta);
		temp/=2.0;
		*beta+=temp;
		*gamma+=temp;
	}

	*t = det(p0-p1, p0-p2, p0-src)/A;
	
	if (*t<0)
		return false;
	
	return true;
}



int Mesh::getClosestVertexIndex(const Vec3Df & origin, const Vec3Df & direction)
{
	float minT=FLT_MAX;
	float minBeta=FLT_MAX;
	float minGamma=FLT_MAX;
	unsigned int minFh=0;

	float tT=FLT_MAX;
	float betaT=FLT_MAX;
	float gammaT=FLT_MAX;
	bool hit=false;

	for (unsigned int i=0; i<triangles.size();++i)
	{
		if (testIntersect(origin, direction, vertices[triangles[i].v[0]].p, vertices[triangles[i].v[1]].p, vertices[triangles[i].v[2]].p, &betaT, &gammaT, &tT))
		{
			hit=true;
			if (tT<minT)
			{
				minT=tT;
				minBeta=betaT;
				minGamma=gammaT;
				minFh=i;
			}
		}
	}

	float beta, gamma, t;
	int fh=minFh;
	
	beta=minBeta;
	gamma=minGamma;
	t=minT;
	fh=minFh;
	
	if (hit)
	{
		if (beta<0.5)
		{
			//p0 or p2
			if(gamma<0.5)
				return triangles[minFh].v[0];
			else
				return triangles[minFh].v[2];
		}
		else
		{
			if(gamma<0.5)
				return triangles[minFh].v[1];
			else
				return triangles[minFh].v[2];
		}
	}
	else
		return -1;
	
}	

void Mesh::draw(){
    glBegin(GL_TRIANGLES);

    for (int i=0;i<triangles.size();++i)
    {
        Vec3Df edge01 = vertices[triangles[i].v[1]].p -  vertices[triangles[i].v[0]].p;
        Vec3Df edge02 = vertices[triangles[i].v[2]].p -  vertices[triangles[i].v[0]].p;
        Vec3Df n = Vec3Df::crossProduct (edge01, edge02);
        n.normalize ();
        glNormal3f(n[0], n[1], n[2]);
        for(int v = 0; v < 3 ; v++){
            glVertex3f(vertices[triangles[i].v[v]].p[0], vertices[triangles[i].v[v]].p[1] , vertices[triangles[i].v[v]].p[2]);
        }

    }
    glEnd();
}


/************************************************************
 * Fonctions de chargement du maillage
 ************************************************************/
bool Mesh::loadMesh(const char * filename)
{

    std::vector<int> vhandles;

    const unsigned int LINE_LEN=256;
    char s[LINE_LEN];
    FILE * in;
#ifdef WIN32
    errno_t error=fopen_s(&in, filename,"r");
    if (error!=0)
#else
        in = fopen(filename,"r");
    if (!(in))
#endif
        return false;

    float x, y, z;

    while(in && !feof(in) && fgets(s, LINE_LEN, in))
    {
        // material file
        // vertex
        if (strncmp(s, "v ", 2) == 0)
        {
            if (sscanf(s, "v %f %f %f", &x, &y, &z))
                vertices.push_back(Vertex(Vec3Df(x,y,z)));
        }
        // face
        else if (strncmp(s, "f ", 2) == 0)
        {
            int component(0), nV(0);
            bool endOfVertex(false);
            char *p0, *p1(s+2); //place behind the "f "

            vhandles.clear();

            while (*p1 == ' ') ++p1; // skip white-spaces

            while (p1)
            {
                p0 = p1;

                // overwrite next separator

                // skip '/', '\n', ' ', '\0', '\r' <-- don't forget Windows
                while (*p1 != '/' && *p1 != '\r' && *p1 != '\n' &&
                       *p1 != ' ' && *p1 != '\0')
                    ++p1;

                // detect end of vertex
                if (*p1 != '/') endOfVertex = true;

                // replace separator by '\0'
                if (*p1 != '\0')
                {
                    *p1 = '\0';
                    p1++; // point to next token
                }

                // detect end of line and break
                if (*p1 == '\0' || *p1 == '\n')
                    p1 = 0;


                // read next vertex component
                if (*p0 != '\0')
                {
                    switch (component)
                    {
                    case 0: // vertex
                        vhandles.push_back(atoi(p0)-1);
                        break;

                    case 1: // texture coord
                        //assert(!vhandles.empty());
                        //assert((unsigned int)(atoi(p0)-1) < texcoords.size());
                        //_bi.set_texcoord(vhandles.back(), texcoords[atoi(p0)-1]);
                        break;

                    case 2: // normal
                        //assert(!vhandles.empty());
                        //assert((unsigned int)(atoi(p0)-1) < normals.size());
                        //_bi.set_normal(vhandles.back(), normals[atoi(p0)-1]);
                        break;
                    }
                }

                ++component;

                if (endOfVertex)
                {
                    component = 0;
                    nV++;
                    endOfVertex = false;
                }
            }


            if (vhandles.size()>3)
            {
                //model is not triangulated, so let us do this on the fly...
                //to have a more uniform mesh, we add randomization
                unsigned int k=(false)?(rand()%vhandles.size()):0;
                for (unsigned int i=0;i<vhandles.size()-2;++i)
                {
                    triangles.push_back(Triangle(vhandles[(k+0)%vhandles.size()],vhandles[(k+i+1)%vhandles.size()],vhandles[(k+i+2)%vhandles.size()]));
                }
            }
            else if (vhandles.size()==3)
            {
                triangles.push_back(Triangle(vhandles[0],vhandles[1],vhandles[2]));
            }
            else
            {
                printf("TriMesh::LOAD: Unexpected number of face vertices (<3). Ignoring face");
            }
        }
        memset(&s, 0, LINE_LEN);
    }
    fclose(in);

    centerAndScaleToUnit ();
    computeVertexNormals();
    return true;
}
