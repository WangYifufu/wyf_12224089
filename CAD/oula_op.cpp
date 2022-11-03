#include "oula_op.h"
#include <cstdio>
#include <iostream>
using namespace std;

Solid* mvfs(double x, double y, double z, Vertex*& vertex) {
	//����Ԫ��
	Solid* solid = new Solid();
	Face* face = new Face();
	Loop* lp = new Loop();
	vertex  = new Vertex(x,y,z);
	//��������
	solid->faces = face;
	face->solid = solid;
	face->outloop = lp;
	lp->face = face;

	return solid;
}

HalfEdge* mev( Vertex* st,double x, double y, double z, Loop* lp) {
	//����Ԫ��
	Vertex *ed = new Vertex(x, y, z);
	Solid* solid = lp->face->solid;
	Edge* edge = new Edge();//create a new edge
	HalfEdge* half_l = new HalfEdge();
	HalfEdge* half_r = new HalfEdge();
	//��������
	half_l->edge = half_r->edge = edge;//��ߺͱ�
	edge->halfedge_l = half_l;
	edge->halfedge_r = half_r;

	half_l->loop = half_r->loop = lp;//��ߺͻ�

	half_l -> startv = st;//��ߺ͵�
	half_l -> endv   = ed;
	half_r -> startv = ed;
	half_r -> endv   = st;

	half_l->neighbor = half_r;//��Եİ��
	half_r->neighbor = half_l;

	if (lp->halfedges == NULL)//��ߺͰ��
	{
		half_l->next = half_r;
		half_r->next = half_l;

		half_l->pre = half_r;
		half_r->pre = half_l;
		lp->halfedges = half_l;
	}
	else
	{
		HalfEdge* he = new HalfEdge;
		for (he = lp->halfedges; he->next->startv != st; he = he->next) {
			//cout << he->startv->p[0] << ' ' << he->startv->p[1] << ' ' << he->startv->p[2] << endl;
		};
		half_r->next = he->next;
		he->next->pre = half_r;
		he->next = half_l;
		half_l->pre = he;
		half_l->next = half_r;
		half_r->pre = half_l;
	}
	//���ߴ洢
	Edge* cedge = solid->edges;
	if (cedge == NULL) {//�廹û�ߣ��ѵ�һ������ӽ�ȥ
		solid->edges = edge;
	}
	else {//�ҵ�������һ���ߣ��������߼ӽ�ȥ
		for (cedge; cedge->next != NULL; cedge = cedge->next);
		cedge->next = edge;
		edge->pre = cedge;
	}
	
	return half_l;
}

Loop* mef(Vertex* st, Vertex* ed, Loop* lp) {
	Solid* solid = lp->face->solid;
	Edge* edge = new Edge();
	HalfEdge* half_l = new HalfEdge();
	HalfEdge* half_r = new HalfEdge();
	Loop* newloop = new Loop();//�����߻Ὣԭ���Ļ��ָ����һ���µĻ�
	Face* newface = new Face();//���ɵ��µ���

	half_l->edge = half_r->edge = edge;//��ߺͱ�
	edge->halfedge_l = half_l;
	edge->halfedge_r = half_r;

	half_l->startv = st;//��ߺ͵�
	half_l->endv = ed;
	half_r->startv = ed;
	half_r->endv = st;

	half_l->neighbor = half_r;//��Եİ��
	half_r->neighbor = half_l;
	
	//��ߺͻ����Ƚϸ��ӣ�������������
	HalfEdge * he1, * he2, * he= lp->halfedges;
	for (he = lp->halfedges; he->endv != st; he = he->next);
	he1 = he;//�ҵ������Ϊ������İ��
	for (; he->endv != ed; he = he->next);
	he2 = he;//�ҵ����յ�Ϊ������İ��
	he = he->next;
	for (; he->endv != ed; he = he->next);//�����ߣ������յ�Ϊ������İ��
	bool inner_roll = false;
	if (he != he2){//���ȣ������ڻ�
		inner_roll = true;
		he2 = he;
	}

	//�ҵ������յ�ǰ��İ�ߣ�������������
	half_l->pre = he1;
	half_l->next = he2->next;
	half_r->pre = he2;
	half_r->next = he1->next;
	he1->next->pre = half_r;
	he1->next = half_l;
	he2->next->pre = half_l;
	he2->next = half_r;

	newface->outloop = newloop;
	newloop->face = newface;
	newface->solid = solid;
	lp->halfedges = half_r;
	newloop->halfedges = half_l;

	HalfEdge* the = half_l->next;//��half_R�ϵİ�߶���newloop��Ӧ
	for (; the != half_l; the = the->next) {
		the->loop = newloop;
	}


	//���µ���ŵ����У����������Ϣ
	Face* f = solid->faces;
	for (f; f->next != NULL; f = f->next);
	f->next = newface;
	newface->pre = f;

	//���ߴ洢
	Edge* cedge = solid->edges;
	if (cedge == NULL) {//�廹û�ߣ��ѵ�һ������ӽ�ȥ
		solid->edges = edge;
	}
	else {//�ҵ�������һ���ߣ��������߼ӽ�ȥ
		for (cedge; cedge->next != NULL; cedge = cedge->next);
		cedge->next = edge;
		edge->pre = cedge;
	}
	//tlp = newloop;
	//for (; tlp != NULL; tlp = tlp->next) {
	//	cout << '1' << endl;
	//}

	return newloop;//�µĻ�����ߵĻ�
}

Loop* kemr(Vertex* st, Vertex* ed, Loop* lp) {//�������ߵĿ��ϣ���Ȼ���ķ�����
	Face* face = lp->face;
	Solid* solid = face->solid;
	Loop* loop = new Loop();

	HalfEdge* he, * he1, * he2;
	for (he = lp->halfedges; he->startv != st || he->endv != ed; he = he->next) {};
	he1 = he;//he1�׵�β
	for (; he->startv != ed || he->endv != st; he = he->next);
	he2 = he;//he2β����

	he2->pre->next = he1->next;
	he1->next->pre = he2->pre;
	he1->pre->next = he2->next;
	he2->next->pre = he1->pre;
	
	//���»�
	lp->halfedges = he2->next;//�����⻷
	loop->halfedges = he2->pre;//�ڻ��԰��
	he2->pre->loop = loop;
	HalfEdge* the = he2->pre->next;//��half_R�ϵİ�߶���newloop��Ӧ
	for (; the != he2->pre; the = the->next) {
		the->loop = loop;
	}
	for (the=he2->next->next; the != he2->next; the = the->next) {
		the->loop = lp;
	}
	loop->face = face;//�ڶ���

	//��Ի�
	if (face->outloop == NULL) {
		face->outloop = loop;
	}
	else {
		Loop* endlp;
		for (endlp = face->outloop; endlp->next != NULL; endlp = endlp->next) { 
			//cout << '1' << endl;
		}
		endlp->next = loop;
		loop->pre = endlp;
		loop->next = NULL;
	}

	//he1,he2��Ӧ�ı�Ҫɾ��
	Edge* edge = solid->edges, * hedge = he->edge;
	while (edge != hedge){
		edge = edge->next;
	}
	if (edge->next == NULL){
		edge->pre->next = NULL;
	}else if (edge->pre == NULL){
		solid->edges = edge->next;
		edge->next->pre = NULL;
	}
	else if (edge->next == NULL && edge->pre == NULL) {
		solid->edges = NULL;
	}else{
		edge->pre->next = edge->next;
		edge->next->pre = edge->pre;
	}

	//�ٽ���ɾ��
	delete edge,he1,he2;

	return loop;//���ڻ�
}

void kfmrh(Loop* outlp, Loop* lp) {
	Face* F = outlp->face;
	Face* f = lp->face;

	if (F->outloop == NULL) {
		F->outloop = lp;
	}
	else {
		Loop* endlp = F->outloop;
		for (endlp ; endlp->next != NULL; endlp = endlp->next);
		endlp->next = lp;
		lp->pre = endlp;
	}

	lp->face = F;

	Solid* solid = F->solid;
	Face* face = solid->faces;
	if (face == f)
	{
		solid->faces = face->next;
	}
	else
	{
		Face* tf = face;
		while (face != f && face != NULL)
		{
			tf = face;
			face = face->next;
		}
		tf->next = face->next;
	}

	delete f;
}

void sweep(Face* face, double x, double y, double z, double d){
	Solid* solid = face->solid;
	Loop* loop;
	HalfEdge *he, *uphe, *first_he;
	Vertex *firstv, *up_v, *pre_up_v, *nextv;
	loop = face->outloop;

	for (loop = face->outloop; loop != NULL ; loop = loop->next){

		//cout << endl << endl;
		//HalfEdge* hhe = loop->halfedges;
		//cout << hhe->startv->p[0] << ' ' << hhe->startv->p[1] << ' ' << hhe->startv->p[2] << endl;
		//for (hhe = hhe->next; hhe != loop->halfedges; hhe = hhe->next) {
		//	cout << hhe->startv->p[0] << ' ' << hhe->startv->p[1] << ' ' << hhe->startv->p[2] << endl;
		//}; //For_Test
		Loop* tlp = loop;
		he = loop->halfedges;
		firstv = he->startv;
		double  x1 = firstv->p[0] + d * x, 
			y1 = firstv->p[1] + d * y, 
			z1 = firstv->p[2] + d * z;

		//cout << x1 << ' ' << y1 << ' ' << z1 << endl;

		first_he = mev(firstv, x1, y1, z1, tlp);
		pre_up_v = first_he->endv;
		he = he->next;
		nextv = he->startv;
		while (nextv != firstv){
			x1 = nextv->p[0] + d * x,
			y1 = nextv->p[1] + d * y,
			z1 = nextv->p[2] + d * z;

			//cout << x1 << ' ' << y1 << ' ' << z1 << endl;
			uphe = mev(nextv, x1, y1, z1, tlp);
			up_v = uphe->endv;
			mef(up_v, pre_up_v ,tlp);

			//cout << endl << endl;
			//HalfEdge* hhe = tlp->halfedges;
			//cout << hhe->startv->p[0] << ' ' << hhe->startv->p[1] << ' ' << hhe->startv->p[2] << endl;
			//for (hhe = hhe->next; hhe != tlp->halfedges; hhe = hhe->next) {
			//	cout << hhe->startv->p[0] << ' ' << hhe->startv->p[1] << ' ' << hhe->startv->p[2] << endl;
			//}; //For_Test
			pre_up_v = up_v;
			he = he->next;
			nextv = he->startv;
		}
		mef( first_he->endv, pre_up_v,  tlp);

		//cout << "abc:" << endl;
	    //HalfEdge* he = tlp->halfedges;
		//for (int i = 0; i < 10; i++) {
		//    cout << he->startv->p[0] << " " << he->startv->p[1] << " " << he->startv->p[2] << endl;
		//    he = he->next;
		//}

	}
}