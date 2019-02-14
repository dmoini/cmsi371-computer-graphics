/***
 assignment1.cpp
 Assignment-1: Cartoonify
 
 Name: Moini, Donovan
 
 Collaborators: Lizarda, Ian
 ** Note: although the assignment should be completed individually
 you may speak with classmates on high level algorithmic concepts. Please
 list their names in this section
 
 Project Summary: For this project, I chose to use Bezier's algorithm to draw a
 cartoon version of Andrew Forney. I used a website (cited below) to get the
 control points for the picture, and created a scaling function to translate
 the given points into points that would fit on the coordinate plan. While both
 an iterative and recursive approach would work, I took an iterative approach
 because the algorithm was simple enough to keep within a few loops.
 
 ***/


#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <vector>
#include <iostream>
using namespace std;

class Vertex {
    GLfloat x, y;
public:
    Vertex(GLfloat, GLfloat);
    GLfloat get_y() { return y; };
    GLfloat get_x() { return x; };
};

Vertex::Vertex(GLfloat X, GLfloat Y) {
    x = X;
    y = Y;
}

int width = 248;
int height = 312;

void setup() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

vector<Vertex> generate_midpoints(vector<Vertex> control_points) {
    vector<Vertex> midpoints;
    
    for (int i = 0; i < control_points.size() - 1; i++) {
        GLfloat x = (control_points[i].get_x() + control_points[i + 1].get_x()) / 2;
        GLfloat y = (control_points[i].get_y() + control_points[i + 1].get_y()) / 2;
        Vertex m = Vertex(x, y);
        midpoints.push_back(m);
    }
    return midpoints;
}

vector<Vertex> generate_points(vector<Vertex> control_points) {
    if (control_points.size() == 0) {
        return control_points;
    }
    
    vector<Vertex> midpoints = generate_midpoints(control_points);
    vector<Vertex> midpoints2 = generate_midpoints(midpoints);
    vector<Vertex> points;
    points.push_back(control_points[0]);
    points.push_back(midpoints[0]);

    for (int i = 0; i < midpoints2.size(); i++) {
        points.push_back(midpoints2[i]);
        points.push_back(midpoints[i + 1]);
    }
    
    points.push_back(control_points[control_points.size() - 1]);
    return points;
}

// Draw a Bezier curve based on the given control points
void draw_curve(vector<Vertex> control_points, int n_iter) {
    while (n_iter > 0) {
        control_points = generate_points(control_points);
        n_iter--;
    }
    glLineWidth(1.0);
    glBegin(GL_LINES);
    for (int i = 0; i < control_points.size() - 1; i++) {
        glVertex2f(control_points[i].get_x(), control_points[i].get_y());
        glVertex2f(control_points[i + 1].get_x(), control_points[i + 1].get_y());
    }
    glEnd();
}

// Converts points from website to appropriate points for program
// Points from https://www.mobilefish.com/services/record_mouse_coordinates/record_mouse_coordinates.php
Vertex scale(Vertex point) {
    GLfloat half_width = width / 2;
    GLfloat half_height = height / 2;
    
    GLfloat x = point.get_x() < half_width ? -(half_width - point.get_x()) / half_width : (point.get_x() - half_width) / half_width;
    GLfloat y = point.get_y() < half_height ? (half_height - point.get_y()) / half_height : -(point.get_y() - half_height) / half_height;
    return Vertex(x, y);
}

void draw_face() {
    vector<Vertex> left_ear;
    left_ear.push_back(scale(Vertex(32, 130)));
    left_ear.push_back(scale(Vertex(25, 137)));
    left_ear.push_back(scale(Vertex(25, 148)));
    left_ear.push_back(scale(Vertex(27, 161)));
    left_ear.push_back(scale(Vertex(28, 174)));
    left_ear.push_back(scale(Vertex(31, 185)));
    left_ear.push_back(scale(Vertex(38, 193)));
    left_ear.push_back(scale(Vertex(46, 192)));
    left_ear.push_back(scale(Vertex(47, 181)));
    left_ear.push_back(scale(Vertex(47, 170)));
    left_ear.push_back(scale(Vertex(45, 161)));
    left_ear.push_back(scale(Vertex(44, 151)));
    left_ear.push_back(scale(Vertex(43, 141)));
    left_ear.push_back(scale(Vertex(38, 135)));
    left_ear.push_back(scale(Vertex(32, 130)));
    draw_curve(left_ear, 10);
    
    vector<Vertex> right_ear;
    right_ear.push_back(scale(Vertex(225, 140)));
    right_ear.push_back(scale(Vertex(216, 147)));
    right_ear.push_back(scale(Vertex(213, 157)));
    right_ear.push_back(scale(Vertex(210, 169)));
    right_ear.push_back(scale(Vertex(207, 181)));
    right_ear.push_back(scale(Vertex(206, 193)));
    right_ear.push_back(scale(Vertex(208, 202)));
    right_ear.push_back(scale(Vertex(215, 194)));
    right_ear.push_back(scale(Vertex(220, 182)));
    right_ear.push_back(scale(Vertex(222, 170)));
    right_ear.push_back(scale(Vertex(227, 160)));
    right_ear.push_back(scale(Vertex(226, 149)));
    right_ear.push_back(scale(Vertex(225, 140)));
    draw_curve(right_ear, 10);
    
    vector<Vertex> top_lip;
    top_lip.push_back(scale(Vertex(89, 207)));
    top_lip.push_back(scale(Vertex(173, 211)));
    draw_curve(top_lip, 10);
    
    vector<Vertex> bottom_lip;
    bottom_lip.push_back(scale(Vertex(89, 207)));
    bottom_lip.push_back(scale(Vertex(100, 222)));
    bottom_lip.push_back(scale(Vertex(117, 231)));
    bottom_lip.push_back(scale(Vertex(132, 234)));
    bottom_lip.push_back(scale(Vertex(147, 231)));
    bottom_lip.push_back(scale(Vertex(161, 222)));
    bottom_lip.push_back(scale(Vertex(173, 211)));
    draw_curve(bottom_lip, 10);
    
    vector<Vertex> teeth1;
    teeth1.push_back(scale(Vertex(100, 219)));
    teeth1.push_back(scale(Vertex(113, 221)));
    teeth1.push_back(scale(Vertex(127, 222)));
    teeth1.push_back(scale(Vertex(139, 223)));
    teeth1.push_back(scale(Vertex(150, 220)));
    teeth1.push_back(scale(Vertex(163, 219)));
    draw_curve(teeth1, 1);
    
    vector<Vertex> teeth2;
    teeth2.push_back(scale(Vertex(97, 208)));
    teeth2.push_back(scale(Vertex(97, 216)));
    draw_curve(teeth2, 1);
    
    vector<Vertex> teeth3;
    teeth3.push_back(scale(Vertex(108, 208)));
    teeth3.push_back(scale(Vertex(108, 225)));
    draw_curve(teeth3, 1);
    
    vector<Vertex> teeth4;
    teeth4.push_back(scale(Vertex(119, 209)));
    teeth4.push_back(scale(Vertex(119, 231)));
    draw_curve(teeth4, 1);
    
    vector<Vertex> teeth5;
    teeth5.push_back(scale(Vertex(130, 209)));
    teeth5.push_back(scale(Vertex(130, 232)));
    draw_curve(teeth5, 1);
    
    vector<Vertex> teeth6;
    teeth6.push_back(scale(Vertex(141, 210)));
    teeth6.push_back(scale(Vertex(141, 231)));
    draw_curve(teeth6, 1);
    
    vector<Vertex> teeth7;
    teeth7.push_back(scale(Vertex(152, 210)));
    teeth7.push_back(scale(Vertex(152, 227)));
    draw_curve(teeth7, 1);
    
    vector<Vertex> teeth8;
    teeth8.push_back(scale(Vertex(163, 211)));
    teeth8.push_back(scale(Vertex(163, 220)));
    draw_curve(teeth8, 1);
    
    vector<Vertex> head;
    head.push_back(scale(Vertex(32, 125)));
    head.push_back(scale(Vertex(33, 98)));
    head.push_back(scale(Vertex(37, 70)));
    head.push_back(scale(Vertex(51, 42)));
    head.push_back(scale(Vertex(72, 22)));
    head.push_back(scale(Vertex(100, 11)));
    head.push_back(scale(Vertex(124, 8)));
    head.push_back(scale(Vertex(151, 11)));
    head.push_back(scale(Vertex(178, 19)));
    head.push_back(scale(Vertex(197, 32)));
    head.push_back(scale(Vertex(210, 51)));
    head.push_back(scale(Vertex(219, 72)));
    head.push_back(scale(Vertex(223, 99)));
    head.push_back(scale(Vertex(222, 117)));
    head.push_back(scale(Vertex(224, 136)));
    head.push_back(scale(Vertex(212, 156)));
    head.push_back(scale(Vertex(207, 178)));
    head.push_back(scale(Vertex(207, 201)));
    head.push_back(scale(Vertex(202, 219)));
    head.push_back(scale(Vertex(194, 234)));
    head.push_back(scale(Vertex(181, 250)));
    head.push_back(scale(Vertex(166, 263)));
    head.push_back(scale(Vertex(150, 275)));
    head.push_back(scale(Vertex(129, 284)));
    head.push_back(scale(Vertex(105, 279)));
    head.push_back(scale(Vertex(85, 265)));
    head.push_back(scale(Vertex(71, 249)));
    head.push_back(scale(Vertex(57, 228)));
    head.push_back(scale(Vertex(49, 207)));
    head.push_back(scale(Vertex(48, 186)));
    head.push_back(scale(Vertex(48, 163)));
    head.push_back(scale(Vertex(43, 143)));
    head.push_back(scale(Vertex(32, 125)));
    draw_curve(head, 10);
    
    vector<Vertex> nose;
    nose.push_back(scale(Vertex(127, 137)));
    nose.push_back(scale(Vertex(124, 161)));
    nose.push_back(scale(Vertex(110, 170)));
    nose.push_back(scale(Vertex(106, 179)));
    nose.push_back(scale(Vertex(111, 184)));
    nose.push_back(scale(Vertex(122, 182)));
    nose.push_back(scale(Vertex(135, 192)));
    nose.push_back(scale(Vertex(148, 181)));
    nose.push_back(scale(Vertex(162, 181)));
    nose.push_back(scale(Vertex(162, 176)));
    nose.push_back(scale(Vertex(153, 167)));
    nose.push_back(scale(Vertex(147, 137)));
    draw_curve(nose, 10);
    
    vector<Vertex> left_eye;
    left_eye.push_back(scale(Vertex(75, 137)));
    left_eye.push_back(scale(Vertex(87, 131)));
    left_eye.push_back(scale(Vertex(99, 131)));
    left_eye.push_back(scale(Vertex(113, 138)));
    left_eye.push_back(scale(Vertex(103, 139)));
    left_eye.push_back(scale(Vertex(93, 142)));
    left_eye.push_back(scale(Vertex(82, 139)));
    left_eye.push_back(scale(Vertex(75, 137)));
    draw_curve(left_eye, 10);
    
    vector<Vertex> left_pupil;
    left_pupil.push_back(scale(Vertex(88, 135)));
    left_pupil.push_back(scale(Vertex(88, 139)));
    left_pupil.push_back(scale(Vertex(94, 142)));
    left_pupil.push_back(scale(Vertex(100, 138)));
    left_pupil.push_back(scale(Vertex(94, 133)));
    left_pupil.push_back(scale(Vertex(88, 135)));
    draw_curve(left_pupil, 10);
    
    vector<Vertex> right_eye;
    right_eye.push_back(scale(Vertex(154, 139)));
    right_eye.push_back(scale(Vertex(162, 132)));
    right_eye.push_back(scale(Vertex(172, 131)));
    right_eye.push_back(scale(Vertex(181, 135)));
    right_eye.push_back(scale(Vertex(188, 141)));
    right_eye.push_back(scale(Vertex(179, 145)));
    right_eye.push_back(scale(Vertex(169, 144)));
    right_eye.push_back(scale(Vertex(161, 141)));
    right_eye.push_back(scale(Vertex(154, 139)));
    draw_curve(right_eye, 10);
    
    vector<Vertex> right_pupil;
    right_pupil.push_back(scale(Vertex(167, 142)));
    right_pupil.push_back(scale(Vertex(166, 139)));
    right_pupil.push_back(scale(Vertex(168, 134)));
    right_pupil.push_back(scale(Vertex(172, 134)));
    right_pupil.push_back(scale(Vertex(178, 135)));
    right_pupil.push_back(scale(Vertex(179, 140)));
    right_pupil.push_back(scale(Vertex(177, 144)));
    right_pupil.push_back(scale(Vertex(173, 143)));
    right_pupil.push_back(scale(Vertex(167, 142)));
    draw_curve(right_pupil, 10);
    
    vector<Vertex> left_eyebrow;
    left_eyebrow.push_back(scale(Vertex(62, 134)));
    left_eyebrow.push_back(scale(Vertex(70, 125)));
    left_eyebrow.push_back(scale(Vertex(81, 121)));
    left_eyebrow.push_back(scale(Vertex(95, 121)));
    left_eyebrow.push_back(scale(Vertex(109, 125)));
    left_eyebrow.push_back(scale(Vertex(120, 126)));
    left_eyebrow.push_back(scale(Vertex(112, 117)));
    left_eyebrow.push_back(scale(Vertex(99, 113)));
    left_eyebrow.push_back(scale(Vertex(85, 113)));
    left_eyebrow.push_back(scale(Vertex(74, 117)));
    left_eyebrow.push_back(scale(Vertex(64, 125)));
    left_eyebrow.push_back(scale(Vertex(62, 134)));
    draw_curve(left_eyebrow, 10);
    
    vector<Vertex> right_eyebrow;
    right_eyebrow.push_back(scale(Vertex(202, 133)));
    right_eyebrow.push_back(scale(Vertex(195, 123)));
    right_eyebrow.push_back(scale(Vertex(183, 118)));
    right_eyebrow.push_back(scale(Vertex(172, 118)));
    right_eyebrow.push_back(scale(Vertex(159, 121)));
    right_eyebrow.push_back(scale(Vertex(150, 127)));
    right_eyebrow.push_back(scale(Vertex(162, 127)));
    right_eyebrow.push_back(scale(Vertex(174, 126)));
    right_eyebrow.push_back(scale(Vertex(186, 127)));
    right_eyebrow.push_back(scale(Vertex(202, 133)));
    draw_curve(right_eyebrow, 10);
    
    vector<Vertex> hair;
    hair.push_back(scale(Vertex(46, 160)));
    hair.push_back(scale(Vertex(50, 134)));
    hair.push_back(scale(Vertex(55, 110)));
    hair.push_back(scale(Vertex(52, 90)));
    hair.push_back(scale(Vertex(60, 67)));
    hair.push_back(scale(Vertex(68, 46)));
    hair.push_back(scale(Vertex(83, 32)));
    hair.push_back(scale(Vertex(105, 23)));
    hair.push_back(scale(Vertex(103, 38)));
    hair.push_back(scale(Vertex(124, 37)));
    hair.push_back(scale(Vertex(144, 37)));
    hair.push_back(scale(Vertex(166, 40)));
    hair.push_back(scale(Vertex(180, 34)));
    hair.push_back(scale(Vertex(189, 52)));
    hair.push_back(scale(Vertex(199, 68)));
    hair.push_back(scale(Vertex(203, 85)));
    hair.push_back(scale(Vertex(206, 102)));
    hair.push_back(scale(Vertex(206, 122)));
    hair.push_back(scale(Vertex(208, 141)));
    hair.push_back(scale(Vertex(207, 160)));
    hair.push_back(scale(Vertex(207, 181)));
    draw_curve(hair, 10);
    
    vector<Vertex> beard;
    beard.push_back(scale(Vertex(47, 164)));
    beard.push_back(scale(Vertex(51, 182)));
    beard.push_back(scale(Vertex(54, 200)));
    beard.push_back(scale(Vertex(60, 218)));
    beard.push_back(scale(Vertex(75, 224)));
    beard.push_back(scale(Vertex(77, 199)));
    beard.push_back(scale(Vertex(96, 193)));
    beard.push_back(scale(Vertex(120, 190)));
    beard.push_back(scale(Vertex(146, 192)));
    beard.push_back(scale(Vertex(169, 195)));
    beard.push_back(scale(Vertex(187, 199)));
    beard.push_back(scale(Vertex(182, 223)));
    beard.push_back(scale(Vertex(194, 214)));
    beard.push_back(scale(Vertex(202, 197)));
    beard.push_back(scale(Vertex(207, 183)));
    draw_curve(beard, 10);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0.0f, 0.0f, 0.0f);
    draw_face();
    glutSwapBuffers();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(width, height); // Set your own window size
    glutCreateWindow("Cartoon Andrew Forney");
    setup();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}

