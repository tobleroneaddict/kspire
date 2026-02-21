////////////////////////////////////////////////////////////////////////////////
//
// Planet-LOD - A simple implementation for rendering planets with LOD
//
// (C) by Sven Forstmann in 2016
//
// Please credit this code if you use it anywhere
//
// License : MIT
// http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
// Mathlib included from 
// http://sourceforge.net/projects/nebuladevice/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
#include "planet-lod.h"

////////////////////////////////////////////////////////////////////////////////

 void Terrain::begin_patch()
{
	if (vbo == 0)
	{
		unsigned int n = patch_resolution;
		std::vector<float> v;
		std::vector<ushort> idx;
		loopi(0, n + 1)loopj(0, i + 1)
		{
			v << float(double(1.0 - double(i) / double(n)));
			v << float(double(double(j) / double(n)));
			v << 0.0f;

			if (i > 0 && j > 0)
			{
				idx << (ushort)(v.size() / 3 - 1);
				idx << (ushort)(v.size() / 3 - 2);
				idx << (ushort)(v.size() / 3 - 2 - i);

				if (j < i)
				{
					idx << (ushort)(v.size() / 3 - 2 - i);
					idx << (ushort)(v.size() / 3 - 1 - i);
					idx << (ushort)(v.size() / 3 - 1);
				}
			}
		}
		indices = idx.size();// / 3;
		// glGenBuffers(1, &vbo); ogl_check_error();
		// glBindBuffer(GL_ARRAY_BUFFER, vbo); ogl_check_error();
		// glBufferData(GL_ARRAY_BUFFER, 12 * v.size(), &v[0], GL_STATIC_DRAW); ogl_check_error();
		// glGenBuffers(1, &vboidx); ogl_check_error();
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboidx); ogl_check_error();
		// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short)*idx.size(), &idx[0], GL_STATIC_DRAW); ogl_check_error();
		// glBindBuffer(GL_ARRAY_BUFFER, 0);
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//shader = Shader("../shader/Patch");
	}

	//Define this somewhere in your header file
	#define BUFFER_OFFSET(i) ((void*)(i))

	// glBindBuffer(GL_ARRAY_BUFFER, vbo); ogl_check_error();
	// glEnableClientState(GL_VERTEX_ARRAY); ogl_check_error();
	// glVertexPointer(3, GL_FLOAT, 12, BUFFER_OFFSET(0)); ogl_check_error();//The starting point of the VBO, for the vertices
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboidx); ogl_check_error();
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);

}

 void Terrain::draw_patch(
	vec3d p1, vec3d p2, vec3d p3,
	vec2d uv1, vec2d uv2, vec2d uv3,
	vec3d center,double size)
{
	vec3d d2 = p2 - p1, d3 = p3 - p1;	


	bool normalize_enabled = size > 0.0001 ? 1 : 0;

	if (!normalize_enabled)
	{
		vec4d p1x = linalg::mul(modelviewprojection, vec4d(p1.x, p1.y, p1.z, 1.0));
		vec4d p2x = linalg::mul(modelviewprojection, vec4d(p2.x, p2.y, p2.z, 1.0));// -p1x;
		vec4d p3x = linalg::mul(modelviewprojection, vec4d(p3.x, p3.y, p3.z, 1.0));// -p1x,linalg::mul
		vec4d n1x = linalg::mul(modelviewprojection, vec4d(p1.x*0.994, p1.y*0.994, p1.z*0.994, 1.0));
		vec4d n2x = linalg::mul(modelviewprojection, vec4d(p2.x*0.994, p2.y*0.994, p2.z*0.994, 1.0));
		vec4d n3x = linalg::mul(modelviewprojection, vec4d(p3.x*0.994, p3.y*0.994, p3.z*0.994, 1.0));

		n1x = n1x - p1x;
		n2x = n2x - p2x;
		n3x = n3x - p3x;

		vec4d d2x = p2x - p1x;
		vec4d d3x = p3x - p1x;

		vec4d nd2x = n2x - n1x;
		vec4d nd3x = n3x - n1x;


	}


	//glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0)); ogl_check_error();//The starting point of the IBO
	//0 and 3 are the first and last vertices
	//glDrawRangeElements(GL_TRIANGLES, 0, 3, 3, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));   //The starting point of the IBO
	//glDrawRangeElements may or may not give a performance advantage over glDrawElements	
	tris_rendered += patch_resolution*patch_resolution;
}
 void Terrain::end_patch()
{
	
	// glBindBuffer(GL_ARRAY_BUFFER, 0);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// glDisableClientState(GL_VERTEX_ARRAY);
	// glDisableClientState(GL_COLOR_ARRAY);
	// glDisable(GL_CULL_FACE);
	// ogl_check_error();
}

 void Terrain::draw_triangle(
	vec3d p1, vec3d p2, vec3d p3,
	vec2d uv1, vec2d uv2, vec2d uv3,
	vec3d center)
{
	// glBegin(GL_TRIANGLES);
	// glVertex3f(p1.x, p1.y, p1.z);
	// glVertex3f(p2.x, p2.y, p2.z);
	// glVertex3f(p3.x, p3.y, p3.z);
	// glEnd();
	tris_rendered++;
}

 void Terrain::draw_recursive(
	vec3d p1, vec3d p2, vec3d p3,
	vec2d uv1, vec2d uv2, vec2d uv3,
	vec3d center, double size)
{
	//double ratio_size = size * gui.screen[0].slider["lod.ratio"].val; // default : 1
	double ratio_size = 1.0f;
	double minsize = 0.1f;    // default : 0.01
	//double minsize = gui.screen[0].slider["detail"].val;    // default : 0.01
	vec3d edge_center[3] = { (p1 + p2) / 2, (p2 + p3) / 2, (p3 + p1) / 2 };
	bool edge_test[3]; double angle[3];

	loopi(0, 3)
	{ 
		vec3d d = center + edge_center[i];
		edge_test[i] = linalg::length(d) > ratio_size; 

		
		double dot = linalg::dot(edge_center[i],linalg::normalize(d));
		angle[i] = acos(clamp(dot, -1, 1));
	}

	// culling
	{
		// angle based
		if (max(angle[0], max(angle[1], angle[2])) < M_PI / 2 - 0.2) return;//culling

		// frustum based
		if (size<0.25)//if (0)
		{
			vec3d p[3] = { p1, p2, p3 };
			int xp = 0, xm = 0, yp = 0, ym = 0, zp = 0;
			loopi(0, 6)
			{
				vec3d v = linalg::normalize(p[i % 3]); 
				if (i >= 3) v = v*0.994;
				vec4d cs = linalg::mul(modelviewprojection, vec4d(v.x, v.y, v.z, 1)); // clipspace
				if (cs.z< cs.w) zp++;
				if (cs.x<-cs.w) xm++;	
				if (cs.x>cs.w) xp++;
				if (cs.y<-cs.w) ym++;	
				if (cs.y>cs.w) yp++;
			}
			if (zp==0 || xm == 6 || xp == 6 || ym == 6 || yp == 6)return; // skip if invisible
		}
	}

	if ((edge_test[0] && edge_test[1] && edge_test[2]) || size < minsize)
	{ 
		if (patches)
			draw_patch(p1, p2, p3,uv1,uv2,uv3,center,size); 
		else
			draw_triangle(p1, p2, p3, uv1, uv2, uv3, center);
		return; 
	}
	// Recurse
	vec3d p[6] = { p1, p2, p3, edge_center[0], edge_center[1], edge_center[2] };
	vec2d uv[6] = { uv1, uv2, uv3, (uv1 + uv2) *0.5, (uv2 + uv3) *0.5, (uv3 + uv1) *0.5 };
	int idx[12] = { 0, 3, 5,    5, 3, 4,    3, 1, 4,    5, 4, 2 };
	bool valid[4] = { 1, 1, 1, 1 };

	if (edge_test[0]){ p[3] = p1; uv[3] = uv1; valid[0] = 0; } // skip triangle 0 ?
	if (edge_test[1]){ p[4] = p2; uv[4] = uv2; valid[2] = 0; } // skip triangle 2 ?
	if (edge_test[2]){ p[5] = p3; uv[5] = uv3; valid[3] = 0; } // skip triangle 3 ?

	loopi(0, 4) if (valid[i])
	{
		int i1 = idx[3 * i + 0], i2 = idx[3 * i + 1], i3 = idx[3 * i + 2];
		draw_recursive(
			linalg::normalize(p[i1])
	      , linalg::normalize(p[i2])
		  ,	linalg::normalize(p[i3]),
			uv[i1], uv[i2], uv[i3],
			center,size/2 );
	}		
}
 void Terrain::draw(vec3d center)
{
	// create icosahedron
	double t = (1.0 + sqrt(5.0)) / 2.0;

	std::vector<vec3d> p({ 
		{ -1, t, 0 }, { 1, t, 0 }, { -1, -t, 0 }, { 1, -t, 0 },
		{ 0, -1, t }, { 0, 1, t }, { 0, -1, -t }, { 0, 1, -t },
		{ t, 0, -1 }, { t, 0, 1 }, { -t, 0, -1 }, { -t, 0, 1 },
	});
	std::vector<int> idx({ 
		0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11,
		1, 5, 9, 5, 11, 4, 11, 10, 2, 10, 7, 6, 10, 7, 6, 7, 1, 8,
		3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9,
		4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7, 9, 8, 1
	});

	tris_rendered = 0;

//	bool patches = gui.screen[0].checkbox["patches"].checked;
	


	if (patches) begin_patch();

	loopi(0, idx.size() / 3)
	{
		//std::cout << i << std::endl;
		vec3d p3[3] = { linalg::normalize(p[idx[i * 3 + 0]]) ,	// triangle point 1
						linalg::normalize(p[idx[i * 3 + 1]]) ,	// triangle point 2
						linalg::normalize(p[idx[i * 3 + 2]]) };	// triangle point 3
		vec2d uv[3] = { vec2d(0, 0), vec2d(0.5, sqrt(3) / 2), vec2d(1, 0) };
		draw_recursive(p3[0], p3[1], p3[2],	uv[0],uv[1],uv[2],center);
	}
	if (patches) end_patch();

//	gui.screen[0].label["stats"].text = Gui::String("Triangles: ") + tris_rendered;
}





