#include "include_all.h"

using namespace std;

const float PI = 3.14159;
int screenWidth = 100;
int screenHeight = 100;
Solid* solidList = nullptr;

void drawLine(float* p, float* q);
void render();
void display(void);
void reshape(int w, int h);

void Cube();
void House();
void sweeping();
void DiySweeping();
int main(int argc, char** argv) {

    if (!glfwInit())   //初始化OpenGl
        return 1;

    //创建OpenGl窗口
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "Dear ImGui GLFW+OpenGL2 example", NULL, NULL);
    if (window == NULL)
        return 1;

    //设置OpenGl山下文
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // 设置ImGui舌下文.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    //设置颜色风格
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();
    ImVec4 clear_color = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        // Start the Dear ImGui frame 启动IMgui Frame框架.
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            //开始绘制ImGui
            ImGui::Begin("Wangyifu_12224089");                          // Create a window called "Hello, world!" and append into it.
            //ImGui::Text("");
            //ImGui::SameLine();
            ImGui::Indent(); //另起一行制表符开始绘制Button
            if (ImGui::Button("Cube", ImVec2(150, 30))) {
                Cube();
            }
            if (ImGui::Button("House", ImVec2(150, 30))) {
                House();
            }
            if (ImGui::Button("House and sweeping", ImVec2(150, 30))) {
                sweeping();
            }
            if (ImGui::Button("DIYface and sweeping", ImVec2(150, 30))) {
                DiySweeping();
            }
            ImGui::End();
        }
        // 3. Show another simple window.
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        glRotatef(0.5, 0.1, 0.1, 0.1);
        render();

        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData()); //必须在绘制完Open之后接着绘制Imgui
        //glUseProgram(last_program);
        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }
    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void drawLine(float* p, float* q) {
    glBegin(GL_LINE_STRIP); 
    glVertex3fv(p); 
    glVertex3fv(q);
    glEnd();
    glPointSize(5.f);
    glBegin(GL_POINTS);
    glVertex3fv(p);
    glVertex3fv(q);
    glEnd();
}

void render() {
    // *****绘制过程*******

    Solid* solid = solidList;
    for (; solid != NULL; solid = solid->next)
    {
        // 遍历solid中的边
        Edge* edge = solid->edges;
        for (; edge != NULL; edge = edge->next)
        {

            Vertex* v1 = edge->halfedge_l->startv;
            Vertex* v2 = edge->halfedge_r->startv;
            float point1[3] = { (double)v1->p[0] / 200, (double)v1->p[1] / 200, (double)v1->p[2] / 200 };
            float point2[3] = { (double)v2->p[0] / 200, (double)v2->p[1] / 200, (double)v2->p[2] / 200 };

            drawLine(point1, point2);

        }
    }
}


void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    screenWidth = w;
    screenHeight = h; glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho(-2.0, 2.0, -2.0 * (GLfloat)h / (GLfloat)w,
            2.0 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);
    else
        glOrtho(-2.0 * (GLfloat)w / (GLfloat)h,
            2.0 * (GLfloat)w / (GLfloat)h, -2.0, 2.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
}

//for (HalfEdge* he = lp[1]->halfedges; ; he = he->next) {
//    //cout << st->p[0] << ' ' << st->p[1] << ' ' << st->p[2] << endl;
//    cout << he->startv->p[0] << ' ' << he->startv->p[1] << ' ' << he->startv->p[2] << endl;
//    //cout << ed->p[0] << ' ' << ed->p[1] << ' ' << ed->p[2] << endl;
//    //cout << he->endv->p[0] << ' ' << he->endv->p[1] << ' ' << he->endv->p[2] << endl << endl;
//    //for_TEST
//}; //For_Test

void Cube() {
    solidList = nullptr;
    Vertex* v[100] = { NULL };
    Loop* lp[100] = { NULL };
    solidList = mvfs(-50, -50, 0, v[0]);
    lp[0] = solidList->faces->outloop;
    v[1] = mev(v[0], 50, -50, 0, lp[0])->endv;
    v[2] = mev(v[1], 50, 50, 0, lp[0])->endv;
    v[3] = mev(v[2], -50, 50, 0, lp[0])->endv;
    lp[1] = mef(v[3], v[0], lp[0]);

    v[4] = mev(v[0], -50, -50, 50, lp[1])->endv;
    v[5] = mev(v[1], 50, -50, 50, lp[1])->endv;
    v[6] = mev(v[2], 50, 50, 50, lp[1])->endv;
    v[7] = mev(v[3], -50, 50, 50, lp[1])->endv;
    mef(v[5], v[4], lp[1]);
    mef(v[6], v[5], lp[1]);
    mef(v[7], v[6], lp[1]);
    lp[2] = mef(v[4], v[7], lp[1]);

    v[8] = mev(v[0], -25, -25, 0, lp[0])->endv;
    v[9] = mev(v[8], 25, -25, 0, lp[0])->endv;
    v[10] = mev(v[9], 25, 25, 0, lp[0])->endv;
    v[11] = mev(v[10], -25, 25, 0, lp[0])->endv;
    lp[3] = mef(v[11], v[8], lp[0]);
    lp[4] = kemr(v[8], v[0], lp[3]);

    v[12] = mev(v[8], -25, -25, 50, lp[3])->endv;
    v[13] = mev(v[9], 25, -25, 50, lp[3])->endv;
    v[14] = mev(v[10], 25, 25, 50, lp[3])->endv;
    v[15] = mev(v[11], -25, 25, 50, lp[3])->endv;
    mef(v[13], v[12], lp[3]);
    mef(v[14], v[13], lp[3]);
    mef(v[15], v[14], lp[3]);
    mef(v[12], v[15], lp[3]);

    kfmrh(lp[1], lp[0]);
}

void House() {
    solidList = nullptr;
    Vertex* v[100] = { NULL };
    Loop* lp[100] = { NULL };
    solidList = mvfs(-50, 0, -100, v[0]);
    lp[0] = solidList->faces->outloop;
    v[1] = mev(v[0], 50, 0, -100, lp[0])->endv;
    v[2] = mev(v[1], 50, 0, 0, lp[0])->endv;
    v[3] = mev(v[2], 100, 0, 0, lp[0])->endv;
    v[4] = mev(v[3], 0, 0, 100, lp[0])->endv;
    v[5] = mev(v[4], -100, 0, 0, lp[0])->endv;
    v[6] = mev(v[5], -50, 0, 0, lp[0])->endv;
    lp[1] = mef(v[6], v[0], lp[0]);

    v[7] = mev(v[0], -30, 0, -60, lp[0])->endv;
    v[8] = mev(v[7], -10, 0, -60, lp[0])->endv;
    v[9] = mev(v[8], -10, 0, -40, lp[0])->endv;
    v[10] = mev(v[9], -30, 0, -40, lp[0])->endv;//lp[0]=房子上的环连接内部线上所有的环
    lp[2] = mef(v[10], v[7], lp[0]);//lp[0]=窗户外环，lp[2]=房子+窗户外+连接 的外环
    lp[3] = kemr(v[0], v[7], lp[2]);//lp[3]=房子的内环，lp[2]=房子的外环
    kfmrh(lp[2], lp[0]);

    v[11] = mev(v[0], 10, 0, -60, lp[2])->endv;
    v[12] = mev(v[11], 30, 0, -60, lp[2])->endv;
    v[13] = mev(v[12], 30, 0, -40, lp[2])->endv;
    v[14] = mev(v[13], 10, 0, -40, lp[2])->endv;
    lp[4] = mef(v[14], v[11], lp[2]);
    lp[5] = kemr(v[0], v[11], lp[4]);
    kfmrh(lp[4], lp[2]);

    v[15] = mev(v[0], 10, 0, -20, lp[4])->endv;
    v[16] = mev(v[15], 30, 0, -20, lp[4])->endv;
    v[17] = mev(v[16], 30, 0, 0, lp[4])->endv;
    v[18] = mev(v[17], 10, 0, 0, lp[4])->endv;
    lp[6] = mef(v[18], v[15], lp[4]);
    lp[7] = kemr(v[0], v[15], lp[6]);
    kfmrh(lp[6], lp[4]);

    v[19] = mev(v[0], -30, 0, -20, lp[6])->endv;
    v[20] = mev(v[19], -10, 0, -20, lp[6])->endv;
    v[21] = mev(v[20], -10, 0, 0, lp[6])->endv;
    v[22] = mev(v[21], -30, 0, 0, lp[6])->endv;
    lp[8] = mef(v[22], v[19], lp[6]);
    lp[9] = kemr(v[0], v[19], lp[8]);
    kfmrh(lp[8], lp[6]);
}

void sweeping() {
    solidList = nullptr;
    Vertex* v[100] = { NULL };
    Loop* lp[100] = { NULL };
    solidList = mvfs(-50, 0, -100, v[0]);
    lp[0] = solidList->faces->outloop;
    v[1] = mev(v[0], 50, 0, -100, lp[0])->endv;
    v[2] = mev(v[1], 50, 0, 0, lp[0])->endv;
    v[3] = mev(v[2], 100, 0, 0, lp[0])->endv;
    v[4] = mev(v[3], 0, 0, 100, lp[0])->endv;
    v[5] = mev(v[4], -100, 0, 0, lp[0])->endv;
    v[6] = mev(v[5], -50, 0, 0, lp[0])->endv;
    lp[1] = mef(v[6], v[0], lp[0]);

    v[7] = mev(v[0], -30, 0, -60, lp[0])->endv;
    v[8] = mev(v[7], -10, 0, -60, lp[0])->endv;
    v[9] = mev(v[8], -10, 0, -40, lp[0])->endv;
    v[10] = mev(v[9], -30, 0, -40, lp[0])->endv;//lp[0]=房子上的环连接内部线上所有的环
    lp[2] = mef(v[10], v[7], lp[0]);//lp[0]=窗户外环，lp[2]=房子+窗户外+连接 的外环
    lp[3] = kemr(v[0], v[7], lp[2]);//lp[3]=房子的内环，lp[2]=房子的外环
    kfmrh(lp[2], lp[0]);

    v[11] = mev(v[0], 10, 0, -60, lp[2])->endv;
    v[12] = mev(v[11], 30, 0, -60, lp[2])->endv;
    v[13] = mev(v[12], 30, 0, -40, lp[2])->endv;
    v[14] = mev(v[13], 10, 0, -40, lp[2])->endv;
    lp[4] = mef(v[14], v[11], lp[2]);
    lp[5] = kemr(v[0], v[11], lp[4]);
    kfmrh(lp[4], lp[2]);

    v[15] = mev(v[0], 10, 0, -20, lp[4])->endv;
    v[16] = mev(v[15], 30, 0, -20, lp[4])->endv;
    v[17] = mev(v[16], 30, 0, 0, lp[4])->endv;
    v[18] = mev(v[17], 10, 0, 0, lp[4])->endv;
    lp[6] = mef(v[18], v[15], lp[4]);
    lp[7] = kemr(v[0], v[15], lp[6]);
    kfmrh(lp[6], lp[4]);

    v[19] = mev(v[0], -30, 0, -20, lp[6])->endv;
    v[20] = mev(v[19], -10, 0, -20, lp[6])->endv;
    v[21] = mev(v[20], -10, 0, 0, lp[6])->endv;
    v[22] = mev(v[21], -30, 0, 0, lp[6])->endv;
    lp[8] = mef(v[22], v[19], lp[6]);
    lp[9] = kemr(v[0], v[19], lp[8]);
    kfmrh(lp[8], lp[6]);
    sweep(lp[8]->face, 0, 1, 0, 80);
}

void DiySweeping() {
    ifstream  fin;
    fin.open("DIYSWEEPING.txt", ios::in);
    if (!fin.is_open()) {
        cout << "cannot open the file" << endl;
    }
    string s;
    int sweep_x, sweep_y, sweep_z, sweep_d, Inner_Loop_Num, outnum, x, y, z, vid=0,lpid=0;
    fin >> s >> sweep_x >> sweep_y >> sweep_z >> sweep_d;
    cout << s << ' ' << sweep_x << ' ' << sweep_y << ' ' << sweep_z << ' ' << sweep_d << endl;
    fin >> s >> Inner_Loop_Num;
    cout << s << Inner_Loop_Num << endl;

    fin >> outnum;
    cout << "The point's number in out Loop :" << outnum << endl;
    fin >> x >> y >> z;
    cout << x << ' ' << y << ' ' << z << endl;
    solidList = nullptr;
    Vertex* v[100] = { NULL };
    Loop* lp[100] = { NULL };
    solidList = mvfs(x, y, z, v[0]);
    lp[lpid++] = solidList->faces->outloop;

    for (vid = 1; vid < outnum; vid++) {//外边
        fin >> x >> y >> z;
        cout << x << ' ' << y << ' ' << z << endl;
        v[vid]= mev(v[vid-1], x, y, z, lp[0])->endv;
    }
    lp[lpid++] = mef(v[vid-1], v[0], lp[0]);

    for (int i = 0; i < Inner_Loop_Num; i++) {
        int innum;
        fin >> innum;
        cout << innum << endl;
        fin >> x >> y >> z;
        cout << x << ' ' << y << ' ' << z << endl;
        v[vid ] = mev(v[0], x, y, z, lp[lpid -2])->endv;
        for (int j = 1; j < innum; j++) {
            fin >> x >> y >> z;
            cout << x << ' ' << y << ' ' << z << endl;
            v[vid+j]= mev(v[vid+j-1], x, y, z, lp[lpid-2])->endv;
        }
        lp[lpid] = mef(v[vid + innum-1], v[vid], lp[lpid-2]);
        lp[lpid+1] = kemr(v[0], v[vid], lp[lpid]);
        kfmrh(lp[lpid], lp[lpid-2]);
        lpid += 2;
        vid += innum;
    }
    sweep(lp[lpid-2]->face, sweep_x, sweep_y, sweep_z, sweep_d);
    fin.close();
}


