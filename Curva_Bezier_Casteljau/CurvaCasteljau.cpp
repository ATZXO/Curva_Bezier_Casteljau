#include <gl/glut.h>
#include <vector>
#include <iostream>
#include <cmath>

struct Punto {
    float x, y;
};

std::vector<Punto> puntos;

bool insertarPuntos = false;
bool mostrarCurva = false;
bool modificarCurva = false;
bool eliminarPuntos = false;
int puntoSeleccionado = -1;
float colorCurva[3] = { 0.0, 1.0, 0.0 };
float colorCurva3D[3] = { 0.0, 1.0, 0.0 };
float grosorCurva = 2.0;
float anguloX = 0.0f, anguloY = 0.0f, anguloZ = 0.0f;

void dibujar_Etiqueta(int indice, float x, float y) {
    char etiqueta = 'A' + indice;
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(x, y + 0.04); 
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, etiqueta);
}

void dibujar_Puntos() {
    glColor3f(0.0, 0.0, 1.0);
    glPointSize(7.0);
    glBegin(GL_POINTS);
    for (auto& p : puntos) {
        glVertex2f(p.x, p.y);
    }
    glEnd();
    for (size_t i = 0; i < puntos.size(); i++) {
        Punto p = puntos[i];
        dibujar_Etiqueta(i, p.x, p.y);
    }
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(1.0);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(3, 0xAAAA);
    for (size_t i = 1; i < puntos.size(); i++) {
        glBegin(GL_LINES);
        glVertex2f(puntos[i - 1].x, puntos[i - 1].y);
        glVertex2f(puntos[i].x, puntos[i].y);
        glEnd();
    }
    glDisable(GL_LINE_STIPPLE);
}

Punto deCasteljau(std::vector<Punto>& puntos, int i, int j, float u) {
    if (i == 0) {
        return puntos[j];
    }
    Punto p1 = deCasteljau(puntos, i - 1, j, u);
    Punto p2 = deCasteljau(puntos, i - 1, j + 1, u);

    // Fórmula de De Casteljau
    return {
        (1 - u) * p1.x + u * p2.x,
        (1 - u) * p1.y + u * p2.y
    };
}

void curva_Bezier() {
    if (!mostrarCurva || puntos.size() < 2) return; 

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glColor3f(colorCurva[0], colorCurva[1], colorCurva[2]);
    glLineWidth(grosorCurva);

    glBegin(GL_LINE_STRIP);
    for (float u = 0; u <= 1.0f; u += 0.01f) { 
        Punto punto = deCasteljau(puntos, puntos.size() - 1, 0, u); // Llamar al algoritmo de Casteljau
        glVertex2f(punto.x, punto.y); 
    }
    glEnd();

    glDisable(GL_LINE_SMOOTH);
}

void mouse(int button, int state, int x, int y) {
    int anchoMitad = glutGet(GLUT_WINDOW_WIDTH) / 2; // Ancho de la mitad izquierda
    int alto = glutGet(GLUT_WINDOW_HEIGHT); // Alto completo de la ventana

    if (x < anchoMitad) { // Solo considera clics dentro del espacio 2D (mitad izquierda)
        float wx = ((float)x / anchoMitad) * 2.0f - 1.0f; // Mapear x al rango [-1, 1]
        float wy = 1.0f - ((float)y / alto) * 2.0f; // Mapear y al rango [-1, 1] (invertido)

        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            if (insertarPuntos) {
                puntos.push_back({ wx, wy }); // Agregar un nuevo punto
                glutPostRedisplay();
            }
            else if (modificarCurva) {
                for (int i = 0; i < puntos.size(); i++) {
                    if (std::hypot(puntos[i].x - wx, puntos[i].y - wy) < 0.05) {
                        puntoSeleccionado = i;
                        break;
                    }
                }
            }
            else if (eliminarPuntos) {
                for (int i = 0; i < puntos.size(); i++) {
                    if (std::hypot(puntos[i].x - wx, puntos[i].y - wy) < 0.05) {
                        puntos.erase(puntos.begin() + i);
                        glutPostRedisplay();
                        break;
                    }
                }
            }
        }
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        puntoSeleccionado = -1;
    }
}

void motion(int x, int y) {
    if (puntoSeleccionado >= 0 && modificarCurva) {
        int anchoMitad = glutGet(GLUT_WINDOW_WIDTH) / 2;
        int alto = glutGet(GLUT_WINDOW_HEIGHT);

        if (x < anchoMitad) { // Solo modifica puntos dentro del espacio 2D
            float wx = ((float)x / anchoMitad) * 2.0f - 1.0f;
            float wy = 1.0f - ((float)y / alto) * 2.0f;
            puntos[puntoSeleccionado] = { wx, wy }; // Actualiza la posición del punto
            glutPostRedisplay();
        }
    }
}

void curva_Bezier_3D() {
    //Dibujar los puntos de control
    glColor3f(1.0, 0.0, 0.0); 
    for (auto& p : puntos) {
        glPushMatrix(); 
        glTranslatef(p.x, p.y, 0.0); 
        glutSolidSphere(0.02, 10, 10); 
        glPopMatrix(); 
    }

    //Dibujar lineas entre los puntos de control
    glBegin(GL_LINE_STRIP);
    for (auto& p : puntos) {
        glVertex3f(p.x, p.y, 0.0); 
    }
    glEnd();

    //Dibujar la curva Bezier Casteljau
    if (!mostrarCurva || puntos.size() < 2) return;

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glColor3f(colorCurva3D[0], colorCurva3D[1], colorCurva3D[2]);
    glLineWidth(3);
    glBegin(GL_LINE_STRIP);
    for (float u = 0; u <= 1.0f; u += 0.01f) {
        Punto punto = deCasteljau(puntos, puntos.size() - 1, 0, u); 
        glVertex3f(punto.x, punto.y, 0.0);
    }
    glEnd();
    glDisable(GL_LINE_SMOOTH);

}

void limpiarPuntos() {
    puntos.clear();
    mostrarCurva = false;
    glutPostRedisplay();
}

void menu_eliminar(int opcion) {
    switch (opcion) {
    case 1: eliminarPuntos = true; break;
    case 2: eliminarPuntos = false; break;
    }
}

void menu_modificar(int opcion) {
    switch (opcion) {
    case 1: modificarCurva = true; break;
    case 2: modificarCurva = false; break;
    }
}

void menu_dibujar(int opcion) {
    switch (opcion) {
    case 1: mostrarCurva = true; curva_Bezier(); glutPostRedisplay(); break;
    case 2: mostrarCurva = false; curva_Bezier(); glutPostRedisplay(); break;
    }
}

void menu_color_Curva(int opcion) {
    switch (opcion) {
    case 1: colorCurva[0] = 1.0; colorCurva[1] = 0.0; colorCurva[2] = 0.0; break; 
    case 2: colorCurva[0] = 0.0; colorCurva[1] = 1.0; colorCurva[2] = 0.0; break; 
    case 3: colorCurva[0] = 0.0; colorCurva[1] = 0.0; colorCurva[2] = 1.0; break; 
    case 4: colorCurva[0] = 1.0; colorCurva[1] = 1.0; colorCurva[2] = 0.0; break; 
    case 5: colorCurva[0] = 1.0; colorCurva[1] = 0.5; colorCurva[2] = 0.0; break; 
    case 6: colorCurva[0] = 0.5; colorCurva[1] = 0.0; colorCurva[2] = 0.5; break; 
    }
    glutPostRedisplay();
}

void menu_color_Superficie(int opcion) {
    switch (opcion) {
    case 1: colorCurva3D[0] = 1.0; colorCurva3D[1] = 0.0; colorCurva3D[2] = 0.0; break;
    case 2: colorCurva3D[0] = 0.0; colorCurva3D[1] = 1.0; colorCurva3D[2] = 0.0; break;
    case 3: colorCurva3D[0] = 0.0; colorCurva3D[1] = 0.0; colorCurva3D[2] = 1.0; break;
    case 4: colorCurva3D[0] = 0.0; colorCurva3D[1] = 0.5; colorCurva3D[2] = 0.5; break;
    case 5: colorCurva3D[0] = 1.0; colorCurva3D[1] = 0.87; colorCurva3D[2] = 0.68; break;
    case 6: colorCurva3D[0] = 1.0; colorCurva3D[1] = 0.0; colorCurva3D[2] = 1.0; break;
    }
    glutPostRedisplay();
}

void menu_grosor(int opcion) {
    switch (opcion) {
    case 1: grosorCurva = 1.0; break;
    case 2: grosorCurva = 3.0; break;
    case 3: grosorCurva = 5.0; break;
    }
    glutPostRedisplay();
}

void menu_puntos(int opcion)
{
    switch (opcion) {
    case 1: insertarPuntos = true; dibujar_Puntos(); break;
    case 2: insertarPuntos = false; break;
    }
}

void menu_principal(int opcion) {
    switch (opcion) {
    case 7:
        limpiarPuntos();
        break;
    case 8:
        exit(0);
        break;
    }
}

void dibujarEjes2D() {
    glColor3f(1.0, 0.0, 0.0);
    glLineWidth(1.0);
    glBegin(GL_LINES);
    glVertex2f(-0.7, 0.0);
    glVertex2f(0.7, 0.0);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(0.0, -0.7);
    glVertex2f(0.0, 0.7);
    glEnd();
}

void dibujarEjes3D() {
    glColor3f(1.0, 0.0, 0.0); 
    glLineWidth(1.0);
    glBegin(GL_LINES);
    glVertex3f(-0.5, 0.0, 0.0);
    glVertex3f(0.5, 0.0, 0.0);
    glEnd();

    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, -0.5, 0.0);
    glVertex3f(0.0, 0.5, 0.0);
    glEnd();

    glColor3f(0.0, 0.0, 1.0); //Eje Z en azul
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, -0.5);
    glVertex3f(0.0, 0.0, 0.5);
    glEnd();
}

void teclado(unsigned char key, int x, int y) {
    switch (key) {
    case 'q': anguloY -= 5.0f; break; //Rota en sentido horario alrededor de Y
    case 'w': anguloY += 5.0f; break; //Rota en sentido antihorario alrededor de Y
    case 'a': anguloX += 5.0f; break; //Rota en sentido horario alrededor de X
    case 's': anguloX -= 5.0f; break; //Rota en sentido antihorario alrededor de X
    case 'z': anguloZ += 5.0f; break; //Rota en sentido horario alrededor de Z
    case 'x': anguloZ -= 5.0f; break; //Rota en sentido antihorario alrededor de Z
    }
    glutPostRedisplay();
}

void iniciar(void)
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glShadeModel(GL_FLAT);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int anchoMitad = glutGet(GLUT_WINDOW_WIDTH) / 2;
    int alto = glutGet(GLUT_WINDOW_HEIGHT);

    // Primer espacio: 2D
    glViewport(0, 0, anchoMitad, alto); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    dibujarEjes2D(); 
    dibujar_Puntos(); 
    curva_Bezier();

    // Segundo espacio: 3D
    glViewport(anchoMitad, 0, anchoMitad, alto); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)anchoMitad / alto, 0.1, 100.0); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(-0.5, 1, 2, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); 
    glRotatef(anguloX, 1.0, 0.0, 0.0);
    glRotatef(anguloY, 0.0, 1.0, 0.0);
    glRotatef(anguloZ, 0.0, 0.0, 1.0);
    dibujarEjes3D();
    curva_Bezier_3D();

    glFlush();
    glutSwapBuffers(); 
}

void myReshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 600);
    glutCreateWindow("Bezier-Casteljau");
    glutKeyboardFunc(teclado);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    //Submenu para el color de la cinta Spline
    int submenu_color_superficie = glutCreateMenu(menu_color_Superficie);
    glutAddMenuEntry("Rojo", 1);
    glutAddMenuEntry("Verde", 2);
    glutAddMenuEntry("Azul", 3);
    glutAddMenuEntry("Turquesa", 4);
    glutAddMenuEntry("Crema", 5);
    glutAddMenuEntry("Rosado", 6);
    //Submenu para activar o desactivar si se eliminan los puntos de control 
    int submenu_eliminar = glutCreateMenu(menu_eliminar);
    glutAddMenuEntry("Activar", 1);
    glutAddMenuEntry("Desactivar", 2);
    //Submenu para activar o desactivar si se modifican los puntos de control
    int submenu_modificar = glutCreateMenu(menu_modificar);
    glutAddMenuEntry("Activar", 1);
    glutAddMenuEntry("Desactivar", 2);
    //Submenu para activar o desactivar si se dibuja la curva Catmull-Rom Spline
    int submenu_dibujar = glutCreateMenu(menu_dibujar);
    glutAddMenuEntry("Activar", 1);
    glutAddMenuEntry("Desactivar", 2);
    //Submenu para color de la curva Catmull-Rom Spline
    int submenu_color_curva = glutCreateMenu(menu_color_Curva);
    glutAddMenuEntry("Rojo", 1);
    glutAddMenuEntry("Verde", 2);
    glutAddMenuEntry("Azul", 3);
    glutAddMenuEntry("Amarillo", 4);
    glutAddMenuEntry("Naranja", 5);
    glutAddMenuEntry("Purpura", 6);
    //Submenu para el grosor de la curva Catmull-Rom Spline
    int submenu_grosor = glutCreateMenu(menu_grosor);
    glutAddMenuEntry("Delgado", 1);
    glutAddMenuEntry("Medio", 2);
    glutAddMenuEntry("Grueso", 3);
    //Submenu para activar o desactivar si ingresan puntos de control 
    int submenu_puntos = glutCreateMenu(menu_puntos);
    glutAddMenuEntry("Activar", 1);
    glutAddMenuEntry("Desactivar", 2);
    //Menu principal, se le agrega los submenus creados anteriormente
    int menuPrincipal = glutCreateMenu(menu_principal);
    glutAddSubMenu("Insertar Puntos de Control", submenu_puntos);
    glutAddSubMenu("Graficar Curva Bezier-Casteljau", submenu_dibujar);
    glutAddSubMenu("Modificar Puntos de Control", submenu_modificar);
    glutAddSubMenu("Eliminar Puntos de Control", submenu_eliminar);
    glutAddSubMenu("Color de la Curva 2D", submenu_color_curva);
    glutAddSubMenu("Grosor de la Curva", submenu_grosor);
    glutAddSubMenu("Color de la Curva 3D", submenu_color_superficie);
    glutAddMenuEntry("Limpiar Puntos de Control", 7);
    glutAddMenuEntry("Salir", 8);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    iniciar();
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    return 0;
}
