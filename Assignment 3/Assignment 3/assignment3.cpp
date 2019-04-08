/***
 Assignment-3: Geometric Modeling of a Scene
 
 Name: Moini, Donovan
 
 Collaborators: Doe, John; Doe, Jane
 ** Note: although the assignment should be completed individually
 you may speak with classmates on high level algorithmic concepts. Please
 list their names in this section
 
 Project Summary: A short paragraph (3-4 sentences) describing the work you
 did for the project.
 ***/


#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#pragma GCC diagnostic pop

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
using namespace std;

// If a float is < EPSILON or > -EPILSON then it should be 0
float EPSILON = 0.000001;
// theta is the angle to rotate the scene
float THETA = 0.0;
// Vector placeholders for the scene and color array
vector<GLfloat> SCENE;
vector<GLfloat> COLOR;
int PLANES;

/**************************************************
 *  Rectangular Prisms via Hierarchical Modeling  *
 *                                                *
 *  using planes as building blocks, build a unit *
 *  cube with transformations that will serve as  *
 *  a primitive for modeling objects in the scene *
 *                                                *
 *************************************************/

// Converts degrees to radians for rotation
float deg2rad(float d) {
    return (d * M_PI) / 180.0;
}

// Initializes a square plane of unit lengths
vector<GLfloat> init_plane() {
    vector<GLfloat> vertices = {
        +0.5,   +0.5,   +0.0,
        -0.5,   +0.5,   +0.0,
        -0.5,   -0.5,   +0.0,
        +0.5,   -0.5,   +0.0
    };
    return vertices;
}

// Converts a vector to an array
GLfloat* vector2array(vector<GLfloat> vec) {
    GLfloat* arr = new GLfloat[vec.size()];
    for (int i = 0; i < vec.size(); i++) {
        arr[i] = vec[i];
    }
    return arr;
}

// Converts Cartesian coordinates to homogeneous coordinates
vector<GLfloat> to_homogeneous_coord(vector<GLfloat> cartesian_coords) {
    vector<GLfloat> result;
    for (int i = 0; i < cartesian_coords.size(); i++) {
        result.push_back(cartesian_coords[i]);
        if ((i + 1) % 3 == 0) {
            result.push_back(1.0);
        }
    }
    return result;
}

// Converts Cartesian coordinates to homogeneous coordinates
vector<GLfloat> to_cartesian_coord(vector<GLfloat> homogeneous_coords) {
    vector<GLfloat> result;
    for (int i = 0; i < homogeneous_coords.size(); i++) {
        if ((i + 1) % 4 == 0) {
            continue;
        } else {
            result.push_back(homogeneous_coords[i]);
        }
    }
    return result;
}

// Definition of a translation matrix
vector<GLfloat> translation_matrix (float dx, float dy, float dz) {
    vector<GLfloat> translate_mat = {
        1.0, 0.0, 0.0, dx,
        0.0, 1.0, 0.0, dy,
        0.0, 0.0, 1.0, dz,
        0.0, 0.0, 0.0, 1.0
    };
    return translate_mat;
}

// Definition of a scaling matrix
vector<GLfloat> scaling_matrix (float sx, float sy, float sz) {
    vector<GLfloat> scale_mat = {
        sx, 0.0, 0.0, 0.0,
        0.0, sy, 0.0, 0.0,
        0.0, 0.0, sz, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    return scale_mat;
}

// Definition of a rotation matrix about the x-axis theta degrees
vector<GLfloat> rotation_matrix_x (float theta) {
    float radian = deg2rad(theta);
    vector<GLfloat> rotate_mat_x = {
        1.0, 0.0, 0.0, 0.0,
        0.0, cos(radian), -sin(radian), 0.0,
        0.0, sin(radian), cos(radian), 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    return rotate_mat_x;
}


// Definition of a rotation matrix about the y-axis by theta degrees
vector<GLfloat> rotation_matrix_y (float theta) {
    float radian = deg2rad(theta);
    vector<GLfloat> rotate_mat_y = {
        cos(radian), 0.0, sin(radian), 0.0,
        0.0, 1.0, 0.0, 0.0,
        -sin(radian), 0.0, cos(radian), 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    return rotate_mat_y;
}


// Definition of a rotation matrix about the z-axis by theta degrees
vector<GLfloat> rotation_matrix_z (float theta) {
    float radian = deg2rad(theta);
    vector<GLfloat> rotate_mat_z = {
        cos(radian), -sin(radian), 0.0, 0.0,
        sin(radian), cos(radian), 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    return rotate_mat_z;
}

// Perform matrix multiplication for A (4x4 translation, scaling, rotation, etc...) and B(4xN coordinates)
// Coordinates in B are stored as:
//      [A1 A2 A3 A4]
//      [B1 B2 B3 B4]
//      ...
//      [Z1 Z2 Z3 Z4]
vector<GLfloat> mat_mult(vector<GLfloat> A, vector<GLfloat> B) {
    vector<GLfloat> result;
    for (int b = 0; b < B.size() / 4; b++) {
        for (int a = 0; a < 4; a++) {
            float res = 0;
            for (int k = 0; k < 4; k++) {
                float product = A[a * 4 + k] * B[b * 4 + k];
                if (-EPSILON < product and product < EPSILON) {
                    product = 0;
                }
                res += product;
            }
            result.push_back(res);
        }
    }
    return result;
}

// Converts vector<vector<GLfloat>> to vector<GLfloat>
vector<GLfloat> squish_vector(vector<vector<GLfloat>> v) {
    vector<GLfloat> result;
    for (int i = 0; i < v.size(); i++) {
        result.insert(result.end(), v[i].begin(), v[i].end());
    }
    return result;
}

// Builds a unit cube centered at the origin
vector<GLfloat> build_cube() {
    vector<GLfloat> a0 = to_homogeneous_coord(init_plane());
    vector<GLfloat> front = mat_mult(translation_matrix(0, 0, 0.5), a0);
    vector<GLfloat> back = mat_mult(translation_matrix(0, 0, -0.5), mat_mult(rotation_matrix_y(180), a0));
    vector<GLfloat> left = mat_mult(translation_matrix(0.5, 0, 0), mat_mult(rotation_matrix_y(-90), a0));
    vector<GLfloat> right = mat_mult(translation_matrix(-0.5, 0, 0), mat_mult(rotation_matrix_y(90), a0));
    vector<GLfloat> top = mat_mult(translation_matrix(0, 0.5, 0), mat_mult(rotation_matrix_x(-90), a0));
    vector<GLfloat> bottom = mat_mult(translation_matrix(0, -0.5, 0), mat_mult(rotation_matrix_x(90), a0));
    
    vector<vector<GLfloat>> scene = {front, back, left, right, top, bottom};
    return squish_vector(scene);
}

void print_matrix(vector<GLfloat> A) {
    cout << "[ ";
    for (int i = 0; i < A.size(); i++) {
        if (i % 4 == 0 && i != 0) {
            cout << " ]\n[ ";
        }
        cout << A[i];
        if (i % 4 != 3) {
            cout << " ";
        }
    }
    cout << " ]" << "\n";
}

/**************************************************
 *            Camera and World Modeling           *
 *                                                *
 *  create a scene by applying transformations to *
 *  the objects built from planes and position    *
 *  the camera to view the scene by setting       *
 *  the projection/viewing matrices               *
 *                                                *
 *************************************************/

void setup() {
    // Enable the vertex array functionality
    glEnableClientState(GL_VERTEX_ARRAY);
    // Enable the color array functionality (so we can specify a color for each vertex)
    glEnableClientState(GL_COLOR_ARRAY);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // Set up some default base color
    glColor3f(0.5, 0.5, 0.5);
    // Set up white background
    glClearColor(1.0, 1.0, 1.0, 0.0);
}

void init_camera() {
    // Camera parameters
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a 50 degree field of view, 1:1 aspect ratio, near and far planes at 3 and 7
    gluPerspective(50.0, 1.0, 2.0, 10.0);
    // Position camera at (2, 3, 5), attention at (0, 0, 0), up at (0, 1, 0)
    gluLookAt(2.0, 6.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    
}

vector<GLfloat> build_rug() {
    vector<GLfloat> rug = mat_mult(scaling_matrix(3.5, 0.1, 2), build_cube());
    vector<vector<GLfloat>> result = {rug};
    return squish_vector(result);
}

vector<GLfloat> build_bed() {
    vector<GLfloat> bed_body = mat_mult(translation_matrix(0, 0.5, -0.9), mat_mult(scaling_matrix(1.5, 1, 2), build_cube()));
    vector<GLfloat> bed_frame = mat_mult(translation_matrix(0, 1, -2), mat_mult(scaling_matrix(1.5, 2, 0.2), build_cube()));
    vector<vector<GLfloat>> result = {bed_body, bed_frame};
    return squish_vector(result);
}

vector<GLfloat> build_bed_pillows() {
    vector<GLfloat> left_pillow = mat_mult(translation_matrix(0.325, 1.25, -1.75), mat_mult(rotation_matrix_x(-20), mat_mult(scaling_matrix(0.5, 0.5, 0.1), build_cube())));
    vector<GLfloat> right_pillow = mat_mult(translation_matrix(-0.325, 1.25, -1.75), mat_mult(rotation_matrix_x(-20), mat_mult(scaling_matrix(0.5, 0.5, 0.1), build_cube())));
    vector<vector<GLfloat>> result = {left_pillow, right_pillow};
    return squish_vector(result);
}

vector<GLfloat> build_floor_pillow() {
    vector<GLfloat> floor_pillow = mat_mult(translation_matrix(1.1, 0.05, 0.3), mat_mult(rotation_matrix_y(20), mat_mult(scaling_matrix(0.5, 0.1, 0.5), build_cube())));
    vector<vector<GLfloat>> result = {floor_pillow};
    return squish_vector(result);
}

vector<GLfloat> build_cabinet() {
    vector<GLfloat> cabinet = mat_mult(translation_matrix(-1.6, 0.875, -1.4), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.5, 1.4, 1) , build_cube())));
    vector<vector<GLfloat>> result = {cabinet};
    return squish_vector(result);
}

vector<GLfloat> build_cabinet_plant() {
    float rotation_angle = 12.5;
    vector<GLfloat> plant_vase = mat_mult(translation_matrix(-1.35, 1.8, -1.5), mat_mult(scaling_matrix(0.18, 0.4, 0.18) , build_cube()));
    vector<GLfloat> plant_stem1 = mat_mult(translation_matrix(-1.30, 2.1, -1.55), mat_mult(rotation_matrix_x(-rotation_angle), mat_mult(rotation_matrix_z(-rotation_angle), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.04, 0.3, 0.04), build_cube())))));
    vector<GLfloat> plant_stem2 = mat_mult(translation_matrix(-1.30, 2.1, -1.45), mat_mult(rotation_matrix_x(rotation_angle), mat_mult(rotation_matrix_z(-rotation_angle), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.04, 0.3, 0.04) , build_cube())))));
    vector<GLfloat> plant_stem3 = mat_mult(translation_matrix(-1.40, 2.1, -1.45), mat_mult(rotation_matrix_x(rotation_angle), mat_mult(rotation_matrix_z(rotation_angle), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.04, 0.3, 0.04) , build_cube())))));
    vector<GLfloat> plant_stem4 = mat_mult(translation_matrix(-1.40, 2.1, -1.55), mat_mult(rotation_matrix_x(-rotation_angle), mat_mult(rotation_matrix_z(rotation_angle), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.04, 0.3, 0.04) , build_cube())))));
    vector<vector<GLfloat>> plant = {plant_vase, plant_stem1, plant_stem2, plant_stem3, plant_stem4};
    return squish_vector(plant);
}

vector<GLfloat> build_floor_pile() {
    vector<GLfloat> floor_pile = mat_mult(translation_matrix(1.2, 0.15, -1.4), mat_mult(scaling_matrix(0.5, 0.3, 0.5), build_cube()));
    vector<vector<GLfloat>> result = {floor_pile};
    return squish_vector(result);
}

vector<GLfloat> build_floor_pile_plant() {
    float rotation_angle = 12.5;
    vector<GLfloat> plant_vase = mat_mult(translation_matrix(1.2, 0.5, -1.4), mat_mult(scaling_matrix(0.18, 0.4, 0.18) , build_cube()));
    vector<GLfloat> plant_stem1 = mat_mult(translation_matrix(1.15, 0.8, -1.45), mat_mult(rotation_matrix_x(-rotation_angle), mat_mult(rotation_matrix_z(rotation_angle), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.04, 0.3, 0.04), build_cube())))));
    vector<GLfloat> plant_stem2 = mat_mult(translation_matrix(1.15, 0.8, -1.35), mat_mult(rotation_matrix_x(rotation_angle), mat_mult(rotation_matrix_z(rotation_angle), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.04, 0.3, 0.04) , build_cube())))));
    vector<GLfloat> plant_stem3 = mat_mult(translation_matrix(1.25, 0.8, -1.35), mat_mult(rotation_matrix_x(rotation_angle), mat_mult(rotation_matrix_z(-rotation_angle), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.04, 0.3, 0.04) , build_cube())))));
    vector<GLfloat> plant_stem4 = mat_mult(translation_matrix(1.25, 0.8, -1.45), mat_mult(rotation_matrix_x(-rotation_angle), mat_mult(rotation_matrix_z(-rotation_angle), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.04, 0.3, 0.04) , build_cube())))));
    vector<vector<GLfloat>> plant = {plant_vase, plant_stem1, plant_stem2, plant_stem3, plant_stem4};
    return squish_vector(plant);
}

// Construct the scene using objects built from cubes/prisms
vector<GLfloat> init_scene() {
    vector<vector<GLfloat>> unsquished_scene = {build_rug(), build_bed(), build_bed_pillows(), build_floor_pillow(), build_cabinet(), build_cabinet_plant(), build_floor_pile(), build_floor_pile_plant()};
    vector<GLfloat> scene = squish_vector(unsquished_scene);
    PLANES = (int)scene.size() / 4;
    return scene;
}

// Construct the color mapping of the scene
vector<GLfloat> init_color(vector<GLfloat> scene) {
    vector<GLfloat> colors;
    for (int i = 0; i < scene.size(); i++) {
        colors.push_back(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    }
    return colors;
}

void display_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    SCENE = init_scene();
    SCENE = mat_mult(rotation_matrix_y(THETA), SCENE);
    SCENE = to_cartesian_coord(SCENE);
    
    
    GLfloat* scene_vertices = vector2array(SCENE);
    GLfloat* color_vertices = vector2array(COLOR);
    // Pass the scene vertex pointer
    glVertexPointer(3,                // 3 components (x, y, z)
                    GL_FLOAT,         // Vertex type is GL_FLOAT
                    0,                // Start position in referenced memory
                    scene_vertices);  // Pointer to memory location to read from
    
    // Pass the color vertex pointer
    glColorPointer(3,                   // 3 components (r, g, b)
                   GL_FLOAT,            // Vertex type is GL_FLOAT
                   0,                   // Start position in referenced memory
                   color_vertices);     // Pointer to memory location to read from
    
    // Draw quad point planes: each 4 vertices
    glDrawArrays(GL_QUADS, 0, PLANES);
    
    glFlush();            //Finish rendering
    glutSwapBuffers();
    
    free(scene_vertices);
    free(color_vertices);
}

void idle_func() {
    THETA += 0.3;
    display_func();
}

int main (int argc, char **argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    // Create a window with rendering context and everything else we need
    glutCreateWindow("Assignment 3");

    setup();
    init_camera();
    // Setting global variables SCENE and COLOR with actual values
    SCENE = init_scene();
    COLOR = init_color(SCENE);

    // Set up our display function
    glutDisplayFunc(display_func);
    glutIdleFunc(idle_func);
    // Render our world
    glutMainLoop();

    // Remember to call "delete" on your dynmically allocated arrays
    // such that you don't suffer from memory leaks. e.g.
    // delete arr;
    
    return 0;
}
