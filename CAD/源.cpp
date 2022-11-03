#include "include_all.h"
#ifndef _WIN32
#define __stdcall
#endif

using namespace std;
using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;
using std::ends;

#ifndef CALLBACK
#define CALLBACK
#endif

// CALLBACK functions for GLU_TESS ////////////////////////////////////////////
// NOTE: must be declared with CALLBACK directive
void CALLBACK tessBeginCB(GLenum which);
void CALLBACK tessEndCB();
void CALLBACK tessErrorCB(GLenum errorCode);
void CALLBACK tessVertexCB(const GLvoid* data);
void CALLBACK tessVertexCB2(const GLvoid* data);
void CALLBACK tessCombineCB(const GLdouble newVertex[3], const GLdouble* neighborVertex[4],
    const GLfloat neighborWeight[4], GLdouble** outData);

// GLUT CALLBACK functions ////////////////////////////////////////////////////
void displayCB();
void reshapeCB(int w, int h);
void timerCB(int millisec);
void idleCB();
void keyboardCB(unsigned char key, int x, int y);
void mouseCB(int button, int stat, int x, int y);
void mouseMotionCB(int x, int y);

// function declarations //////////////////////////////////////////////////////
void initGL();
int  initGLUT(int argc, char** argv);
bool initSharedMem();
void clearSharedMem();
void initLights();
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
void drawString(const char* str, int x, int y, float color[4], void* font);
void drawString3D(const char* str, float pos[3], float color[4], void* font);
void showInfo();
const char* getPrimitiveType(GLenum type);
GLuint show_face(int d);

// global variables ///////////////////////////////////////////////////////////
void* font = GLUT_BITMAP_9_BY_15;
bool mouseLeftDown;
bool mouseRightDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance;
int drawMode = 0;
GLuint listId1, listId2, listId3;       // IDs of display lists
GLdouble vertices[64][6];               // arrary to store newly created vertices (x,y,z,r,g,b) by combine callback
int vertexIndex = 0;                    // array index for above array incremented inside combine callback
Solid* mysolid = nullptr;
int color_faces[100][3];
// DEBUG //
stringstream ss;

// model ///////////////////////////////////////////////////////////
void Cube();
void House();
void DiySweeping();

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    for (int i = 0; i < 100; i++)
    {
        unsigned char red = rand() % 255;
        unsigned char green = rand() % 255;
        unsigned char blue = rand() % 255;
        color_faces[i][0] = red;
        color_faces[i][1] = green;
        color_faces[i][2] = blue;
    }

    initSharedMem();

    // init GLUT and GL
    initGLUT(argc, argv);
    initGL();

    Cube();
    listId1 = show_face(-300);
    DiySweeping();
    listId2 = show_face(-100);
    House();
    listId3 = show_face(200);    
    glutMainLoop(); /* Start GLUT event-processing loop */

    return 0;
}


GLuint show_face(int d)
{
    if (mysolid != NULL) {
        GLuint id = glGenLists(1);  // create a display list
        if (!id) return id;          // failed to create a list, return 0
        GLdouble p[1000][3];
        GLUtesselator* tess = gluNewTess(); // create a tessellator
        if (!tess) return 0;         // failed to create tessellation object, return 0

        // register callback functions
        gluTessCallback(tess, GLU_TESS_BEGIN, (void(__stdcall*)(void))tessBeginCB);
        gluTessCallback(tess, GLU_TESS_END, (void(__stdcall*)(void))tessEndCB);
        gluTessCallback(tess, GLU_TESS_ERROR, (void(__stdcall*)(void))tessErrorCB);
        gluTessCallback(tess, GLU_TESS_VERTEX, (void(__stdcall*)())tessVertexCB);
        glNewList(id, GL_COMPILE);
        Face* face = mysolid->faces;
        int index = 0, vid = 0;
        while (face) {//依次绘制各面
            gluTessBeginPolygon(tess, 0);
            Loop* temploop = face->outloop;
            //采用随机产生的color来绘制该面
            unsigned char red = color_faces[index][0];
            unsigned char green = color_faces[index][1];
            unsigned char blue = color_faces[index][2];
            //cout << index << ": " << endl;
            while (temploop) {//遍历当前面的各点
                glColor3b(red, green, blue);
                HalfEdge* halfedge = temploop->halfedges;
                Vertex* start = halfedge->startv;
                gluTessBeginContour(tess);
                //cout << start->p[0] << ' ' << start->p[1] << ' ' << start->p[2] << endl;
                //p[vid][0] = start->p[0]; p[vid][1] = start->p[1]; p[vid][2] = start->p[2];
                p[vid][0] = start->p[0]+d; p[vid][1] = start->p[1]; p[vid][2] = start->p[2];
                gluTessVertex(tess, p[vid], p[vid]); vid++;
                Vertex* tmp = halfedge->next->startv;
                halfedge = halfedge->next;
                while (tmp != start)
                {
                    //cout << tmp->p[0] << ' ' << tmp->p[1] << ' ' << tmp->p[2] << endl;
                    //p[vid][0] = tmp->p[0]; p[vid][1] = tmp->p[1]; p[vid][2] = tmp->p[2];
                    p[vid][0] = tmp->p[0]+d; p[vid][1] = tmp->p[1]; p[vid][2] = tmp->p[2];
                    gluTessVertex(tess, p[vid], p[vid]); vid++;
                    halfedge = halfedge->next;
                    tmp = halfedge->startv;
                }
                gluTessEndContour(tess);
                temploop = temploop->next;
            }
            gluTessEndPolygon(tess);
            face = face->next;
            index++;
            cout << endl;
        }
        glEndList();
        return id;      // return handle ID of a display list
    }
}


///////////////////////////////////////////////////////////////////////////////
// convert enum of OpenGL primitive type to a string(char*)
// OpenGL supports only 10 primitive types.
///////////////////////////////////////////////////////////////////////////////
const char* getPrimitiveType(GLenum type)
{
    switch (type)
    {
    case 0x0000:
        return "GL_POINTS";
        break;
    case 0x0001:
        return "GL_LINES";
        break;
    case 0x0002:
        return "GL_LINE_LOOP";
        break;
    case 0x0003:
        return "GL_LINE_STRIP";
        break;
    case 0x0004:
        return "GL_TRIANGLES";
        break;
    case 0x0005:
        return "GL_TRIANGLE_STRIP";
        break;
    case 0x0006:
        return "GL_TRIANGLE_FAN";
        break;
    case 0x0007:
        return "GL_QUADS";
        break;
    case 0x0008:
        return "GL_QUAD_STRIP";
        break;
    case 0x0009:
        return "GL_POLYGON";
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
// initialize GLUT for windowing
///////////////////////////////////////////////////////////////////////////////
int initGLUT(int argc, char** argv)
{
    // GLUT stuff for windowing
    // initialization openGL window.
    // it is called before any other GLUT routine
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);   // display mode

    glutInitWindowSize(1200, 700);               // window size

    glutInitWindowPosition(100, 100);           // window location

    // finally, create a window with openGL context
    // Window will not displayed until glutMainLoop() is called
    // it returns a unique ID
    int handle = glutCreateWindow(argv[0]);     // param is the title of window

    // register GLUT callback functions
    glutDisplayFunc(displayCB);
    //glutDisplayFunc(glut_display_func);
    glutTimerFunc(33, timerCB, 33);             // redraw only every given millisec
    //glutIdleFunc(idleCB);                       // redraw only every given millisec
    glutReshapeFunc(reshapeCB);
    glutKeyboardFunc(keyboardCB);
    glutMouseFunc(mouseCB);
    glutMotionFunc(mouseMotionCB);

    return handle;
}



///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL
// disable unused features
///////////////////////////////////////////////////////////////////////////////
void initGL()
{
    glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

    // enable /disable features
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    //glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);

    // track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glClearColor(131.0 / 255, 175.0 / 255, 155.0/255,1);                   // background color
    glClearStencil(0);                          // clear stencil buffer
    glClearDepth(1.0f);                         // 0 is near, 1 is far
    glDepthFunc(GL_LEQUAL);

    initLights();
    setCamera(0,0,500, 0, 0, 0);
}



///////////////////////////////////////////////////////////////////////////////
// write 2d text using GLUT
// The projection matrix must be set to orthogonal before call this function.
///////////////////////////////////////////////////////////////////////////////
void drawString(const char* str, int x, int y, float color[4], void* font)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color

    glColor4fv(color);          // set text color
    glRasterPos2i(x, y);        // place text position

    // loop all characters in the string
    while (*str)
    {
        glutBitmapCharacter(font, *str);
        ++str;
    }

    glEnable(GL_LIGHTING);
    glPopAttrib();
}



///////////////////////////////////////////////////////////////////////////////
// draw a string in 3D space
///////////////////////////////////////////////////////////////////////////////
void drawString3D(const char* str, float pos[3], float color[4], void* font)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color

    glColor4fv(color);          // set text color
    glRasterPos3fv(pos);        // place text position

    // loop all characters in the string
    while (*str)
    {
        glutBitmapCharacter(font, *str);
        ++str;
    }

    glEnable(GL_LIGHTING);
    glPopAttrib();
}



///////////////////////////////////////////////////////////////////////////////
// initialize global variables
///////////////////////////////////////////////////////////////////////////////
bool initSharedMem()
{
    mouseLeftDown = mouseRightDown = false;
    return true;
}



///////////////////////////////////////////////////////////////////////////////
// clean up shared memory
///////////////////////////////////////////////////////////////////////////////
void clearSharedMem()
{
}



///////////////////////////////////////////////////////////////////////////////
// initialize lights
///////////////////////////////////////////////////////////////////////////////
void initLights()
{
    // set up light colors (ambient, diffuse, specular)
    GLfloat lightKa[] = { .2f, .2f, .2f, 1.0f };  // ambient light
    GLfloat lightKd[] = { .7f, .7f, .7f, 1.0f };  // diffuse light
    GLfloat lightKs[] = { 1, 1, 1, 1 };           // specular light
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

    // position the light
    float lightPos[4] = { 0, 0, 200, 1 }; // positional light
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration
}



///////////////////////////////////////////////////////////////////////////////
// set camera position and lookat direction
///////////////////////////////////////////////////////////////////////////////
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
}



///////////////////////////////////////////////////////////////////////////////
// display info messages
///////////////////////////////////////////////////////////////////////////////
void showInfo()
{
    // backup current model-view matrix
    glPushMatrix();                     // save current modelview matrix
    glLoadIdentity();                   // reset modelview matrix

    // set to 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);     // switch to projection matrix
    glPushMatrix();                  // save current projection matrix
    glLoadIdentity();                // reset projection matrix
    gluOrtho2D(0, 400, 0, 200);  // set to orthogonal projection

    float color[4] = { 0, 0, 0, 1 };

    stringstream ss;
    ss << std::fixed << std::setprecision(3);

    if (drawMode == 0)
        ss << "Draw Mode: Fill" << ends;
    else if (drawMode == 1)
        ss << "Draw Mode: Wireframe" << ends;
    else
        ss << "Draw Mode: Points" << ends;
    drawString(ss.str().c_str(), 1, 150, color, font);
    ss.str("");

    ss << "Press 'D' to switch drawing mode." << ends;
    drawString(ss.str().c_str(), 1, 30, color, font);
    ss.str("");

    ss << "Absolute Eular" << ends;
    drawString(ss.str().c_str(), 50, 60, color, font);
    ss.str("");

    ss << "DIY Euler and Sweep" << ends;
    drawString(ss.str().c_str(), 150, 60, color, font);
    ss.str("");

    ss << "Euler and Sweep" << ends;
    drawString(ss.str().c_str(), 250, 60, color, font);
    ss.str("");
    // unset floating format
    ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

    // restore projection matrix
    glPopMatrix();                   // restore to previous projection matrix

    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
    glPopMatrix();                   // restore to previous modelview matrix
}



//=============================================================================
// CALLBACKS
//=============================================================================

void displayCB()
{
    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // save the initial ModelView matrix before modifying ModelView matrix
    glPushMatrix();

    // tramsform camera
    glTranslatef(0, 0, cameraDistance);
    glRotatef(cameraAngleX, 1, 0, 0);   // pitch
    glRotatef(cameraAngleY, 0, 1, 0);   // heading

    // draw meshes
    glTranslatef(-4, -1.5f, 0);
    glCallList(listId1);

    glTranslatef(4, 0, 0);
    glCallList(listId2);

    glTranslatef(4, 0, 0);
    glCallList(listId3);

    // draw info messages
    showInfo();

    glPopMatrix();

    glutSwapBuffers();
}


void reshapeCB(int w, int h)
{
    // set viewport to be the entire window
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    // set perspective viewing frustum
    float aspectRatio = (float)w / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glFrustum(-aspectRatio, aspectRatio, -1, 1, 1, 100);
    gluPerspective(60.0f, (float)(w) / h, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip

    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
}


void timerCB(int millisec)
{
    glutTimerFunc(millisec, timerCB, millisec);
    glutPostRedisplay();
}


void idleCB()
{
    glutPostRedisplay();
}


void keyboardCB(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27: // ESCAPE
        clearSharedMem();
        exit(0);
        break;

    case ' ':
        break;

    case 'd': // switch rendering modes (fill -> wire -> point)
    case 'D':
        drawMode = ++drawMode % 3;
        if (drawMode == 0)        // fill mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_DEPTH_TEST);
            //glEnable(GL_CULL_FACE);
            glDisable(GL_CULL_FACE);
        }
        else if (drawMode == 1)  // wireframe mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        else                    // point mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        break;

    default:
        ;
    }

    glutPostRedisplay();
}


void mouseCB(int button, int state, int x, int y)
{
    mouseX = x;
    mouseY = y;

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            mouseLeftDown = true;
        }
        else if (state == GLUT_UP)
            mouseLeftDown = false;
    }

    else if (button == GLUT_RIGHT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            mouseRightDown = true;
        }
        else if (state == GLUT_UP)
            mouseRightDown = false;
    }
}


void mouseMotionCB(int x, int y)
{
    if (mouseLeftDown)
    {
        cameraAngleY += (x - mouseX);
        cameraAngleX += (y - mouseY);
        mouseX = x;
        mouseY = y;
    }
    if (mouseRightDown)
    {
        cameraDistance += (y - mouseY) * 0.2f;
        mouseY = y;
    }

    //glutPostRedisplay();
}



///////////////////////////////////////////////////////////////////////////////
// GLU_TESS CALLBACKS
///////////////////////////////////////////////////////////////////////////////
void CALLBACK tessBeginCB(GLenum which)
{
    glBegin(which);

    // DEBUG //
    ss << "glBegin(" << getPrimitiveType(which) << ");\n";
}



void CALLBACK tessEndCB()
{
    glEnd();

    // DEBUG //
    ss << "glEnd();\n";
}



void CALLBACK tessVertexCB(const GLvoid* data)
{
    // cast back to double type
    const GLdouble* ptr = (const GLdouble*)data;

    glVertex3dv(ptr);

    // DEBUG //
    ss << "  glVertex3d(" << *ptr << ", " << *(ptr + 1) << ", " << *(ptr + 2) << ");\n";
}


///////////////////////////////////////////////////////////////////////////////
// draw a vertex with color
///////////////////////////////////////////////////////////////////////////////
void CALLBACK tessVertexCB2(const GLvoid* data)
{
    // cast back to double type
    const GLdouble* ptr = (const GLdouble*)data;

    glColor3dv(ptr + 3);
    glVertex3dv(ptr);

    // DEBUG //
    ss << "  glColor3d(" << *(ptr + 3) << ", " << *(ptr + 4) << ", " << *(ptr + 5) << ");\n";
    ss << "  glVertex3d(" << *ptr << ", " << *(ptr + 1) << ", " << *(ptr + 2) << ");\n";
}

void CALLBACK tessCombineCB(const GLdouble newVertex[3], const GLdouble* neighborVertex[4],
    const GLfloat neighborWeight[4], GLdouble** outData)
{
    // copy new intersect vertex to local array
    // Because newVertex is temporal and cannot be hold by tessellator until next
    // vertex callback called, it must be copied to the safe place in the app.
    // Once gluTessEndPolygon() called, then you can safly deallocate the array.
    vertices[vertexIndex][0] = newVertex[0];
    vertices[vertexIndex][1] = newVertex[1];
    vertices[vertexIndex][2] = newVertex[2];

    // compute vertex color with given weights and colors of 4 neighbors
    // the neighborVertex[4] must hold required info, in this case, color.
    // neighborVertex was actually the third param of gluTessVertex() and is
    // passed into here to compute the color of the intersect vertex.
    vertices[vertexIndex][3] = neighborWeight[0] * neighborVertex[0][3] +   // red
        neighborWeight[1] * neighborVertex[1][3] +
        neighborWeight[2] * neighborVertex[2][3] +
        neighborWeight[3] * neighborVertex[3][3];
    vertices[vertexIndex][4] = neighborWeight[0] * neighborVertex[0][4] +   // green
        neighborWeight[1] * neighborVertex[1][4] +
        neighborWeight[2] * neighborVertex[2][4] +
        neighborWeight[3] * neighborVertex[3][4];
    vertices[vertexIndex][5] = neighborWeight[0] * neighborVertex[0][5] +   // blue
        neighborWeight[1] * neighborVertex[1][5] +
        neighborWeight[2] * neighborVertex[2][5] +
        neighborWeight[3] * neighborVertex[3][5];


    // return output data (vertex coords and others)
    *outData = vertices[vertexIndex];   // assign the address of new intersect vertex

    ++vertexIndex;  // increase index for next vertex
}

void CALLBACK tessErrorCB(GLenum errorCode)
{
    const GLubyte* errorStr;

    errorStr = gluErrorString(errorCode);
    cerr << "[ERROR]: " << errorStr << endl;
}

void Cube() {
    mysolid = nullptr;
    Vertex* v[1000] = { NULL };
    Loop* lp[1000] = { NULL };
    mysolid = mvfs(-50, -50, 0, v[0]);
    lp[0] = mysolid->faces->outloop;
    v[1] = mev(v[0], 50, -50, 0, lp[0])->endv;
    v[2] = mev(v[1], 50, 50, 0, lp[0])->endv;
    v[3] = mev(v[2], -50, 50, 0, lp[0])->endv;
    lp[1] = mef(v[3], v[0], lp[0]);

    v[4] = mev(v[0], -50, -50, 50, lp[0])->endv;
    v[5] = mev(v[1], 50, -50, 50, lp[0])->endv;
    v[6] = mev(v[2], 50, 50, 50, lp[0])->endv;
    v[7] = mev(v[3], -50, 50, 50, lp[0])->endv;
    mef(v[4], v[5], lp[0]);
    mef(v[5], v[6], lp[0]);
    mef(v[6], v[7], lp[0]);
    mef(v[7], v[4], lp[0]);

    v[8] = mev(v[0], -25, -25, 0, lp[1])->endv;
    v[9] = mev(v[8], 25, -25, 0, lp[1])->endv;
    v[10] = mev(v[9], 25, 25, 0, lp[1])->endv;
    v[11] = mev(v[10], -25, 25, 0, lp[1])->endv;
    lp[2] = mef(v[11], v[8], lp[1]);
    lp[7] = kemr(v[0], v[8], lp[1]);

    v[12] = mev(v[8], -25, -25, 50, lp[2])->endv;
    v[13] = mev(v[9], 25, -25, 50, lp[2])->endv;
    v[14] = mev(v[10], 25, 25, 50, lp[2])->endv;
    v[15] = mev(v[11], -25, 25, 50, lp[2])->endv;
    mef(v[13], v[12], lp[2]);
    mef(v[14], v[13], lp[2]);
    mef(v[15], v[14], lp[2]);
    mef(v[12], v[15], lp[2]);

    kfmrh(lp[0], lp[2]);
}

void House() {
    Vertex* v[1000] = { NULL };
    Loop* lp[1000] = { NULL };
    mysolid = mvfs(-50, 0, -100, v[0]);
    lp[0] = mysolid->faces->outloop;
    v[1] = mev(v[0], 50, 0, -100, lp[0])->endv;
    v[2] = mev(v[1], 50, 0, 0, lp[0])->endv;
    v[3] = mev(v[2], 100, 0, 0, lp[0])->endv;
    v[4] = mev(v[3], 0, 0, 100, lp[0])->endv;
    v[5] = mev(v[4], -100, 0, 0, lp[0])->endv;
    v[6] = mev(v[5], -50, 0, 0, lp[0])->endv;
    lp[1] = mef(v[6], v[0], lp[0]);

    v[7] = mev(v[0], -30, 0, -60, lp[1])->endv;
    v[8] = mev(v[7], -10, 0, -60, lp[1])->endv;
    v[9] = mev(v[8], -10, 0, -40, lp[1])->endv;
    v[10] = mev(v[9], -30, 0, -40, lp[1])->endv;
    lp[2] = mef(v[10], v[7], lp[1]);
    kemr(v[0], v[7], lp[1]);
    kfmrh(lp[0], lp[2]);

    v[11] = mev(v[0], 10, 0, -60, lp[1])->endv;
    v[12] = mev(v[11], 30, 0, -60, lp[1])->endv;
    v[13] = mev(v[12], 30, 0, -40, lp[1])->endv;
    v[14] = mev(v[13], 10, 0, -40, lp[1])->endv;
    lp[3] = mef(v[14], v[11], lp[1]);
    kemr(v[0], v[11], lp[1]);
    kfmrh(lp[0], lp[3]);

    v[15] = mev(v[0], 10, 0, -20, lp[1])->endv;
    v[16] = mev(v[15], 30, 0, -20, lp[1])->endv;
    v[17] = mev(v[16], 30, 0, 0, lp[1])->endv;
    v[18] = mev(v[17], 10, 0, 0, lp[1])->endv;
    lp[4] = mef(v[18], v[15], lp[1]);
    kemr(v[0], v[15], lp[1]);
    kfmrh(lp[0], lp[4]);

    v[19] = mev(v[0], -30, 0, -20, lp[1])->endv;
    v[20] = mev(v[19], -10, 0, -20, lp[1])->endv;
    v[21] = mev(v[20], -10, 0, 0, lp[1])->endv;
    v[22] = mev(v[21], -30, 0, 0, lp[1])->endv;
    lp[5] = mef(v[22], v[19], lp[1]);
    kemr(v[0], v[19], lp[1]);
    kfmrh(lp[0], lp[5]);
    sweep(lp[0]->face, 0, 1, 0, 40);

}

void DiySweeping() {
    ifstream  fin;
    fin.open("DIYSWEEPING.txt", ios::in);
    if (!fin.is_open()) {
        cout << "cannot open the file" << endl;
    }
    string s;
    int sweep_x, sweep_y, sweep_z, sweep_d, Inner_Loop_Num, outnum, x, y, z, vid = 0, lpid = 0;
    fin >> s >> sweep_x >> sweep_y >> sweep_z >> sweep_d;
    cout << s << ' ' << sweep_x << ' ' << sweep_y << ' ' << sweep_z << ' ' << sweep_d << endl;
    fin >> s >> Inner_Loop_Num;
    cout << s << Inner_Loop_Num << endl;

    fin >> outnum;
    cout << "The point's number in out Loop :" << outnum << endl;
    fin >> x >> y >> z;
    cout << x << ' ' << y << ' ' << z << endl;
    mysolid = nullptr;
    Vertex* v[1000] = { NULL };
    Loop* lp[1000] = { NULL };
    mysolid = mvfs(x, y, z, v[0]);
    lp[lpid++] = mysolid->faces->outloop;

    for (vid = 1; vid < outnum; vid++) {//外边
        fin >> x >> y >> z;
        cout << x << ' ' << y << ' ' << z << endl;
        v[vid] = mev(v[vid - 1], x, y, z, lp[0])->endv;
    }
    lp[lpid++] = mef(v[vid - 1], v[0], lp[0]);

    for (int i = 0; i < Inner_Loop_Num; i++) {
        int innum;
        fin >> innum;
        cout << innum << endl;
        fin >> x >> y >> z;
        cout << x << ' ' << y << ' ' << z << endl;
        v[vid] = mev(v[0], x, y, z, lp[1])->endv;
        for (int j = 1; j < innum; j++) {
            fin >> x >> y >> z;
            cout << x << ' ' << y << ' ' << z << endl;
            v[vid + j] = mev(v[vid + j - 1], x, y, z, lp[1])->endv;
        }
        lp[lpid] = mef(v[vid + innum - 1], v[vid], lp[1]);
        kemr(v[0], v[vid], lp[1]);
        kfmrh(lp[0], lp[lpid]);
        lpid ++;
        vid += innum;
    }
    sweep(lp[0]->face, sweep_x, sweep_y, sweep_z, sweep_d);
    fin.close();
}
