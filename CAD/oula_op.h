#include "Half_Edge_Struct.h"
#include <vector>
using namespace std;

Solid* mvfs(double x, double y, double z, Vertex*& newVertex);    //给定初始点，构造一个新的体,包含一个面,一个外环,一个点

HalfEdge* mev(Vertex* st, double x, double y, double z, Loop* lp); //给定一个新点，结束点,连接这两个点，形成一条新的边

Loop* mef(Vertex* st, Vertex* ed, Loop* lp);///给定一个新边，形成一个新的面,更新环

Loop* kemr(Vertex* st, Vertex* ed, Loop* lp);//删除一条边构造一个环

void kfmrh(Loop* outlp, Loop* lp);//删除一个与面F相接触的一个面f

void sweep(Face* face, double x, double y, double z, double d);//扫成