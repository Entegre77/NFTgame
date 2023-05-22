#ifndef __BMF_H_
#define __BMF_H_

typedef short word;
typedef unsigned short int uword;
typedef int dword;
typedef float float32;



/* 
 * Untransformed with Normal Vertex 
 * Corresponds to OpenGL's GL_T2F_N3F_V3F format
*/

typedef struct {
	float32 u, v;
	float32 i, j, k;
	float32 x, y, z;
} BMF_UNVERTEX;

typedef struct {
	float32 x, y, z, w;
} T_VERTEX;

typedef struct {
	float32 i, j, k;
} T_NORMAL;

/* 
 * RGB Color 
 */

typedef struct {
	float32 red, green, blue, alpha;
} BMF_RGBCOLOR;



/*
 * Surface of three indexes into a vertex list; this is a triangle
 */

typedef struct {
	uword p0, p1, p2;
} BMF_SURFACE;



/*
 * The texture name length includes the terminating 0, so it is
 * always at least one. The texture name is a string terminated
 * with a 0. If it is empty (ie, no texture), then it is simply
 * a 0.
 *
 * The ambient, diffuse, and specular red, green, and blue
 * components are exactly that.
 *
 * The number of vertices is the size of the vertex list array.
 * The vertex list is an array of UNVERTEX, or an Untransformed with
 * Normal Vertex. 
 * The vertex list is local to this particular material.
 *
 */

typedef struct {
	uword texture_name_length;
	char *texture_name;
	GLuint texbind;
	BMF_RGBCOLOR ambient;
	BMF_RGBCOLOR diffuse;
	BMF_RGBCOLOR specular;
	uword number_of_vertices;
	BMF_UNVERTEX *vertexlist;
	short number_of_triangles;
	BMF_SURFACE *indexlist;
} SparseMesh;

typedef struct {
	uword texture_name_length;
	char *texture_name;
	GLuint texbind;
	BMF_RGBCOLOR ambient;
	BMF_RGBCOLOR diffuse;
	BMF_RGBCOLOR specular;
	uword number_of_vertices;
	BMF_UNVERTEX *vertexlist;
	uword number_of_strips;
	uword *length_of_strip;
	uword number_of_strip_indices;
	uword *stripindex;
} StripMesh;



/*
 * This is the actual BMF object. NumMaterials is the size of the
 * mesh array. The mesh array is either sparse, or strip. One will be
 * a null pointer and the other will be a real pointer to the data.
 * Thus, if BMFObject->strip == 0, then BMFObject->sparse is an array
 * of SparseMesh elements.
 */
typedef struct {
	uword NumMaterials;
	SparseMesh *sparse;
	StripMesh *strip;
	double radius;				/* openuniverse */
	int texsize;				/* openuniverse */
} BMFObject;

BMFObject *LoadBMF(char *, double);
void DrawBMFObject(BMFObject *);

#endif
