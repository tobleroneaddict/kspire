#pragma once
#include "../../globals.h"

#define vec2d linalg::vec<double,2>
#define vec3d linalg::vec<double,3>
#define vec4d linalg::vec<double,4>
#define matrix44d linalg::mat<double,4,4>

#ifndef M_PI
    #define M_PI 3.1415926
#endif
#define GLuint unsigned int

#define ushort unsigned short
#define uint unsigned int
#define uchar unsigned char
/*------------------------------------------------------*/

/*------------------------------------------------------*/
#define loop0i(end_l) for ( unsigned int i=0;i<end_l;++i )
#define loop0j(end_l) for ( unsigned int j=0;j<end_l;++j )
#define loopi(start_l,end_l) for ( unsigned int i=start_l;i<end_l;++i )
#define loopj(start_l,end_l) for ( unsigned int j=start_l;j<end_l;++j )
#define loopk(start_l,end_l) for ( unsigned int k=start_l;k<end_l;++k )
#define loopl(start_l,end_l) for ( unsigned int l=start_l;l<end_l;++l )
#define loopm(start_l,end_l) for ( unsigned int m=start_l;m<end_l;++m )
#define loopn(start_l,end_l) for ( unsigned int n=start_l;n<end_l;++n )
#define loop(a_l,start_l,end_l) for ( int a_l = start_l;a_l<end_l;++a_l )
/*------------------------------------------------------*/
#define loopij(_sti,_stj,_eni,_enj) loopi(_sti,_eni)loopj (_stj,_enj)
#define loopijk(_sti,_stj,_stk,_eni,_enj,_enk) loopi(_sti,_eni) loopj (_stj,_enj) loopk (_stk,_enk)
#define looplmn(_stl,_stm,_stn,_enl,_enm,_enn) loopl(_stl,_enl) loopm (_stm,_enm) loopn (_stn,_enn)
/*------------------------------------------------------*/
#define foreach(var, container) for( auto var = (container).begin(); var != (container).end(); ++var)
/*------------------------------------------------------*/
#define f_min min
#define f_max max
#define vswap(a,b) { auto c=a;a=b;b=c; }
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
#define clamp(a_,b_,c_) (min(max((a_),(b_)),(c_)))
#define frac(a) (a-floor(a))
#define dot3(a,b) (a.x*b.x+a.y*b.y+a.z*b.z)
#define cross3(a,b) vec3f( a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z , a.x * b.y - a.y * b.x )
#define min3f(a,b) vec3f(min(a.x,b.x),min(a.y,b.y),min(a.z,b.z))
#define max3f(a,b) vec3f(max(a.x,b.x),max(a.y,b.y),max(a.z,b.z))
#define lerp(a_,b_,t_) ( a_*(1-t_) + b_*t_ )
#define normalize3(a) (a*(1.0/sqrt(a.x*a.x+a.y*a.y+a.z*a.z)))
#define normalize4(a) (a*(1.0/sqrt(a.x*a.x+a.y*a.y+a.z*a.z+a.w*a.w)))
/*------------------------------------------------------*/
#define ifeq(sa,sb) if( 0==strncmp(sa,sb,min(strnlen(sa,1000),strnlen(sb,1000)) )) 
/*------------------------------------------------------*/
#define loopall(var, container) for( auto var = (container).begin(); var != (container).end(); ++var)
#define ifeq_lower(a,b) if( strnlen(a,1000)>0 && strnlen(b,1000)>0 && strcmp_lower(a,b)) 
#define ifexist( fname ) if( fexist(fname) )
#define ifnotexist( fname ) if( !fexist(fname) )
/*------------------------------------------------------*/
template< class T >
std::vector<T>& operator<<(std::vector<T>& v, const T& obj){ v.push_back(obj); return v; }
/*------------------------------------------------------*/
struct intfloat
{
	union {int i;float f;uint ui;};
};
/*------------------------------------------------------*/
class uchar4 
{ 
	public:

	uchar x,y,z,w;

	uchar4(){};
	uchar4(uint x,uint y,uint z,uint w)
	{
		this->x=x;
		this->y=y;
		this->z=z;
		this->w=w;
	}
	
	uint to_uint()
	{
		return x+(y<<8)+(z<<24);//*((uint*)this);
	};
	void from_uint(uint a)
	{
		x=a;y=a>>8;z=a>>24;//*((uint*)this)=a;
	};
};

		

class Terrain {

    matrix44d modelviewprojection;
    
    int tris_rendered;
    enum { patch_resolution = 16 };

    GLuint vbo, vboidx, indices ;


    void begin_patch();

    void draw_patch(
    vec3d p1, vec3d p2, vec3d p3,
    vec2d uv1, vec2d uv2, vec2d uv3,
    vec3d center,double size);

    void end_patch();

    void draw_triangle(
    vec3d p1, vec3d p2, vec3d p3,
    vec2d uv1, vec2d uv2, vec2d uv3,
    vec3d center);


    void draw_recursive(
    vec3d p1, vec3d p2, vec3d p3,
    vec2d uv1, vec2d uv2, vec2d uv3,
    vec3d center, double size = 1);

    void draw(vec3d center);

    bool patches = false;
};