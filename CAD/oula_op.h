#include "Half_Edge_Struct.h"
#include <vector>
using namespace std;

Solid* mvfs(double x, double y, double z, Vertex*& newVertex);    //������ʼ�㣬����һ���µ���,����һ����,һ���⻷,һ����

HalfEdge* mev(Vertex* st, double x, double y, double z, Loop* lp); //����һ���µ㣬������,�����������㣬�γ�һ���µı�

Loop* mef(Vertex* st, Vertex* ed, Loop* lp);///����һ���±ߣ��γ�һ���µ���,���»�

Loop* kemr(Vertex* st, Vertex* ed, Loop* lp);//ɾ��һ���߹���һ����

void kfmrh(Loop* outlp, Loop* lp);//ɾ��һ������F��Ӵ���һ����f

void sweep(Face* face, double x, double y, double z, double d);//ɨ��