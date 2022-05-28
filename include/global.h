#pragma once
#include <raylib.h>
#include <rlgl.h>
#include <math.h>
#include <map>
#include <functional>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <list>

typedef unsigned long       DWORD;
typedef unsigned short      WORD;
typedef unsigned char       BYTE;

#define ARGB(a,r,g,b)	((DWORD(a)<<24) + (DWORD(r)<<16) + (DWORD(g)<<8) + DWORD(b))
#define GETA(col)		((col)>>24)
#define GETR(col)		(((col)>>16) & 0xFF)
#define GETG(col)		(((col)>>8) & 0xFF)
#define GETB(col)		((col) & 0xFF)
#define SETA(col,a)		(((col) & 0x00FFFFFF) + (DWORD(a)<<24))
#define SETR(col,r)		(((col) & 0xFF00FFFF) + (DWORD(r)<<16))
#define SETG(col,g)		(((col) & 0xFFFF00FF) + (DWORD(g)<<8))
#define SETB(col,b)		(((col) & 0xFFFFFF00) + DWORD(b))

#define PIOVER180	0.017453292519943295769236907684886
#define M_PI 3.14159265358979323846
#define DEG  -180 / M_PI;
#define RAD  M_PI / -180;
#define RMAX(a, b) (a) < (b) ? (b) : (a)
#define RMIN(a, b) (a) > (b) ? (b) : (a)
#define FIX_ARTIFACTS_BY_STRECHING_TEXEL true

#define	BLEND_COLORADD		1
#define	BLEND_COLORMUL		0

#define	BLEND_ALPHABLEND	2
#define	BLEND_ALPHAADD		0

#define	BLEND_ZWRITE		4
#define	BLEND_NOZWRITE		0

#define BLEND_DEFAULT		(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE)
#define BLEND_DEFAULT_Z		(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_ZWRITE)

#define  ResizerPointSize  8
#define  RotatePointSize   40

typedef enum {
	MODE_NORMAL = 0,
	MODE_RESIZE_WIDGET,
	MODE_MOVE_WIDGET,
    MODE_PLACE_WIDGET,
	MODE_ROTATE_WIDGET
} EditorMode;

typedef enum {
	RESIZER_POINT_N = 0,
	RESIZER_POINT_NE,
	RESIZER_POINT_E,
	RESIZER_POINT_SE,
	RESIZER_POINT_S,
	RESIZER_POINT_SW,
	RESIZER_POINT_W,
	RESIZER_POINT_NW,
	RESIZER_POINT_COUNT
}ResizerPoint;


struct rVertex
{
	float			x, y,z;		// screen position
	DWORD			col;		// color
	float			tx, ty;		// texture coordinates

};



struct rQuad
{
	rVertex		v[4];
	Texture2D		tex;
	int				blend;
};





inline Rectangle nrect(float x, float y, float w, float h)
{
 Rectangle r;
 r.x=x;
 r.y=y;
 r.width=w;
 r.height=h;
 return r;

}


template <typename T, typename Key>
inline bool key_exists(const T& container, const Key& key)
{
    return (container.find(key) != std::end(container));
}
template<typename T>
inline void vector_remove(std::vector<T> & v, const T & item)
{
    v.erase(std::remove(v.begin(), v.end(), item), v.end());
}




inline  void RenderQuad(const rQuad *quad)
{


    if (quad->tex.id > 0)
    {


        rlEnableTexture(quad->tex.id);



            rlBegin(RL_QUADS);


                Color a = GetColor(quad->v[1].col);
                Color b = GetColor(quad->v[0].col);
                Color c = GetColor(quad->v[3].col);
                Color d = GetColor(quad->v[2].col);

                rlNormal3f(0.0f, 0.0f, 1.0f);                          // Normal vector pointing towards viewer


                rlColor4ub(a.r,a.g,a.b,a.a);
                rlTexCoord2f(quad->v[1].tx,quad->v[1].ty);
                rlVertex3f(quad->v[1].x,quad->v[1].y,quad->v[1].z);

                rlColor4ub(b.r,b.g,b.b,b.a);
                rlTexCoord2f(quad->v[0].tx,quad->v[0].ty);
                rlVertex3f(quad->v[0].x,quad->v[0].y,quad->v[0].z);

                 rlColor4ub(c.r,c.g,c.b,c.a);
                rlTexCoord2f(quad->v[3].tx,quad->v[3].ty);
                rlVertex3f(quad->v[3].x,quad->v[3].y,quad->v[3].z);

                rlColor4ub(d.r,d.g,d.b,d.a);
                rlTexCoord2f(quad->v[2].tx,quad->v[2].ty);
                rlVertex3f(quad->v[2].x,quad->v[2].y,quad->v[2].z);

            rlEnd();


        rlDisableTexture();
    }

}



inline  void  RenderNormal(Texture2D texture, float x, float y,int blend)
{

rQuad quad;
quad.tex=texture;
quad.blend=blend;


float u=0.0f;
float v=0.0f;
float u2=1.0f;
float v2=1.0f;

float fx2=x+ texture.width;
float fy2=y+texture.height;


    quad.v[1].x = x;    quad.v[1].y = y;
    quad.v[1].tx = u;    quad.v[1].ty = v;

    quad.v[0].x = x;    quad.v[0].y = fy2;
    quad.v[0].tx = u;    quad.v[0].ty = v2;

    quad.v[3].x = fx2;    quad.v[3].y = fy2;
    quad.v[3].tx = u2;    quad.v[3].ty = v2;

    quad.v[2].x = fx2;    quad.v[2].y = y;
    quad.v[2].tx = u2;    quad.v[2].ty = v;




    quad.v[0].z =            quad.v[1].z =            quad.v[2].z =            quad.v[3].z = 0.0f;
    quad.v[0].col =            quad.v[1].col =            quad.v[2].col =            quad.v[3].col = 0xffffffff;


   RenderQuad(&quad);





}
inline void  RenderTile(Texture2D texture, float x, float y, float width, float height, Rectangle clip,bool flipx, bool flipy,int blend)
{

float fx2 = x+width;
float fy2 = y+height;
rQuad quad;
quad.tex=texture;
quad.blend=blend;


int widthTex  = texture.width;
int heightTex = texture.height;

float left,right,top,bottom;


if (FIX_ARTIFACTS_BY_STRECHING_TEXEL)
{

   left = (2*clip.x+1) / (2*widthTex);
   right =  left +(clip.width*2-2) / (2*widthTex);
   top = (2*clip.y+1) / (2*heightTex);
   bottom = top +(clip.height * 2 - 2) / (2 * heightTex);



}else
  {
   left = clip.x / widthTex;
   right =  (clip.x + clip.width) / widthTex;
   top = clip.y / heightTex;
   bottom = (clip.y + clip.height) / heightTex;
  }


if (flipx)
{
    float tmp = left;
    left = right;
    right = tmp;
}

if (flipy)
{
    float tmp = top;
    top = bottom;
    bottom = tmp;
}


    quad.v[1].tx = left;    quad.v[1].ty = top;
    quad.v[1].x = x;        quad.v[1].y = y;

    quad.v[0].x = x;    quad.v[0].y = fy2;
    quad.v[0].tx = left;    quad.v[0].ty = bottom;

    quad.v[3].x = fx2;    quad.v[3].y = fy2;
    quad.v[3].tx = right;    quad.v[3].ty = bottom;


    quad.v[2].x = fx2;    quad.v[2].y = y;
    quad.v[2].tx = right;    quad.v[2].ty = top;



    quad.v[0].z =            quad.v[1].z =            quad.v[2].z =            quad.v[3].z = 0.0f;
    quad.v[0].col =            quad.v[1].col =            quad.v[2].col =            quad.v[3].col = 0xffffffff;


   RenderQuad(&quad);




}
