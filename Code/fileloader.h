#include <vector>
#include <stdio.h>
#include <string.h>

struct Point3f
{

	Point3f(float xI, float yI, float zI)
		:x(xI)
		,y(yI)
		,z(zI)
	{

	}

	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		float pos[3];
	};	
};

struct Triplet3i
{
	Triplet3i(int xI, int yI, int zI)
		:i1(xI)
		,i2(yI)
		,i3(zI)
	{

	}

	union
	{
		struct{
			int i1;
			int i2;
			int i3;
		};
		int index[3];
	};
};

static bool loadMesh(const char * filename, std::vector<Point3f> * verticesP, std::vector<Triplet3i> * facesP)
{
	if ((facesP==0)||(verticesP==0))
		return false;

	std::vector<Triplet3i> & faces=*facesP;
	std::vector<Point3f> & vertices=*verticesP;


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
				vertices.push_back(Point3f(x,y,z));
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
					faces.push_back(Triplet3i(vhandles[(k+0)%vhandles.size()],vhandles[(k+i+1)%vhandles.size()],vhandles[(k+i+2)%vhandles.size()]));
				}
			}
			else if (vhandles.size()==3)
			{
				faces.push_back(Triplet3i(vhandles[0],vhandles[1],vhandles[2]));
			}
			else
			{
				printf("TriMesh::LOAD: Unexpected number of face vertices (<3). Ignoring face");
			}
		}
		memset(&s, 0, LINE_LEN);
	}
	fclose(in);
	return true;
}
