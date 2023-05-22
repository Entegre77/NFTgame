/* 
 * Sample BMF file reader (from view3ds by David Farrell)
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef WIN32
#include <windows.h>
#endif
#include "GL/glut.h"
#include "bmf.h"
#include "ou.h"

GLubyte *read_JPEG_file(char *, int *, int *, int *);

char modelspath[100] = MODEL_DIR;

BMFObject *LoadBMF(char *filename, double radius)
{
	uword MeshType;
	SparseMesh *MaterialList;
	StripMesh *StripMaterialList;
	BMFObject *obj;
	FILE *in;
	int i, j;
	int width, height, components;
	GLubyte *image;
	char s[120];				/* openuniverse */
	double max = 0.0, d;		/* openuniverse */

	/* Create the new BMF object */
	if (!(obj = (BMFObject *) malloc(sizeof(BMFObject)))) {
		error("Can't allocate memory for BMFObject\n");
		shutdown(1);
	}

	sprintf(s, "%s/%s", modelspath, filename);

	if (!(in = fopen(s, "rb"))) {
		sprintf(s, "Can't open file %s", filename);
		error("Can't open file\n");
		shutdown(1);
	}
	obj->texsize = 0;

	/* Get number of materials */
	fread(&obj->NumMaterials, sizeof(uword), 1, in);

	/* Get mesh type */
	fread(&MeshType, sizeof(uword), 1, in);

	if (MeshType == 0) {
		/* Sparse Triangles */

		/* Allocate space for the materials */
		if (!
			(MaterialList =
			 (SparseMesh *) malloc(sizeof(SparseMesh) *
								   obj->NumMaterials))) {
			error("Can't allocate memory for materials\n");
			shutdown(1);
		}
		obj->sparse = MaterialList;
		obj->strip = 0;

		/* Read in each material */
		for (i = 0; i < obj->NumMaterials; i++) {
			/* Get the texture's name length */
			fread(&MaterialList[i].texture_name_length, sizeof(uword), 1,
				  in);

			if (MaterialList[i].texture_name_length == 0) {
				error("Invalid texture name length\n");
				shutdown(1);
			}

			/* Get the texture's name */
			MaterialList[i].texture_name =
				(char *) malloc(MaterialList[i].texture_name_length);
			fread(MaterialList[i].texture_name,
				  MaterialList[i].texture_name_length, 1, in);

			if (MaterialList[i].texture_name_length > 1) {

				for (j = 0; j < MaterialList[i].texture_name_length; j++)
					MaterialList[i].texture_name[j] =
						(char) tolower(MaterialList[i].texture_name[j]);

				image =
					read_JPEG_file(MaterialList[i].texture_name, &width,
								   &height, &components);

				if (image == NULL) {
					sprintf(s, "reading image %s",
							MaterialList[i].texture_name);
					error(s);
					shutdown(1);
				}
				image = texture_LOD(image, &width, &height, components);
				if (components == 1)
					j = 1;
				else
					j = color_depth / 8;
				obj->texsize += (width * height * j);

				glGenTextures(1, &MaterialList[i].texbind);
				glBindTexture(GL_TEXTURE_2D, MaterialList[i].texbind);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
								GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
								GL_LINEAR);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
						  GL_MODULATE);

				glTexImage2D(GL_TEXTURE_2D,
							 0,
							 components == 3 ? cd_rgb : cd_lum,
							 width, height,
							 0,
							 components == 3 ? GL_RGB : GL_LUMINANCE,
							 GL_UNSIGNED_BYTE, image);
				free(image);
			}


			/* Read in RGB colors */
			fread(&MaterialList[i].ambient, sizeof(BMF_RGBCOLOR), 1, in);

			fread(&MaterialList[i].diffuse, sizeof(BMF_RGBCOLOR), 1, in);

			fread(&MaterialList[i].specular, sizeof(BMF_RGBCOLOR), 1, in);

			/* Get the number of vertices */
			fread(&MaterialList[i].number_of_vertices, sizeof(uword), 1,
				  in);

			/* Allocate space for the vertex list */
			MaterialList[i].vertexlist =
				(BMF_UNVERTEX *) malloc(sizeof(BMF_UNVERTEX) *
										MaterialList
										[i].number_of_vertices);

			if (MaterialList[i].vertexlist == 0) {
				error("Error allocating memory for vertexlist\n");
				shutdown(1);
			}

			/* Read in the vertex list */
			fread(MaterialList[i].vertexlist,
				  sizeof(BMF_UNVERTEX),
				  MaterialList[i].number_of_vertices, in);

			for (j = 0; j < MaterialList[i].number_of_vertices; j++) {
				d = DISTANCE(MaterialList[i].vertexlist[j].x,
							 MaterialList[i].vertexlist[j].y,
							 MaterialList[i].vertexlist[j].z);
				if (d > max)
					max = d;
			}


			/* Read in the number of polygons */
			fread(&MaterialList[i].number_of_triangles, sizeof(uword), 1,
				  in);

			/* Allocate space for the index list */
			MaterialList[i].indexlist =
				(BMF_SURFACE *) malloc(sizeof(BMF_SURFACE) *
									   MaterialList
									   [i].number_of_triangles);

			/* Read in the triangle indices */
			fread(MaterialList[i].indexlist,
				  sizeof(BMF_SURFACE),
				  MaterialList[i].number_of_triangles, in);
		}

		/* Sorta vertex normalization */
		for (i = 0; i < obj->NumMaterials; i++) {
			for (j = 0; j < MaterialList[i].number_of_vertices; j++) {
				MaterialList[i].vertexlist[j].x /= max / radius;
				MaterialList[i].vertexlist[j].y /= max / radius;
				MaterialList[i].vertexlist[j].z /= max / radius;
			}
		}
	} else {
		/* Triangle Strips */

		/* Allocate space for the materials */
		if (!
			(StripMaterialList =
			 (StripMesh *) malloc(sizeof(StripMesh) *
								  obj->NumMaterials))) {
			error("Can't allocate memory for materials\n");
			shutdown(1);
		}
		obj->sparse = 0;
		obj->strip = StripMaterialList;

		/* Read in each material */
		for (i = 0; i < obj->NumMaterials; i++) {
			/* Get the texture's name length */
			fread(&StripMaterialList[i].texture_name_length,
				  sizeof(uword), 1, in);

			if (StripMaterialList[i].texture_name_length == 0) {
				error("Invalid texture name length\n");
				shutdown(1);
			}

			/* Get the texture's name */
			StripMaterialList[i].texture_name =
				(char *) malloc(StripMaterialList[i].texture_name_length);
			fread(StripMaterialList[i].texture_name,
				  StripMaterialList[i].texture_name_length, 1, in);

			if (StripMaterialList[i].texture_name_length > 1) {

				for (j = 0; j < StripMaterialList[i].texture_name_length;
					 j++)
					StripMaterialList[i].texture_name[j] = (char)
						tolower(StripMaterialList[i].texture_name[j]);

				image =
					read_JPEG_file(StripMaterialList[i].texture_name,
								   &width, &height, &components);
				if (image == NULL) {
					sprintf(s, "error reading image %s",
							StripMaterialList[i].texture_name);
					error(s);
					shutdown(1);
				}
				image = texture_LOD(image, &width, &height, components);
				if (components == 1)
					j = 1;
				else
					j = color_depth / 8;
				obj->texsize += (width * height * j);

				glGenTextures(1, &StripMaterialList[i].texbind);
				glBindTexture(GL_TEXTURE_2D, StripMaterialList[i].texbind);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
								GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
								GL_LINEAR);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
						  GL_MODULATE);
				glTexImage2D(GL_TEXTURE_2D, 0,
							 components == 3 ? cd_rgb : cd_lum,
							 width, height, 0,
							 components == 3 ? GL_RGB : GL_LUMINANCE,
							 GL_UNSIGNED_BYTE, image);
				free(image);
			}

			/* Read in RGB colors */
			fread(&StripMaterialList[i].ambient, sizeof(BMF_RGBCOLOR), 1,
				  in);

			fread(&StripMaterialList[i].diffuse, sizeof(BMF_RGBCOLOR), 1,
				  in);

			fread(&StripMaterialList[i].specular,
				  sizeof(BMF_RGBCOLOR), 1, in);

			/* Get the number of vertices */
			fread(&StripMaterialList[i].number_of_vertices,
				  sizeof(uword), 1, in);

			/* Allocate space for the vertex list */
			StripMaterialList[i].vertexlist =
				(BMF_UNVERTEX *) malloc(sizeof(BMF_UNVERTEX) *
										StripMaterialList
										[i].number_of_vertices);

			if (StripMaterialList[i].vertexlist == 0) {
				error("Error allocating memory for vertexlist\n");
				shutdown(1);
			}

			/* Read in the vertex list */
			fread(StripMaterialList[i].vertexlist,
				  sizeof(BMF_UNVERTEX),
				  StripMaterialList[i].number_of_vertices, in);

			for (j = 0; j < StripMaterialList[i].number_of_vertices; j++) {
				d = DISTANCE(StripMaterialList[i].vertexlist[j].x,
							 StripMaterialList[i].vertexlist[j].y,
							 StripMaterialList[i].vertexlist[j].z);
				if (d > max)
					max = d;
			}


			/* Read in the number of strips */
			fread(&StripMaterialList[i].number_of_strips,
				  sizeof(uword), 1, in);

			/* Allocate space for the strip length list */
			StripMaterialList[i].length_of_strip =
				(uword *) malloc(sizeof(uword) *
								 StripMaterialList[i].number_of_strips);

			/* Read in the triangle strip lengths */
			fread(StripMaterialList[i].length_of_strip,
				  sizeof(uword), StripMaterialList[i].number_of_strips,
				  in);

			/* Read in the number of strip points */
			fread(&StripMaterialList[i].number_of_strip_indices,
				  sizeof(uword), 1, in);

			/* Allocate space for the strip length list */
			StripMaterialList[i].stripindex =
				(uword *) malloc(sizeof(uword) *
								 StripMaterialList
								 [i].number_of_strip_indices);

			/* Read in the triangle strip lengths */
			fread(StripMaterialList[i].stripindex,
				  sizeof(uword),
				  StripMaterialList[i].number_of_strip_indices, in);
		}

		/* Sorta vertex normalization */
		for (i = 0; i < obj->NumMaterials; i++) {
			for (j = 0; j < StripMaterialList[i].number_of_vertices; j++) {
				StripMaterialList[i].vertexlist[j].x /= max / radius;
				StripMaterialList[i].vertexlist[j].y /= max / radius;
				StripMaterialList[i].vertexlist[j].z /= max / radius;
			}
		}
	}

	fclose(in);
	obj->radius = radius;
	return obj;
}
