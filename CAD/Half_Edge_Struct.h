#include <stdlib.h>

struct Solid;
struct Face;
struct Loop;
struct HalfEdge;
struct Vertex;
struct Edge;

struct Solid
{
	Face* faces; // 第一个面
	Edge* edges;//
	Solid* next;
	Solid* pre;

	Solid() : faces(NULL), edges(NULL), next(NULL), pre(NULL) {}
};

struct Face
{
	Solid* solid; // 面对应的体
	Loop* outloop; //面对应的外环
	Face* next;
	Face* pre;

	Face() : outloop(NULL),  solid(NULL), next(NULL), pre(NULL) {}
};

struct Loop
{
	HalfEdge* halfedges; // 环对应的半边
	Face* face; // 环对应的面
	Loop* next;
	Loop* pre;

	Loop() : halfedges(NULL), face(NULL), next(NULL), pre(NULL) {}
};

struct HalfEdge
{
	Edge* edge; //半边对应的边
	Vertex* startv; //起点
	Vertex* endv; //终点
	Loop* loop; //半边对应的环
	HalfEdge* next;
	HalfEdge* pre;
	HalfEdge* neighbor;//对面的边

	HalfEdge() : edge(NULL), startv(NULL), endv(NULL), loop(NULL), next(NULL), pre(NULL), neighbor(NULL) {}
};

struct Edge
{
	HalfEdge* halfedge_l; 
	HalfEdge* halfedge_r; 
	Edge* next;
	Edge* pre;

	Edge() : halfedge_l(NULL), halfedge_r(NULL), next(NULL), pre(NULL) {}
};


struct Vertex
{
	double p[3];
	Vertex* next;
	Vertex* pre;


	Vertex(double x, double y, double z) : next(NULL), pre(NULL){
		p[0] = x;p[1] = y;p[2] = z;
	}
};
