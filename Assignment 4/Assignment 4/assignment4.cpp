/***
 Assignment-4: Shading via Illumination and Colors
 
 Name: Moini, Donovan
 
 Collaborators: Zafiris, Serena
 ** Note: although the assignment should be completed individually
 you may speak with classmates on high level algorithmic concepts. Please
 list their names in this section
 
 In this project, I began by building a cube and constructing the individual components of the scene. For each component,
 I created a ObjectModel and added the points, normals, and base colors for each component's ObjectModel. From those
 ObjectModels, I applied shading on them and assigned the new Ir, Ig, Ib values to each ObjectModel's colors. From each
 components points and shaded colors, I built the scene.
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

GLfloat amb = 0.3;
GLfloat diff = 0.15;
GLfloat spec = 0.15;

vector<GLfloat> AMB = {amb, amb, amb};
vector<GLfloat> DIFF = {diff, diff, diff};
vector<GLfloat> SPEC = {spec, spec, spec};
vector<GLfloat> LIGHT_SOURCE = {3, 3, 2};
vector<GLfloat> CAMERA = {2.0, 3.0, 5.0};
GLfloat M = 1;

/**************************************************
 *              Object Model Class                *
 *                                                *
 *  Stores the points of the object as a vector   *
 *  along with the colors and normals for each    *
 *  point. Normals are computed from the points.  *
 *                                                *
 *************************************************/
class ObjectModel {
    vector<GLfloat> _points;
    vector<GLfloat> _normals;
    vector<GLfloat> _base_colors;
    vector<GLfloat> _colors;
public:
    ObjectModel() { };
    vector<GLfloat> get_points() { return _points; };
    vector<GLfloat> get_normals() { return _normals; };
    vector<GLfloat> get_base_colors() { return _base_colors; };
    vector<GLfloat> get_colors() { return _colors; };
    void set_points(vector<GLfloat> points) { _points = points; };
    void set_normals(vector<GLfloat> normals) { _normals = normals; };
    void set_base_colors(vector<GLfloat> base_colors) { _base_colors = base_colors; };
    void set_colors(vector<GLfloat> colors) { _colors = colors; };
};

// The model of the scene
ObjectModel SCENE;

/**************************************************
 *              Utilitie Functions                *
 *************************************************/

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

// Converts degrees to radians for rotation
float deg2rad(float d) {
    return (d * M_PI) / 180.0;
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
        1.0,    0.0,    0.0,    dx,
        0.0,    1.0,    0.0,    dy,
        0.0,    0.0,    1.0,    dz,
        0.0,    0.0,    0.0,    1.0
    };
    return translate_mat;
}

// Definition of a scaling matrix
vector<GLfloat> scaling_matrix (float sx, float sy, float sz) {
    vector<GLfloat> scale_mat = {
        sx,     0.0,    0.0,    0.0,
        0.0,    sy,     0.0,    0.0,
        0.0,    0.0,    sz,     0.0,
        0.0,    0.0,    0.0,    1.0
    };
    return scale_mat;
}

// Definition of a rotation matrix about the x-axis theta degrees
vector<GLfloat> rotation_matrix_x (float theta) {
    float radians = deg2rad(theta);
    vector<GLfloat> rotate_mat_x = {
        1.0, 0.0, 0.0, 0.0,
        0.0, (float)(cos(radians)), (float)(-sin(radians)), 0.0,
        0.0, (float)(sin(radians)), (float)(cos(radians)), 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    return rotate_mat_x;
}


// Definition of a rotation matrix about the y-axis by theta degrees
vector<GLfloat> rotation_matrix_y (float theta) {
    float radians = deg2rad(theta);
    vector<GLfloat> rotate_mat_y = {
        (float)cos(radians), 0.0, (float)sin(radians), 0.0,
        0.0, 1.0, 0.0, 0.0,
        (float)-sin(radians), 0.0, (float)cos(radians), 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    return rotate_mat_y;
}


// Definition of a rotation matrix about the z-axis by theta degrees
vector<GLfloat> rotation_matrix_z (float theta) {
    float radians = deg2rad(theta);
    vector<GLfloat> rotate_mat_z = {
        (float)cos(radians), (float)-sin(radians), 0.0, 0.0,
        (float)sin(radians), (float)cos(radians), 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    return rotate_mat_z;
}

// Perform matrix multiplication for A B
vector<GLfloat> mat_mult(vector<GLfloat> A, vector<GLfloat> B) {
    vector<GLfloat> result;
    for (int b = 0; b < B.size()/4; b++) {
        for (int a = 0; a < 4; a++) {
            float element_wise_sum = 0.0;
            for (int k = 0; k < 4;  k++) {
                float element_wise = A[a*4+k]*B[b*4+k];
                if (element_wise < EPSILON && element_wise > -1.0*EPSILON) {
                    element_wise = 0.0;
                }
                element_wise_sum += element_wise;
            }
            result.push_back(element_wise_sum);
        }
    }
    return result;
}

// Converts vector<vector<GLfloat>> to vector<GLfloat>
vector<GLfloat> squish_vector(vector<vector<GLfloat>> V) {
    vector<GLfloat> result;
    for (int i = 0; i < V.size(); i++) {
        result.insert(result.end(), V[i].begin(), V[i].end());
    }
    return result;
}

// Builds a unit cube centered at the origin
vector<GLfloat> build_cube() {
    vector<GLfloat> result;
    // Primitive plane
    vector<GLfloat> a0 = to_homogeneous_coord(init_plane());
    // Construct 6 planes of the cube
    vector<GLfloat> a1 = mat_mult(translation_matrix(0.0,  0.0,  0.5), a0);
    vector<GLfloat> a2 = mat_mult(translation_matrix(0.0,  0.0, -0.5), mat_mult(rotation_matrix_y(180), a0));
    vector<GLfloat> a3 = mat_mult(translation_matrix(-0.5, 0.0,  0.0), mat_mult(rotation_matrix_y(-90), a0));
    vector<GLfloat> a4 = mat_mult(translation_matrix(0.5,  0.0,  0.0), mat_mult(rotation_matrix_y(90), a0));
    vector<GLfloat> a5 = mat_mult(translation_matrix(0.0,  0.5,  0.0), mat_mult(rotation_matrix_x(-90), a0));
    vector<GLfloat> a6 = mat_mult(translation_matrix(0.0, -0.5,  0.0), mat_mult(rotation_matrix_x(90), a0));
    
    result.insert(std::end(result), std::begin(a1), std::end(a1));
    result.insert(std::end(result), std::begin(a2), std::end(a2));
    result.insert(std::end(result), std::begin(a3), std::end(a3));
    result.insert(std::end(result), std::begin(a4), std::end(a4));
    result.insert(std::end(result), std::begin(a5), std::end(a5));
    result.insert(std::end(result), std::begin(a6), std::end(a6));
    
    return result;
}


/**************************************************
 *           Generating Surface Normals           *
 *                                                *
 *  Generate the surface normals of the objects   *
 *  using the cross product between two vectors   *
 *  that lie on the surface (plane) of interest.  *
 *  Recall that the direction of the normal to a  *
 *  surface follows the Right Hand Rule.          *
 *                                                *
 *************************************************/

GLfloat get_magnitude(GLfloat x, GLfloat y, GLfloat z){
    return (GLfloat)sqrt((x * x) + (y * y) + (z * z));
}

vector<GLfloat> subtract_vectors(vector<GLfloat> A, vector<GLfloat> B){
    vector<GLfloat> result = {A[0] - B[0], A[1] - B[1], A[2] - B[2]};
    return result;
}

vector<GLfloat> unit_vector(vector<GLfloat> A){
    GLfloat unit_x = A[0] / get_magnitude(A[0], A[1], A[2]);
    GLfloat unit_y = A[1] / get_magnitude(A[0], A[1], A[2]);
    GLfloat unit_z = A[2] / get_magnitude(A[0], A[1], A[2]);
    return {unit_x, unit_y, unit_z};
}

// Performs the cross product between two vectors
vector<GLfloat> cross_product(vector<GLfloat> A, vector<GLfloat> B) {
    vector<GLfloat> C = {
        (A[1] * B[2]) - (A[2] * B[1]), (A[2] * B[0]) - (A[0] * B[2]), (A[0] * B[1]) - (A[1] * B[0])
    };
    return C;
}

// Generates the normals to each surface (plane)
vector<GLfloat> generate_normals(vector<GLfloat> points) {
    vector<GLfloat> normals;
    
    for (int i = 0; i < points.size(); i += 12){
        vector<GLfloat> p0 = {points[i], points[i+1], points[i+2]};
        vector<GLfloat> p1 = {points[i+3], points[i+4], points[i+5]};
        vector<GLfloat> p3 = {points[i+9], points[i+10], points[i+11]};
        
        vector<GLfloat> A = subtract_vectors(p1, p0);
        vector<GLfloat> B = subtract_vectors(p3, p0);
        
        vector<GLfloat> unit_cp = unit_vector(cross_product(A, B));
        
        for(int j = 0; j < 4; j++){
            normals.push_back(unit_cp[0]);
            normals.push_back(unit_cp[1]);
            normals.push_back(unit_cp[2]);
        }
    }
    
    return normals;
}


/**************************************************
 *       Shading via Illumination and Color       *
 *                                                *
 *  Generate the set of colors for each face of   *
 *  the planes that compose the objects in the    *
 *  scene. Based on the light source and surface  *
 *  normals, render the colors of the objects by  *
 *  applying the shading equation.                *
 *                                                *
 *************************************************/

// Performs the dot product between two vectors
GLfloat dot_product(vector<GLfloat> A, vector<GLfloat> B) {
    return (A[0] * B[0]) + (A[1] * B[1]) + (A[2] * B[2]);
}

// Initializes the base color of a plane to a single color
vector<GLfloat> init_base_color(GLfloat r, GLfloat g, GLfloat b) {
    vector<GLfloat> base_color = {
        r,   g,   b,
        r,   g,   b,
        r,   g,   b,
        r,   g,   b
    };
    return base_color;
}

// Initializes the base color of a plane by specifying the color of each point
vector<GLfloat> init_base_color(GLfloat r0, GLfloat g0, GLfloat b0, GLfloat r1, GLfloat g1, GLfloat b1,
                                GLfloat r2, GLfloat g2, GLfloat b2, GLfloat r3, GLfloat g3, GLfloat b3) {
    // This enables OpenGL to use interpolation for (Gouraud) shading the plane
    vector<GLfloat> base_color = {
        r0,   g0,   b0,
        r1,   g1,   b1,
        r2,   g2,   b2,
        r3,   g3,   b3
    };
    return base_color;
}

// Allows for ambience (a vector of 3 values), diffusion (vector of 3 values) and specular (vector of 3 values)
// as input to the shading equation
ObjectModel apply_shading(ObjectModel object_model, vector<GLfloat> light_source, vector<GLfloat> camera,
                          vector<GLfloat> amb, vector<GLfloat> diff, vector<GLfloat> spec, GLfloat m) {
    vector<GLfloat> colors;
    
    vector<GLfloat> base_colors = object_model.get_base_colors();
    vector<GLfloat> points = object_model.get_points();
    vector<GLfloat> normals = object_model.get_normals();

    for(int i = 0; i < points.size(); i += 3){
        vector<GLfloat> point = {points[i], points[i+1], points[i+2]};
        vector<GLfloat> base_color = {base_colors[i], base_colors[i+1], base_colors[i+2]};
        vector<GLfloat> normal = {normals[i], normals[i+1], normals[i+2]};
        
        vector<GLfloat> v = subtract_vectors(light_source, point);
        vector<GLfloat> h = unit_vector(subtract_vectors(light_source, v));
        GLfloat n_dot_l = dot_product(normal, light_source);
        GLfloat n_dot_h = dot_product(normal, h);
        
        GLfloat I_red = base_color[0] * (amb[0] + (diff[0] * n_dot_l)) + (spec[0] * base_color[0] * pow(n_dot_h, m));
        colors.push_back(I_red);
        
        GLfloat I_green = base_color[1] * (amb[1] + (diff[1] * n_dot_l)) + (spec[1] * base_color[1] * pow(n_dot_h, m));
        colors.push_back(I_green);
        
        GLfloat I_blue = base_color[2] * (amb[2] + (diff[2] * n_dot_l)) + (spec[2] * base_color[2] * pow(n_dot_h, m));
        colors.push_back(I_blue);
    }
    
    object_model.set_colors(colors);
    return object_model;
}

vector<GLfloat> times_six(vector<GLfloat> v) {
    return squish_vector({v, v, v, v, v, v});
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
    gluLookAt(2.0, 3.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

ObjectModel build_rug() {
    ObjectModel rug_object;
    
    vector<vector<GLfloat>> rug;
    rug.push_back(to_cartesian_coord(mat_mult(scaling_matrix(3.5, 0.1, 2), build_cube())));
    rug_object.set_points(squish_vector(rug));
    
    vector<GLfloat> points = rug_object.get_points();
    vector<GLfloat> normals = generate_normals(points);
    rug_object.set_normals(normals);
    
    vector<vector<GLfloat>> colors;
    colors.push_back(times_six(init_base_color(0.788, 0.764, 0.745)));
    rug_object.set_base_colors(squish_vector(colors));
    
    return apply_shading(rug_object, LIGHT_SOURCE, CAMERA, AMB, DIFF, SPEC, M);
}

ObjectModel build_bed() {
    ObjectModel bed_object;
    
    vector<vector<GLfloat>> bed;
    bed.push_back(to_cartesian_coord(mat_mult(translation_matrix(0, 0.5, -0.9), mat_mult(scaling_matrix(1.5, 1, 2), build_cube()))));
    bed.push_back(to_cartesian_coord(mat_mult(translation_matrix(0, 1, -2), mat_mult(scaling_matrix(1.5, 2, 0.2), build_cube()))));
    bed_object.set_points(squish_vector(bed));
    
    vector<GLfloat> points = bed_object.get_points();
    vector<GLfloat> normals = generate_normals(points);
    bed_object.set_normals(normals);
    
    vector<vector<GLfloat>> colors;
    colors.push_back(times_six(init_base_color(0.658, 0.462, 0.325)));
    colors.push_back(times_six(init_base_color(0.658, 0.462, 0.325)));
    bed_object.set_base_colors(squish_vector(colors));
    
    return apply_shading(bed_object, LIGHT_SOURCE, CAMERA, AMB, DIFF, SPEC, M);
}

ObjectModel build_bed_pillows() {
    ObjectModel bed_pillows_object;
    
    vector<vector<GLfloat>> bed_pillows;
    bed_pillows.push_back(to_cartesian_coord(mat_mult(translation_matrix(0.325, 1.25, -1.75), mat_mult(rotation_matrix_x(-20), mat_mult(scaling_matrix(0.5, 0.5, 0.1), build_cube())))));
    bed_pillows.push_back(to_cartesian_coord(mat_mult(translation_matrix(-0.325, 1.25, -1.75), mat_mult(rotation_matrix_x(-20), mat_mult(scaling_matrix(0.5, 0.5, 0.1), build_cube())))));
    bed_pillows_object.set_points(squish_vector(bed_pillows));
    
    vector<GLfloat> points = bed_pillows_object.get_points();
    vector<GLfloat> normals = generate_normals(points);
    bed_pillows_object.set_normals(normals);
    
    vector<vector<GLfloat>> colors;
    colors.push_back(times_six(init_base_color(0.909, 0.898, 0.909)));
    colors.push_back(times_six(init_base_color(0.909, 0.898, 0.909)));
    bed_pillows_object.set_base_colors(squish_vector(colors));
    
    return apply_shading(bed_pillows_object, LIGHT_SOURCE, CAMERA, AMB, DIFF, SPEC, M);
}

ObjectModel build_floor_pillow() {
    ObjectModel floor_pillow_object;
    
    vector<vector<GLfloat>> floor_pillow;
    floor_pillow.push_back(to_cartesian_coord(mat_mult(translation_matrix(1.1, 0.05, 0.3), mat_mult(rotation_matrix_y(20), mat_mult(scaling_matrix(0.5, 0.1, 0.5), build_cube())))));
    floor_pillow_object.set_points(squish_vector(floor_pillow));
    
    vector<GLfloat> points = floor_pillow_object.get_points();
    vector<GLfloat> normals = generate_normals(points);
    floor_pillow_object.set_normals(normals);
    
    vector<vector<GLfloat>> colors;
    colors.push_back(times_six(init_base_color(0.909, 0.898, 0.909)));
    floor_pillow_object.set_base_colors(squish_vector(colors));
    
    return apply_shading(floor_pillow_object, LIGHT_SOURCE, CAMERA, AMB, DIFF, SPEC, M);
}

ObjectModel build_cabinet() {
    ObjectModel cabinet_object;
    
    vector<vector<GLfloat>> cabinet;
    cabinet.push_back(to_cartesian_coord(mat_mult(translation_matrix(-1.6, 0.875, -1.4), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.5, 1.4, 1) , build_cube())))));
    cabinet_object.set_points(squish_vector(cabinet));
    
    vector<GLfloat> points = cabinet_object.get_points();
    vector<GLfloat> normals = generate_normals(points);
    cabinet_object.set_normals(normals);
    
    vector<vector<GLfloat>> colors;
    colors.push_back(times_six(init_base_color(0.749, 0.698, 0.631)));
    cabinet_object.set_base_colors(squish_vector(colors));
    
    return apply_shading(cabinet_object, LIGHT_SOURCE, CAMERA, AMB, DIFF, SPEC, M);
}

ObjectModel build_cabinet_plant() {
    ObjectModel cabinet_plant_object;
    
    float rotation_angle = 12.5;
    vector<vector<GLfloat>> cabinet_plant;
    cabinet_plant.push_back(to_cartesian_coord(mat_mult(translation_matrix(-1.35, 1.8, -1.5), mat_mult(scaling_matrix(0.18, 0.4, 0.18) , build_cube()))));
    cabinet_plant.push_back(to_cartesian_coord(mat_mult(translation_matrix(-1.30, 2.1, -1.55), mat_mult(rotation_matrix_x(-rotation_angle), mat_mult(rotation_matrix_z(-rotation_angle), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.04, 0.3, 0.04), build_cube())))))));
    cabinet_plant.push_back(to_cartesian_coord(mat_mult(translation_matrix(-1.30, 2.1, -1.45), mat_mult(rotation_matrix_x(rotation_angle), mat_mult(rotation_matrix_z(-rotation_angle), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.04, 0.3, 0.04) , build_cube())))))));
    cabinet_plant.push_back(to_cartesian_coord(mat_mult(translation_matrix(-1.40, 2.1, -1.45), mat_mult(rotation_matrix_x(rotation_angle), mat_mult(rotation_matrix_z(rotation_angle), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.04, 0.3, 0.04) , build_cube())))))));
    cabinet_plant.push_back(to_cartesian_coord(mat_mult(translation_matrix(-1.40, 2.1, -1.55), mat_mult(rotation_matrix_x(-rotation_angle), mat_mult(rotation_matrix_z(rotation_angle), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.04, 0.3, 0.04) , build_cube())))))));
    cabinet_plant_object.set_points(squish_vector(cabinet_plant));
    
    vector<GLfloat> points = cabinet_plant_object.get_points();
    vector<GLfloat> normals = generate_normals(points);
    cabinet_plant_object.set_normals(normals);
    
    vector<vector<GLfloat>> colors;
    colors.push_back(times_six(init_base_color(0.643, 0.631, 0.662)));
    colors.push_back(times_six(init_base_color(0.098, 0.8, 0.211)));
    colors.push_back(times_six(init_base_color(0.098, 0.8, 0.211)));
    colors.push_back(times_six(init_base_color(0.098, 0.8, 0.211)));
    colors.push_back(times_six(init_base_color(0.098, 0.8, 0.211)));
    cabinet_plant_object.set_base_colors(squish_vector(colors));
    
    return apply_shading(cabinet_plant_object, LIGHT_SOURCE, CAMERA, AMB, DIFF, SPEC, M);
}

ObjectModel build_floor_pile() {
    ObjectModel floor_pile_object;
    
    vector<vector<GLfloat>> floor_pile;
    floor_pile.push_back(to_cartesian_coord(mat_mult(translation_matrix(1.2, 0.15, -1.4), mat_mult(scaling_matrix(0.5, 0.3, 0.5), build_cube()))));
    floor_pile_object.set_points(squish_vector(floor_pile));
    
    vector<GLfloat> points = floor_pile_object.get_points();
    vector<GLfloat> normals = generate_normals(points);
    floor_pile_object.set_normals(normals);
    
    vector<vector<GLfloat>> colors;
    colors.push_back(times_six(init_base_color(0.549, 0.533, 0.517)));
    floor_pile_object.set_base_colors(squish_vector(colors));
    
    return apply_shading(floor_pile_object, LIGHT_SOURCE, CAMERA, AMB, DIFF, SPEC, M);
}

ObjectModel build_floor_pile_plant() {
    ObjectModel floor_pile_plant_object;
    
    float rotation_angle = 12.5;
    vector<vector<GLfloat>> floor_pile_plant;
    floor_pile_plant.push_back(to_cartesian_coord(mat_mult(translation_matrix(1.2, 0.5, -1.4), mat_mult(scaling_matrix(0.18, 0.4, 0.18) , build_cube()))));
    floor_pile_plant.push_back(to_cartesian_coord(mat_mult(translation_matrix(1.15, 0.8, -1.45), mat_mult(rotation_matrix_x(-rotation_angle), mat_mult(rotation_matrix_z(rotation_angle), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.04, 0.3, 0.04), build_cube())))))));
    floor_pile_plant.push_back(to_cartesian_coord(mat_mult(translation_matrix(1.15, 0.8, -1.35), mat_mult(rotation_matrix_x(rotation_angle), mat_mult(rotation_matrix_z(rotation_angle), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.04, 0.3, 0.04) , build_cube())))))));
    floor_pile_plant.push_back(to_cartesian_coord(mat_mult(translation_matrix(1.25, 0.8, -1.35), mat_mult(rotation_matrix_x(rotation_angle), mat_mult(rotation_matrix_z(-rotation_angle), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.04, 0.3, 0.04) , build_cube())))))));
    floor_pile_plant.push_back(to_cartesian_coord(mat_mult(translation_matrix(1.25, 0.8, -1.45), mat_mult(rotation_matrix_x(-rotation_angle), mat_mult(rotation_matrix_z(-rotation_angle), mat_mult(rotation_matrix_y(-70), mat_mult(scaling_matrix(0.04, 0.3, 0.04) , build_cube())))))));
    floor_pile_plant_object.set_points(squish_vector(floor_pile_plant));
    
    vector<GLfloat> points = floor_pile_plant_object.get_points();
    vector<GLfloat> normals = generate_normals(points);
    floor_pile_plant_object.set_normals(normals);
    
    vector<vector<GLfloat>> colors;
    colors.push_back(times_six(init_base_color(0.643, 0.631, 0.662)));
    colors.push_back(times_six(init_base_color(0.098, 0.8, 0.211)));
    colors.push_back(times_six(init_base_color(0.098, 0.8, 0.211)));
    colors.push_back(times_six(init_base_color(0.098, 0.8, 0.211)));
    colors.push_back(times_six(init_base_color(0.098, 0.8, 0.211)));
    floor_pile_plant_object.set_base_colors(squish_vector(colors));
    
    return apply_shading(floor_pile_plant_object, LIGHT_SOURCE, CAMERA, AMB, DIFF, SPEC, M);
}

// Construct the scene using objects built from cubes/prisms
vector<GLfloat> init_scene() {
    vector<vector<GLfloat>> scene_points;
    
    ObjectModel rug = build_rug();
    vector<GLfloat> rug_points = rug.get_points();
    scene_points.push_back(rug_points);

    ObjectModel bed = build_bed();
    vector<GLfloat> bed_points = bed.get_points();
    scene_points.push_back(bed_points);

    ObjectModel bed_pillows = build_bed_pillows();
    vector<GLfloat> bed_pillows_points = bed_pillows.get_points();
    scene_points.push_back(bed_pillows_points);

    ObjectModel floor_pillow = build_floor_pillow();
    vector<GLfloat> floor_pillow_points = floor_pillow.get_points();
    scene_points.push_back(floor_pillow_points);

    ObjectModel cabinet = build_cabinet();
    vector<GLfloat> cabinet_points = cabinet.get_points();
    scene_points.push_back(cabinet_points);

    ObjectModel cabinet_plant = build_cabinet_plant();
    vector<GLfloat> cabinet_plant_points = cabinet_plant.get_points();
    scene_points.push_back(cabinet_plant_points);

    ObjectModel floor_pile = build_floor_pile();
    vector<GLfloat> floor_pile_points = floor_pile.get_points();
    scene_points.push_back(floor_pile_points);

    ObjectModel floor_pile_plant = build_floor_pile_plant();
    vector<GLfloat> floor_pile_plant_points = floor_pile_plant.get_points();
    scene_points.push_back(floor_pile_plant_points);
    
    return squish_vector(scene_points);
}

// Construct the color mapping of the scene
vector<GLfloat> init_color() {
    vector<vector<GLfloat>> scene_colors;
    
    ObjectModel rug = build_rug();
    vector<GLfloat> rug_colors = rug.get_colors();
    scene_colors.push_back(rug_colors);

    ObjectModel bed = build_bed();
    vector<GLfloat> bed_color = bed.get_colors();
    scene_colors.push_back(bed_color);

    ObjectModel bed_pillows = build_bed_pillows();
    vector<GLfloat> bed_pillows_colors = bed_pillows.get_colors();
    scene_colors.push_back(bed_pillows_colors);

    ObjectModel floor_pillow = build_floor_pillow();
    vector<GLfloat> floor_pillow_colors = floor_pillow.get_colors();
    scene_colors.push_back(floor_pillow_colors);

    ObjectModel cabinet = build_cabinet();
    vector<GLfloat> cabinet_colors = cabinet.get_colors();
    scene_colors.push_back(cabinet_colors);

    ObjectModel cabinet_plant = build_cabinet_plant();
    vector<GLfloat> cabinet_plant_colors = cabinet_plant.get_colors();
    scene_colors.push_back(cabinet_plant_colors);

    ObjectModel floor_pile = build_floor_pile();
    vector<GLfloat> floor_pile_colors = floor_pile.get_colors();
    scene_colors.push_back(floor_pile_colors);

    ObjectModel floor_pile_plant = build_floor_pile_plant();
    vector<GLfloat> floor_pile_plant_colors = floor_pile_plant.get_colors();
    scene_colors.push_back(floor_pile_plant_colors);
    
    return squish_vector(scene_colors);
}


void display_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Initialize your scene at every iteration
    SCENE.set_points(init_scene());
    SCENE.set_colors(init_color());
    
    // Rotate the scene using the rotation matrix
    SCENE.set_points(to_cartesian_coord(mat_mult(rotation_matrix_y(THETA), to_homogeneous_coord(SCENE.get_points()))));
    
    GLfloat* scene_vertices = vector2array(SCENE.get_points());
    GLfloat* color_vertices = vector2array(SCENE.get_colors());
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
    
    // Draw quad point planes: each 4 vertices with 3 dimensions
    glDrawArrays(GL_QUADS, 0, (int)SCENE.get_points().size() / 3);
    
    glFlush();            //Finish rendering
    glutSwapBuffers();
    
    // Clean up
    delete scene_vertices;
    delete color_vertices;
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
    glutCreateWindow("Assignment 4");
    
    setup();
    init_camera();
    
    SCENE.set_points(init_scene());
    SCENE.set_base_colors(init_color());
    
    // Set up our display function
    glutDisplayFunc(display_func);
    glutIdleFunc(idle_func);
    // Render our world
    glutMainLoop();
    
    return 0;
}
