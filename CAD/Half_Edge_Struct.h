#include <stdlib.h>

struct Solid;
struct Face;
struct Loop;
struct HalfEdge;
struct Vertex;
struct Edge;

struct Solid
{
	Face* faces; // ��һ����
	Edge* edges;//
	Solid* next;
	Solid* pre;

	Solid() : faces(NULL), edges(NULL), next(NULL), pre(NULL) {}
};

struct Face
{
	Solid* solid; // ���Ӧ����
	Loop* outloop; //���Ӧ���⻷
	Face* next;
	Face* pre;

	Face() : outloop(NULL),  solid(NULL), next(NULL), pre(NULL) {}
};

struct Loop
{
	HalfEdge* halfedges; // ����Ӧ�İ��
	Face* face; // ����Ӧ����
	Loop* next;
	Loop* pre;

	Loop() : halfedges(NULL), face(NULL), next(NULL), pre(NULL) {}
};

struct HalfEdge
{
	Edge* edge; //��߶�Ӧ�ı�
	Vertex* startv; //���
	Vertex* endv; //�յ�
	Loop* loop; //��߶�Ӧ�Ļ�
	HalfEdge* next;
	HalfEdge* pre;
	HalfEdge* neighbor;//����ı�

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
