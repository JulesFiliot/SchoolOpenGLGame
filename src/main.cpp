/*****************************************************************************\
 * TP CPE, 4ETI, TP synthese d'images
 * --------------
 *
 * Programme principal des appels OpenGL
 \*****************************************************************************/

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include<string>

#define GLEW_STATIC 1
#include <GL/glew.h>

#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#define __gl_h_
#include <GLUT/glut.h>
#define MACOSX_COMPATIBILITY GLUT_3_2_CORE_PROFILE
#else
#include <GL/glut.h>
#define MACOSX_COMPATIBILITY 0
#endif

#include "glhelper.hpp"
#include "mat4.hpp"
#include "vec3.hpp"
#include "vec2.hpp"
#include "triangle_index.hpp"
#include "vertex_opengl.hpp"
#include "image.hpp"
#include "mesh.hpp"

/*****************************************************************************\
 * Variables globales
 *
 *
 \*****************************************************************************/
//variables drawn_enemy_squad
int test_draw_1 = 1;
int test_draw_2 = 1;
int test_draw_3 = 1;
int test_draw_4 = 1;
int test_draw_5 = 1;
int test_draw_6 = 1;
int test_draw_7 = 1;
int test_draw_8 = 1;
int reset_parameter = 0;
//variable pour tester si on draw ou pas le player
int test_draw_player = 1;
int test_enemy_collision = 0;
//score du joueur
int score = 0;
//compteur activation des tirs enemies
int cpt_tirs_enemies = 0;

//Variables gameplay
volatile int tir=0;
volatile int tir1=0;
volatile int tir2=0;
volatile int tir3=0;

volatile int tirEnnemi1=0;
volatile int tirEnnemi2=0;
volatile int tirEnnemi3=0;
volatile int tirEnnemi4=0;

volatile int deplacementHaut=0;
volatile int deplacementBas=0;
volatile int deplacementGauche=0;
volatile int deplacementDroite=0;

//variables fonction move_enemy_squad()
int cpt_boucle = 1;
float move_forward = 0.3f;
float move_side = 0.05f;
float move_rotation = 0.8f;
int init_parameter = 1;

//identifiants pour object 3
GLuint vao_object_3=0;
GLuint vbo_object_3=0;
GLuint vboi_object_3=0;
GLuint texture_id_object_3=0;
int nbr_triangle_object_3;

//identifiants pour object 4
GLuint vao_object_4=0;
GLuint vbo_object_4=0;
GLuint vboi_object_4=0;
GLuint texture_id_object_4=0;
int nbr_triangle_object_4;
//identifiant du shader
GLuint shader_program_id;

//
GLuint gui_program_id;

// Text drawing Structure
struct text{
  GLuint vao = 0;              // Vertex array id
  GLuint texture_id = 0;       // Well, texture id...
  std::string value;           // Value of the text to display
  vec2 bottomLeft;
  vec2 topRight;
};
text text_to_draw;

//Matrice de transformation
struct transformation
{
  mat4 rotation;
  vec3 rotation_center;
  vec3 translation;

  transformation():rotation(),rotation_center(),translation(){}
};

//Informations correspondants à un model
struct model
{
  GLuint vao = 0;
  GLuint texture_id=0;
  int nbr_triangle;
  transformation transformation_model;
  vec3 angle = vec3(0.0f,0.0f,0.0f); // angle suivant x, y et z
  // rotation autour de y non implémentée
};

//Liste des modèls
model model_dinosaure;
model model_sol1;
model model_sol2;
model model_monstre;
model model_balle1;
model model_balle2;
model model_balle3;
model model_balleEnnemie1;
model model_balleEnnemie2;
model model_balleEnnemie3;
model model_balleEnnemie4;

model model_armadillo;
model model_armadillo2;
model model_armadillo3;
model model_armadillo4;
model model_armadillo5;
model model_armadillo6;
model model_armadillo7;
model model_armadillo8;

//Transformation de la vue (camera)
transformation transformation_view;
float angle_view = 0.0f;

//Matrice de projection
mat4 projection;

void load_texture(const char* filename,GLuint *texture_id,GLuint program_id);

void init_model_1();
void init_sol1();
void init_sol2();
void init_testArmadillo();
void init_model_balle1();
void init_model_balle2();
void init_model_balle3();
void init_model_balleEnnemie1();
void init_model_balleEnnemie2();
void init_model_balleEnnemie3();
void init_model_balleEnnemie4();

void init_model_3();
void init_model_4();
void init_model_5();
void init_model_6();
void init_model_7();
void init_model_8();
void init_model_9();
void init_model_10();
void init_enemy_squad();
void draw_enemy_squad();
void move_enemy_squad();

void player_collision_detection();
void bullet_collision_detection();
void enemy_bullet_collision();

void draw_model(model model_id);
void gestionTexte();
void init_text(text *t);
void draw_texts(text *text_t);

void gestionTirs();
void gestionDeplacement();
void gestionSol();
void gestionTirsEnnemis();

void reset_wave();
void game_over();

static void init()
{

  // Chargement du shader
  shader_program_id = glhelper::create_program_from_file(
      "shaders/shader.vert",
      "shaders/shader.frag"); CHECK_GL_ERROR();
  glUseProgram(shader_program_id);

  //matrice de projection
  projection = matrice_projection(60.0f*M_PI/180.0f,1.0f,0.01f,100.0f);
  GLint loc_projection = glGetUniformLocation(shader_program_id, "projection"); CHECK_GL_ERROR();
  if (loc_projection == -1) std::cerr << "Pas de variable uniforme : projection" << std::endl;
  glUniformMatrix4fv(loc_projection,1,false,pointeur(projection)); CHECK_GL_ERROR();

  //centre de rotation de la 'camera' (les objets sont centres en z=-2)
  transformation_view.rotation_center = vec3(0.0f,0.0f,-2.0f);

  //activation de la gestion de la profondeur
  glEnable(GL_DEPTH_TEST); CHECK_GL_ERROR();

  // Charge modele 1 sur la carte graphique
  init_model_1();
  // Charge modele 2 sur la carte graphique
  init_sol1();
  init_sol2();
  // Charge modèle 3
  init_testArmadillo();

  //Charge les modèles des balles
  init_model_balle1();
  init_model_balle2();
  init_model_balle3();
  init_model_balleEnnemie1();
  init_model_balleEnnemie2();
  init_model_balleEnnemie3();
  init_model_balleEnnemie4();

  init_enemy_squad();

  transformation_view.translation.z -= 1.0f;

//
  gui_program_id = glhelper::create_program_from_file(
      "shaders/shadertext.vert",
      "shaders/shadertext.frag"); CHECK_GL_ERROR();
  glUseProgram(gui_program_id);

  /*text_to_draw.value='EFG';
  text_to_draw.bottomLeft=vec2(0.0,0.0);
  text_to_draw.topRight=vec2(1.0,1.0);
  init_text(&text_to_draw);*/
  text_to_draw.value = "score:";
  text_to_draw.bottomLeft = vec2(-0.4, 0.8);
  text_to_draw.topRight = vec2(0.4, 2);
  //text_to_draw.bottomLeft = vec2(-1.0,0.8);
  //text_to_draw.topRight = vec2(0.0,2.0);
  init_text(&text_to_draw);
  
  

}

//Fonction d'affichage
static void display_callback()
{
  //effacement des couleurs du fond d'ecran
  glClearColor(0.5f, 0.6f, 0.9f, 1.0f); //CHECK_GL_ERROR();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //CHECK_GL_ERROR();


  //définie le programme à utiliser
  glUseProgram(shader_program_id);



  // Affecte les parametres uniformes de la vue (identique pour tous les modeles de la scene)
  {

    //envoie de la rotation
    GLint loc_rotation_view = glGetUniformLocation(shader_program_id, "rotation_view"); //CHECK_GL_ERROR();
    if (loc_rotation_view == -1) std::cerr << "Pas de variable uniforme : rotation_view" << std::endl;
    glUniformMatrix4fv(loc_rotation_view,1,false,pointeur(transformation_view.rotation));// CHECK_GL_ERROR();

    //envoie du centre de rotation
    vec3 cv = transformation_view.rotation_center;
    GLint loc_rotation_center_view = glGetUniformLocation(shader_program_id, "rotation_center_view"); //CHECK_GL_ERROR();
    if (loc_rotation_center_view == -1) std::cerr << "Pas de variable uniforme : rotation_center_view" << std::endl;
    glUniform4f(loc_rotation_center_view , cv.x,cv.y,cv.z , 0.0f);// CHECK_GL_ERROR();

    //envoie de la translation
    vec3 tv = transformation_view.translation;
    GLint loc_translation_view = glGetUniformLocation(shader_program_id, "translation_view");// CHECK_GL_ERROR();
    if (loc_translation_view == -1) std::cerr << "Pas de variable uniforme : translation_view" << std::endl;
    glUniform4f(loc_translation_view , tv.x,tv.y,tv.z , 0.0f); //CHECK_GL_ERROR();
  }

  if (test_enemy_collision == 1) {
    // Affiche le modele dinosaure
    if (test_draw_player == 1) {draw_model(model_dinosaure);}
    // Affiche le modele sol
    draw_model(model_sol1);
    draw_model(model_sol2);
    draw_enemy_squad();
    
    move_enemy_squad();

    player_collision_detection();
    bullet_collision_detection();
    if(cpt_tirs_enemies > 150) {enemy_bullet_collision();}

    //affiche tir
    if(cpt_tirs_enemies > 100) {
      gestionTirs();
      gestionTirsEnnemis();
    }
    gestionDeplacement();
    gestionSol();

    //fonction qui reset la vague d'enemies si besoin
    reset_wave();

    
  }
  //Gestion affichage textes
  glUseProgram(gui_program_id);
  glDisable(GL_DEPTH_TEST);
  gestionTexte();
  draw_texts(&text_to_draw);
  glEnable(GL_DEPTH_TEST);
  //Changement de buffer d'affichage pour eviter un effet de scintillement
  glutSwapBuffers();
  
  cpt_tirs_enemies += 1;
  
}

/*****************************************************************************\
 * keyboard_callback                                                         *
 \*****************************************************************************/
static void keyboard_callback(unsigned char key, int, int)
{
  float d_angle=0.1f;
  float dz=0.5f;

  //quitte le programme si on appuie sur les touches 'q', 'Q', ou 'echap'
  //enregistre l'image si on appuie sur la touche 'p'
  switch (key)
  {
    case 'p':
      glhelper::print_screen();
      break;
    case 'q':
    case 'Q':
    case 27:
      exit(0);
      break;

    case 'o':
      model_monstre.angle.x += d_angle;
      break;
    case 'l':
      model_dinosaure.angle.x -= d_angle;
      break;

    case 'k':
      model_dinosaure.angle.y += d_angle;
      break;
    case 'm':
      model_dinosaure.angle.y -= d_angle;
      break;


    case 's':
      angle_view += d_angle;
      break;
    case 'f':
      angle_view -= d_angle;
      break;


    case 'e':
      transformation_view.translation.z += dz;
      break;
    case 'd':
      transformation_view.translation.z -= dz;
      break;

    case ' ':
      if(tir1==0){
        tir1=1;
        }
      else if (tir2==0){
        tir2=1;
      }
      else if (tir3==0){
        tir3=1;
      }
      break;

    case 'w':
      tir1=1;
      break;

    case 'b':
      model_dinosaure.angle.y += M_PI;
      break;

    case 'x':
      tir2=1;
      break;
    case 'c':
      tir3=1;
      break;

    case 'a':
      test_enemy_collision = 1;
      break;

  }

  model_dinosaure.transformation_model.rotation = matrice_rotation(model_dinosaure.angle.y, 0.0f,1.0f,0.0f) * matrice_rotation(model_dinosaure.angle.x, 1.0f,0.0f,0.0f);
  transformation_view.rotation = matrice_rotation(angle_view , 0.0f,1.0f,0.0f);
  model_monstre.transformation_model.rotation = matrice_rotation(model_monstre.angle.y, 0.0f,1.0f,0.0f) * matrice_rotation(model_monstre.angle.x, 1.0f,0.0f,0.0f);

  // Des exemples de camera vous sont données dans le programme 10

}

/*****************************************************************************\
 * special_callback                                                          *
 \*****************************************************************************/
static void special_callback(int key, int,int)
{
  float dL=0.03f;
  switch (key)
  {
    case GLUT_KEY_UP:
      deplacementHaut=1;
      //model_dinosaure.transformation_model.translation.y += dL; //rotation avec la touche du haut
      break;
    case GLUT_KEY_DOWN:
      deplacementBas=1;
      //model_dinosaure.transformation_model.translation.y -= dL; //rotation avec la touche du bas
      break;
    case GLUT_KEY_LEFT:
      deplacementGauche=1;
 //     model_dinosaure.transformation_model.translation.x -= dL; //rotation avec la touche de gauche
      break;
    case GLUT_KEY_RIGHT:
      deplacementDroite=1;
//      model_dinosaure.transformation_model.translation.x += dL; //rotation avec la touche de droite
      break;
  }
}
/***************************************
 * keyboardUp
 * ****************************/
static void keyboardUp_callback(unsigned char key, int, int)
{
  switch (key)
  {
    case 'e':
    break;

  }

}


static void specialUp_callback(int key, int,int)
{
  float dL=0.03f;
  switch (key)
  {
    case GLUT_KEY_UP:
      deplacementHaut=0;
      break;
    case GLUT_KEY_DOWN:
      deplacementBas=0;
      break;
    case GLUT_KEY_LEFT:
      deplacementGauche=0;
      break;
    case GLUT_KEY_RIGHT:
      deplacementDroite=0;
      break;
  }
}
/*****************************************************************************\
 * timer_callback                                                            *
 \*****************************************************************************/
static void timer_callback(int)
{
  //demande de rappel de cette fonction dans 25ms
  glutTimerFunc(25, timer_callback, 0);

  //reactualisation de l'affichage
  glutPostRedisplay();

}

int main(int argc, char** argv)
{
  //**********************************************//
  //Lancement des fonctions principales de GLUT
  //**********************************************//

  //initialisation
  glutInit(&argc, argv);

  //Mode d'affichage (couleur, gestion de profondeur, ...)
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | MACOSX_COMPATIBILITY);

  //Taille de la fenetre a l'ouverture
  glutInitWindowSize(600, 600);

  //Titre de la fenetre
  glutCreateWindow("OpenGL");

  //Fonction de la boucle d'affichage
  glutDisplayFunc(display_callback);

  //Fonction de gestion du clavier
  glutKeyboardFunc(keyboard_callback);

  //Fonction des touches speciales du clavier (fleches directionnelles)
  glutSpecialFunc(special_callback);

  //Fonction touche qui se relève
  glutKeyboardUpFunc(keyboardUp_callback);
  glutSpecialUpFunc(specialUp_callback);
  //Fonction d'appel d'affichage en chaine
  glutTimerFunc(25, timer_callback, 0);

  //Option de compatibilité
  glewExperimental = true;

  //Initialisation des fonctions OpenGL
  glewInit();

  //Affiche la version openGL utilisée
  std::cout << "OpenGL: " << (GLchar *)(glGetString(GL_VERSION)) << std::endl;

  //Notre fonction d'initialisation des donnees et chargement des shaders
  init();

  //Lancement de la boucle (infinie) d'affichage de la fenetre
  glutMainLoop();

  //Plus rien n'est execute apres cela
  return 0;
}

void draw_model(model m)
{

  //envoie des parametres uniformes
  {
    GLint loc_rotation_model = glGetUniformLocation(shader_program_id, "rotation_model"); //CHECK_GL_ERROR();
    if (loc_rotation_model == -1) std::cerr << "Pas de variable uniforme : rotation_model" << std::endl;
    glUniformMatrix4fv(loc_rotation_model,1,false,pointeur(m.transformation_model.rotation));    //CHECK_GL_ERROR();

    vec3 c = m.transformation_model.rotation_center;
    GLint loc_rotation_center_model = glGetUniformLocation(shader_program_id, "rotation_center_model"); //CHECK_GL_ERROR();
    if (loc_rotation_center_model == -1) std::cerr << "Pas de variable uniforme : rotation_center_model" << std::endl;
    glUniform4f(loc_rotation_center_model , c.x,c.y,c.z , 0.0f);                                 //CHECK_GL_ERROR();

    vec3 t = m.transformation_model.translation;
    GLint loc_translation_model = glGetUniformLocation(shader_program_id, "translation_model"); //CHECK_GL_ERROR();
    if (loc_translation_model == -1) std::cerr << "Pas de variable uniforme : translation_model" << std::endl;
    glUniform4f(loc_translation_model , t.x,t.y,t.z , 0.0f);                                    // CHECK_GL_ERROR();
  }

  glBindVertexArray(m.vao);//CHECK_GL_ERROR();

  //affichage
  {
    glBindTexture(GL_TEXTURE_2D, m.texture_id);                           //  CHECK_GL_ERROR();
    glDrawElements(GL_TRIANGLES, 3*m.nbr_triangle, GL_UNSIGNED_INT, 0);   //  CHECK_GL_ERROR();
  }
}

void init_model_1()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_obj_file("data/stegosaurus.obj");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.2f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f,-0.9f,
      0.0f, 0.0f,   s ,-2.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  model_dinosaure.transformation_model.rotation_center = vec3(0.0f,-0.5f,-2.0f);

  // Retourne le dinosaure
  model_dinosaure.angle.y += M_PI;
  model_dinosaure.transformation_model.rotation = matrice_rotation(model_dinosaure.angle.y, 0.0f,1.0f,0.0f) * matrice_rotation(model_dinosaure.angle.x, 1.0f,0.0f,0.0f);
  
  //translate le dinosaure pour qu'il apparaisse où l'on souhaite
  //model_dinosaure.transformation_model.translation.x += 10.5f;
  model_dinosaure.transformation_model.translation.y += 1.0f;

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_dinosaure.vao);
  glBindVertexArray(model_dinosaure.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo); CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 1
  model_dinosaure.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/stg2.tga",&model_dinosaure.texture_id,shader_program_id);
}

void init_sol1()
{
  //Creation manuelle du model 2

  //coordonnees geometriques des sommets
  vec3 p0=vec3(-25.0f,-0.9f,-75.0f);
  vec3 p1=vec3( 25.0f,-0.9f,-75.0f);
  vec3 p2=vec3( 25.0f,-0.9f, -25.0f);
  vec3 p3=vec3(-25.0f,-0.9f, -25.0f);

  //normales pour chaque sommet
  vec3 n0=vec3(0.0f,1.0f,0.0f);
  vec3 n1=n0;
  vec3 n2=n0;
  vec3 n3=n0;

  //couleur pour chaque sommet
  vec3 c0=vec3(1.0f,1.0f,1.0f);
  vec3 c1=c0;
  vec3 c2=c0;
  vec3 c3=c0;

  //texture du sommet
  vec2 t0=vec2(0.0f,0.0f);
  vec2 t1=vec2(1.0f,0.0f);
  vec2 t2=vec2(1.0f,1.0f);
  vec2 t3=vec2(0.0f,1.0f);

  vertex_opengl v0=vertex_opengl(p0,n0,c0,t0);
  vertex_opengl v1=vertex_opengl(p1,n1,c1,t1);
  vertex_opengl v2=vertex_opengl(p2,n2,c2,t2);
  vertex_opengl v3=vertex_opengl(p3,n3,c3,t3);

  //tableau entrelacant coordonnees-normales
  vertex_opengl geometrie[]={v0,v1,v2,v3};

  //indice des triangles
  triangle_index tri0=triangle_index(0,1,2);
  triangle_index tri1=triangle_index(0,2,3);
  triangle_index index[]={tri0,tri1};
  model_sol1.nbr_triangle = 2;



  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_sol1.vao);
  glBindVertexArray(model_sol1.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo);                                             CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo);                                CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,sizeof(geometrie),geometrie,GL_STATIC_DRAW);  CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi);                                            CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi);                       CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(index),index,GL_STATIC_DRAW);  CHECK_GL_ERROR();

  // Chargement de la texture
  load_texture("data/grass.tga",&model_sol1.texture_id,shader_program_id);

}

void init_sol2()
{
  //Creation manuelle du model 2

  //coordonnees geometriques des sommets
  vec3 p0=vec3(-25.0f,-0.9f,-25.0f);
  vec3 p1=vec3( 25.0f,-0.9f,-25.0f);
  vec3 p2=vec3( 25.0f,-0.9f, 25.0f);
  vec3 p3=vec3(-25.0f,-0.9f, 25.0f);

  //normales pour chaque sommet
  vec3 n0=vec3(0.0f,1.0f,0.0f);
  vec3 n1=n0;
  vec3 n2=n0;
  vec3 n3=n0;

  //couleur pour chaque sommet
  vec3 c0=vec3(1.0f,1.0f,1.0f);
  vec3 c1=c0;
  vec3 c2=c0;
  vec3 c3=c0;

  //texture du sommet
  vec2 t0=vec2(0.0f,0.0f);
  vec2 t1=vec2(1.0f,0.0f);
  vec2 t2=vec2(1.0f,1.0f);
  vec2 t3=vec2(0.0f,1.0f);

  vertex_opengl v0=vertex_opengl(p0,n0,c0,t0);
  vertex_opengl v1=vertex_opengl(p1,n1,c1,t1);
  vertex_opengl v2=vertex_opengl(p2,n2,c2,t2);
  vertex_opengl v3=vertex_opengl(p3,n3,c3,t3);

  //tableau entrelacant coordonnees-normales
  vertex_opengl geometrie[]={v0,v1,v2,v3};

  //indice des triangles
  triangle_index tri0=triangle_index(0,1,2);
  triangle_index tri1=triangle_index(0,2,3);
  triangle_index index[]={tri0,tri1};
  model_sol2.nbr_triangle = 2;

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_sol2.vao);
  glBindVertexArray(model_sol2.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo);                                             CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo);                                CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,sizeof(geometrie),geometrie,GL_STATIC_DRAW);  CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi);                                            CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi);                       CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(index),index,GL_STATIC_DRAW);  CHECK_GL_ERROR();

  // Chargement de la texture
  load_texture("data/grass.tga",&model_sol2.texture_id,shader_program_id);

}

void init_testArmadillo()
{
  // Chargement d'un maillage a partir d'un fichier
  //mesh m = load_off_file("data/armadillo_light.off");

mesh m = load_off_file("data/armadillo_light.off");
  // Affecte une transformation sur les sommets du maillage
  float s = 0.01f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 1.0f,
      0.0f,    s, 0.0f,4.0f,
      0.0f, 0.0f,   s ,-5.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,matrice_rotation(M_PI/2.0f,1.0f,0.0f,0.0f));
  apply_deformation(&m,matrice_rotation(1.5*M_PI/2.0f,0.0f,1.0f,0.0f));
  apply_deformation(&m,transform);

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_monstre.vao);
  glBindVertexArray(model_monstre.vao);
  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo);                                 CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();
  
  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 3
  model_monstre.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/white.tga",&model_monstre.texture_id,shader_program_id);


}

void init_model_balle1()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_obj_file("data/stegosaurus.obj");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.1f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f,-0.9f,
      0.0f, 0.0f,   s ,-3.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  model_balle1.transformation_model.rotation_center = vec3(0.0f,-0.5f,-2.0f);

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_balle1.vao);
  glBindVertexArray(model_balle1.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo); CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 4
  model_balle1.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/stg3.tga",&model_balle1.texture_id,shader_program_id);
}


void init_model_balle2()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_obj_file("data/stegosaurus.obj");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.1f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f,-0.9f,
      0.0f, 0.0f,   s ,-3.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  model_balle2.transformation_model.rotation_center = vec3(0.0f,-0.5f,-2.0f);

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_balle2.vao);
  glBindVertexArray(model_balle2.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo); CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 4
  model_balle2.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/stg3.tga",&model_balle2.texture_id,shader_program_id);
}
void init_model_balle3()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_obj_file("data/stegosaurus.obj");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.1f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f,-0.9f,
      0.0f, 0.0f,   s ,-3.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  model_balle3.transformation_model.rotation_center = vec3(0.0f,-0.5f,-2.0f);

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_balle3.vao);
  glBindVertexArray(model_balle3.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo); CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 4
  model_balle3.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/stg3.tga",&model_balle3.texture_id,shader_program_id);
}


void init_model_balleEnnemie1()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_obj_file("data/stegosaurus.obj");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.1f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f,-0.9f,
      0.0f, 0.0f,   s ,-3.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  model_balleEnnemie1.transformation_model.rotation_center = vec3(0.0f,-0.5f,-2.0f);

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_balleEnnemie1.vao);
  glBindVertexArray(model_balleEnnemie1.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo); CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 4
  model_balleEnnemie1.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/stg3.tga",&model_balleEnnemie1.texture_id,shader_program_id);
}

void init_model_balleEnnemie2()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_obj_file("data/stegosaurus.obj");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.1f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f,-0.9f,
      0.0f, 0.0f,   s ,-3.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  model_balleEnnemie2.transformation_model.rotation_center = vec3(0.0f,-0.5f,-2.0f);

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_balleEnnemie2.vao);
  glBindVertexArray(model_balleEnnemie2.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo); CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 4
  model_balleEnnemie2.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/stg3.tga",&model_balleEnnemie2.texture_id,shader_program_id);
}

void init_model_balleEnnemie3()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_obj_file("data/stegosaurus.obj");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.1f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f,-0.9f,
      0.0f, 0.0f,   s ,-3.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  model_balleEnnemie3.transformation_model.rotation_center = vec3(0.0f,-0.5f,-2.0f);

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_balleEnnemie3.vao);
  glBindVertexArray(model_balleEnnemie3.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo); CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 4
  model_balleEnnemie3.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/stg3.tga",&model_balleEnnemie3.texture_id,shader_program_id);
}

void init_model_balleEnnemie4()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_obj_file("data/stegosaurus.obj");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.1f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f,-0.9f,
      0.0f, 0.0f,   s ,-3.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  model_balleEnnemie4.transformation_model.rotation_center = vec3(0.0f,-0.5f,-2.0f);

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_balleEnnemie4.vao);
  glBindVertexArray(model_balleEnnemie4.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo); CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 4
  model_balleEnnemie4.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/stg3.tga",&model_balleEnnemie4.texture_id,shader_program_id);
}


//RANGEE 1 D'ENEMIES
//RANGEE 1 D'ENEMIES

void init_model_3()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_off_file("data/armadillo_light.off");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.01f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f,-0.2f,
      0.0f, 0.0f,   s ,-2.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,matrice_rotation(M_PI/2.0f,1.0f,0.0f,0.0f));
  apply_deformation(&m,matrice_rotation(1.5*M_PI/2.0f,0.0f,1.0f,0.0f));
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  model_armadillo.transformation_model.rotation_center = vec3(0.0f,-0.5f,-2.0f);

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_armadillo.vao);
  glBindVertexArray(model_armadillo.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo); CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 3
  model_armadillo.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/white.tga",&model_armadillo.texture_id,shader_program_id);
}

void init_model_4()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_off_file("data/armadillo_light.off");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.01f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f,-0.2f,
      0.0f, 0.0f,   s ,-2.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,matrice_rotation(M_PI/2.0f,1.0f,0.0f,0.0f));
  apply_deformation(&m,matrice_rotation(1.5*M_PI/2.0f,0.0f,1.0f,0.0f));
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  model_armadillo2.transformation_model.rotation_center = vec3(0.0f,-0.5f,-2.0f);

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_armadillo2.vao);
  glBindVertexArray(model_armadillo2.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo); CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 3
  model_armadillo2.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/white.tga",&model_armadillo2.texture_id,shader_program_id);
}

void init_model_5()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_off_file("data/armadillo_light.off");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.01f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f,-0.2f,
      0.0f, 0.0f,   s ,-2.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,matrice_rotation(M_PI/2.0f,1.0f,0.0f,0.0f));
  apply_deformation(&m,matrice_rotation(1.5*M_PI/2.0f,0.0f,1.0f,0.0f));
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  model_armadillo3.transformation_model.rotation_center = vec3(0.0f,-0.5f,-2.0f);

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_armadillo3.vao);
  glBindVertexArray(model_armadillo3.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo); CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 3
  model_armadillo3.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/white.tga",&model_armadillo3.texture_id,shader_program_id);
}

void init_model_6()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_off_file("data/armadillo_light.off");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.01f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f,-0.2f,
      0.0f, 0.0f,   s ,-2.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,matrice_rotation(M_PI/2.0f,1.0f,0.0f,0.0f));
  apply_deformation(&m,matrice_rotation(1.5*M_PI/2.0f,0.0f,1.0f,0.0f));
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  model_armadillo4.transformation_model.rotation_center = vec3(0.0f,-0.5f,-2.0f);

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_armadillo4.vao);
  glBindVertexArray(model_armadillo4.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo); CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 3
  model_armadillo4.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/white.tga",&model_armadillo4.texture_id,shader_program_id);
}

//RANGEE 2 D'ENEMIES :

void init_model_7()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_off_file("data/armadillo_light.off");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.01f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f,-0.2f,
      0.0f, 0.0f,   s ,-2.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,matrice_rotation(M_PI/2.0f,1.0f,0.0f,0.0f));
  apply_deformation(&m,matrice_rotation(1.5*M_PI/2.0f,0.0f,1.0f,0.0f));
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  model_armadillo5.transformation_model.rotation_center = vec3(0.0f,-0.5f,-2.0f);

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_armadillo5.vao);
  glBindVertexArray(model_armadillo5.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo); CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 3
  model_armadillo5.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/white.tga",&model_armadillo5.texture_id,shader_program_id);
}

void init_model_8()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_off_file("data/armadillo_light.off");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.01f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f,-0.2f,
      0.0f, 0.0f,   s ,-2.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,matrice_rotation(M_PI/2.0f,1.0f,0.0f,0.0f));
  apply_deformation(&m,matrice_rotation(1.5*M_PI/2.0f,0.0f,1.0f,0.0f));
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  model_armadillo6.transformation_model.rotation_center = vec3(0.0f,-0.5f,-2.0f);

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_armadillo6.vao);
  glBindVertexArray(model_armadillo6.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo); CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 3
  model_armadillo6.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/white.tga",&model_armadillo6.texture_id,shader_program_id);
}

void init_model_9()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_off_file("data/armadillo_light.off");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.01f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f,-0.2f,
      0.0f, 0.0f,   s ,-2.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,matrice_rotation(M_PI/2.0f,1.0f,0.0f,0.0f));
  apply_deformation(&m,matrice_rotation(1.5*M_PI/2.0f,0.0f,1.0f,0.0f));
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  model_armadillo7.transformation_model.rotation_center = vec3(0.0f,-0.5f,-2.0f);

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_armadillo7.vao);
  glBindVertexArray(model_armadillo7.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo); CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 3
  model_armadillo7.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/white.tga",&model_armadillo7.texture_id,shader_program_id);
}

void init_model_10()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_off_file("data/armadillo_light.off");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.01f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f,-0.2f,
      0.0f, 0.0f,   s ,-2.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,matrice_rotation(M_PI/2.0f,1.0f,0.0f,0.0f));
  apply_deformation(&m,matrice_rotation(1.5*M_PI/2.0f,0.0f,1.0f,0.0f));
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  model_armadillo8.transformation_model.rotation_center = vec3(0.0f,-0.5f,-2.0f);

  // Calcul automatique des normales du maillage
  update_normals(&m);
  // Les sommets sont affectes a une couleur blanche
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  //attribution d'une liste d'état (1 indique la création d'une seule liste)
  glGenVertexArrays(1, &model_armadillo8.vao);
  glBindVertexArray(model_armadillo8.vao);

  GLuint vbo;
  //attribution d'un buffer de donnees (1 indique la création d'un buffer)
  glGenBuffers(1,&vbo); CHECK_GL_ERROR();
  //affectation du buffer courant
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  //copie des donnees des sommets sur la carte graphique
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Active l'utilisation des données de positions (le 0 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les positions des sommets
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  // Active l'utilisation des données de normales (le 1 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les normales des sommets
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  // Active l'utilisation des données de couleurs (le 2 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les couleurs des sommets
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  // Active l'utilisation des données de textures (le 3 correspond à la location dans le vertex shader)
  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  // Indique comment le buffer courant (dernier vbo "bindé") est utilisé pour les textures des sommets
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  GLuint vboi;
  //attribution d'un autre buffer de donnees
  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  //affectation du buffer courant (buffer d'indice)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  //copie des indices sur la carte graphique
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  // Nombre de triangles de l'objet 3
  model_armadillo8.nbr_triangle = m.connectivity.size();

  // Chargement de la texture
  load_texture("data/white.tga",&model_armadillo8.texture_id,shader_program_id);
}

void load_texture(const char* filename,GLuint *texture_id,GLuint program_id)
{
  // Chargement d'une texture (seul les textures tga sont supportes)
  Image  *image = image_load_tga(filename);
  if (image) //verification que l'image est bien chargee
  {

    //Creation d'un identifiant pour la texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); CHECK_GL_ERROR();
    glGenTextures(1, texture_id); CHECK_GL_ERROR();

    //Selection de la texture courante a partir de son identifiant
    glBindTexture(GL_TEXTURE_2D, *texture_id); CHECK_GL_ERROR();

    //Parametres de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); CHECK_GL_ERROR();

    //Envoie de l'image en memoire video
    if(image->type==IMAGE_TYPE_RGB){ //image RGB
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data); CHECK_GL_ERROR();}
    else if(image->type==IMAGE_TYPE_RGBA){ //image RGBA
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data); CHECK_GL_ERROR();}
    else{
      std::cout<<"Image type not handled"<<std::endl;}

    delete image;
  }
  else
  {
    std::cerr<<"Erreur chargement de l'image, etes-vous dans le bon repertoire?"<<std::endl;
    abort();
  }

  GLint loc_texture = glGetUniformLocation(program_id, "texture"); CHECK_GL_ERROR();
  if (loc_texture == -1) std::cerr << "Pas de variable uniforme : texture" << std::endl;
  glUniform1i (loc_texture,0); CHECK_GL_ERROR();
}


void init_text(text *t){
    vec3 p0=vec3( 0.0f, 0.0f, 0.0f);
    vec3 p1=vec3( 0.0f, 1.0f, 0.0f);
    vec3 p2=vec3( 1.0f, 1.0f, 0.0f);
    vec3 p3=vec3( 1.0f, 0.0f, 0.0f);

    vec3 geometrie[4] = {p0, p1, p2, p3}; 
    triangle_index index[2] = { triangle_index(0, 1, 2), triangle_index(0, 2, 3)};

  glGenVertexArrays(1, &(t->vao));                                              CHECK_GL_ERROR();
  glBindVertexArray(t->vao);                                                  CHECK_GL_ERROR();

  GLuint vbo;
  glGenBuffers(1, &vbo);                                                       CHECK_GL_ERROR();
  glBindBuffer(GL_ARRAY_BUFFER,vbo);                                          CHECK_GL_ERROR();
  glBufferData(GL_ARRAY_BUFFER,sizeof(geometrie),geometrie,GL_STATIC_DRAW);   CHECK_GL_ERROR();

  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); CHECK_GL_ERROR();

  GLuint vboi;
  glGenBuffers(1,&vboi);                                                      CHECK_GL_ERROR();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi);                                 CHECK_GL_ERROR();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(index),index,GL_STATIC_DRAW);   CHECK_GL_ERROR();

  load_texture("data/data_fontB.tga",&t->texture_id,gui_program_id);
}

void draw_texts(text *text_t){
   vec2 size = (text_t->topRight - text_t->bottomLeft) / float(text_t->value.size());
  //  std::cout << size << " " << text_t->bottomLeft << std::endl;
  
  //Send uniforma parameters
  
  GLint loc_size = glGetUniformLocation(gui_program_id, "size"); //CHECK_GL_ERROR();
  if (loc_size == -1) std::cerr << "Pas de variable uniforme : size" << std::endl;
  glUniform2f(loc_size,size.x, size.y);    //CHECK_GL_ERROR();

  glBindVertexArray(text_t->vao);//CHECK_GL_ERROR();
  
  for(int i = 0; i < text_t->value.size(); ++i)
  {
    GLint loc_start = glGetUniformLocation(gui_program_id, "start"); //CHECK_GL_ERROR();
    if (loc_start == -1) std::cerr << "Pas de variable uniforme : start" << std::endl;
    glUniform2f(loc_start,text_t->bottomLeft.x+i*size.x, text_t->bottomLeft.y);   // CHECK_GL_ERROR();

    GLint loc_char = glGetUniformLocation(gui_program_id, "c");// CHECK_GL_ERROR();
    if (loc_char == -1) std::cerr << "Pas de variable uniforme : c" << std::endl;
    glUniform1i(loc_char, (int)text_t->value[i]);   // CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, text_t->texture_id);                   //    CHECK_GL_ERROR();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);               //     CHECK_GL_ERROR();
  }
}

void gestionTirs(){

  if(tir1==1){
  model_balle1.transformation_model.translation.z -= 0.06;
  draw_model(model_balle1);
  if (model_balle1.transformation_model.translation.z<=-10.0){  
    model_balle1.transformation_model.translation.z = 0.0;
    tir1=0;
    }
  }
  else {
      model_balle1.transformation_model.translation.x=model_dinosaure.transformation_model.translation.x;
      model_balle1.transformation_model.translation.y=model_dinosaure.transformation_model.translation.y;

  }
  if (tir2==1){
  model_balle2.transformation_model.translation.z -= 0.06;
  draw_model(model_balle2);
  if (model_balle2.transformation_model.translation.z<=-10.0){  
    model_balle2.transformation_model.translation.z = 0.0;
    tir2=0;
    }
  }
  else {
      model_balle2.transformation_model.translation.x=model_dinosaure.transformation_model.translation.x;
      model_balle2.transformation_model.translation.y=model_dinosaure.transformation_model.translation.y;
  }
  if (tir3==1){
  model_balle3.transformation_model.translation.z -= 0.06;
  draw_model(model_balle3);
  if (model_balle3.transformation_model.translation.z<=-10.0){  
    model_balle3.transformation_model.translation.z = 0.0;
    tir3=0;
    }
  }  
  else {
      model_balle3.transformation_model.translation.x=model_dinosaure.transformation_model.translation.x;
      model_balle3.transformation_model.translation.y=model_dinosaure.transformation_model.translation.y;

  }

}


void gestionDeplacement(){

  if(deplacementHaut==1 ){
    model_dinosaure.transformation_model.translation.y += 0.03;
  }
  if(deplacementBas==1&& model_dinosaure.transformation_model.translation.y>0.0){
    model_dinosaure.transformation_model.translation.y -= 0.03;
  }

  if(deplacementGauche==1){
    model_dinosaure.transformation_model.translation.x -= 0.03;
  }
  if(deplacementDroite==1){
    model_dinosaure.transformation_model.translation.x += 0.03;
  }
}

void gestionTexte(){
  int tirDispo=3-(tir1+tir2+tir3);
  if (test_enemy_collision == 1) {
    if (test_draw_player == 1) {
      text_to_draw.value="Tirs disponibles :"+std::to_string(tirDispo)+"   Score :"+std::to_string(score);
      text_to_draw.bottomLeft = vec2(-0.8, 0.9);
      text_to_draw.topRight = vec2(0.8, 3);
    } else {
      text_to_draw.value="GAME OVER - Score :"+std::to_string(score);
      text_to_draw.bottomLeft = vec2(-0.8,0.3);
      text_to_draw.topRight = vec2(0.8,2.5);}
    }
    else {
      text_to_draw.value="Tapez A pour lancer la partie";
      text_to_draw.bottomLeft = vec2(-0.8,0.3);
      text_to_draw.topRight = vec2(0.8,2.5);}
    }
void gestionSol(){
  model_sol1.transformation_model.translation.z += 0.04; //rotation avec la touche du haut
  model_sol2.transformation_model.translation.z += 0.04;
  if (model_sol1.transformation_model.translation.z > 75){
    model_sol1.transformation_model.translation.z -= 100;
  }
  if (model_sol2.transformation_model.translation.z > 25){
    model_sol2.transformation_model.translation.z -= 100;
  }
}

void gestionTirsEnnemis(){
  if(tirEnnemi1==1 && test_draw_1 == 1){
  model_balleEnnemie1.transformation_model.translation.z += 0.09;
  draw_model(model_balleEnnemie1);
    if (model_balleEnnemie1.transformation_model.translation.z>=model_dinosaure.transformation_model.translation.z+3){  
      model_balleEnnemie1.transformation_model.translation.z = model_armadillo.transformation_model.translation.z;      
      tirEnnemi1=0;
      }
  }
  else {
      model_balleEnnemie1.transformation_model.translation.x=model_armadillo.transformation_model.translation.x;
      model_balleEnnemie1.transformation_model.translation.y=model_armadillo.transformation_model.translation.y;
      tirEnnemi1=1;
  }
  if(tirEnnemi2==1 && test_draw_2 == 1){
  model_balleEnnemie2.transformation_model.translation.z += 0.09;
  draw_model(model_balleEnnemie2);
    if (model_balleEnnemie2.transformation_model.translation.z>=model_dinosaure.transformation_model.translation.z+3){ 
      model_balleEnnemie2.transformation_model.translation.z = model_armadillo2.transformation_model.translation.z;            
      tirEnnemi2=0;
      }
  }
  else {
      model_balleEnnemie2.transformation_model.translation.x=model_armadillo2.transformation_model.translation.x;
      model_balleEnnemie2.transformation_model.translation.y=model_armadillo2.transformation_model.translation.y;
      tirEnnemi2=1;
  }

  if(tirEnnemi3==1 && test_draw_5 == 1){
  model_balleEnnemie3.transformation_model.translation.z += 0.09;
  draw_model(model_balleEnnemie3);
    if (model_balleEnnemie3.transformation_model.translation.z>=model_dinosaure.transformation_model.translation.z+3){  
      tirEnnemi3=0;
      }
  }
  else {
      model_balleEnnemie3.transformation_model.translation.x=model_armadillo5.transformation_model.translation.x;
      model_balleEnnemie3.transformation_model.translation.y=model_armadillo5.transformation_model.translation.y;
      model_balleEnnemie3.transformation_model.translation.z = model_armadillo5.transformation_model.translation.z;      
      tirEnnemi3=1;
  }

  if(tirEnnemi4==1 && test_draw_7 == 1){
  model_balleEnnemie4.transformation_model.translation.z += 0.09;
  draw_model(model_balleEnnemie4);
    if (model_balleEnnemie4.transformation_model.translation.z>=model_dinosaure.transformation_model.translation.z+3){  
      tirEnnemi4=0;
      }
  }
  else {
      model_balleEnnemie4.transformation_model.translation.x=model_armadillo7.transformation_model.translation.x;
      model_balleEnnemie4.transformation_model.translation.y=model_armadillo7.transformation_model.translation.y;
      model_balleEnnemie4.transformation_model.translation.z = model_armadillo7.transformation_model.translation.z;

      tirEnnemi4=1;
  }
}

//initialisation enemis
void init_enemy_squad() {
  init_model_3();
  init_model_4();
  init_model_5();
  init_model_6();
  init_model_7();
  init_model_8();
  init_model_9();
  init_model_10();  
}
void draw_enemy_squad() {
  if (test_draw_1 == 1) { draw_model(model_armadillo); }
  if (test_draw_2 == 1) { draw_model(model_armadillo2); }
  if (test_draw_3 == 1) { draw_model(model_armadillo3); }
  if (test_draw_4 == 1) { draw_model(model_armadillo4); }
  if (test_draw_5 == 1) { draw_model(model_armadillo5); }
  if (test_draw_6 == 1) { draw_model(model_armadillo6); }
  if (test_draw_7 == 1) { draw_model(model_armadillo7); }
  if (test_draw_8 == 1) { draw_model(model_armadillo8); }
}
void move_enemy_squad() {
  if (init_parameter == 1) {  //si c'est le premier appel de cette fonction on positionne les armadillos à leur coordonnées d'origine
    
    //translation sur z pour chaque armadillo : on leur fixe une position d'origine pour faire deux rangées de quatres
    model_armadillo.transformation_model.translation.z -= 10.0f;
    model_armadillo2.transformation_model.translation.z -= 10.0f;
    model_armadillo3.transformation_model.translation.z -= 10.0f;
    model_armadillo4.transformation_model.translation.z -= 10.0f;

    model_armadillo5.transformation_model.translation.z -= 10.0f;
    model_armadillo6.transformation_model.translation.z -= 10.0f;
    model_armadillo7.transformation_model.translation.z -= 10.0f;
    model_armadillo8.transformation_model.translation.z -= 10.0f;

    // model_armadillo5.transformation_model.translation.z += -2.0f;
    // model_armadillo6.transformation_model.translation.z += -2.0f;
    // model_armadillo7.transformation_model.translation.z += -2.0f;
    // model_armadillo8.transformation_model.translation.z += -2.0f;

    //positionnement de chaque armadillo pour qu'ils soient tous alignés et qu'ils forment deux rangée l'une au dessus de l'autre
    model_armadillo2.transformation_model.translation.x += 1.5f;
    model_armadillo3.transformation_model.translation.x += 3.0f;
    model_armadillo4.transformation_model.translation.x += -1.5f;
    model_armadillo6.transformation_model.translation.x += 1.5f;
    model_armadillo7.transformation_model.translation.x += 3.0f;
    model_armadillo8.transformation_model.translation.x += -1.5f;

    model_armadillo5.transformation_model.translation.y += 2.0f;
    model_armadillo6.transformation_model.translation.y += 2.0f;
    model_armadillo7.transformation_model.translation.y += 2.0f;
    model_armadillo8.transformation_model.translation.y += 2.0f;

    //application de la même rotation à chaque armadillo pour qu'il soit de face au joueur
    model_armadillo.transformation_model.rotation = matrice_rotation(move_rotation, 0.0f, 1.0f, 0.0f);
    model_armadillo2.transformation_model.rotation = matrice_rotation(move_rotation, 0.0f,1.0f,0.0f);
    model_armadillo3.transformation_model.rotation = matrice_rotation(move_rotation, 0.0f, 1.0f, 0.0f);
    model_armadillo4.transformation_model.rotation = matrice_rotation(move_rotation, 0.0f, 1.0f, 0.0f);
    model_armadillo5.transformation_model.rotation = matrice_rotation(move_rotation, 0.0f, 1.0f, 0.0f);
    model_armadillo6.transformation_model.rotation = matrice_rotation(move_rotation, 0.0f,1.0f,0.0f);
    model_armadillo7.transformation_model.rotation = matrice_rotation(move_rotation, 0.0f, 1.0f, 0.0f);
    model_armadillo8.transformation_model.rotation = matrice_rotation(move_rotation, 0.0f, 1.0f, 0.0f);
    //move_rotation += 0.1f;

    //Décalage vers la gauche
    model_armadillo.transformation_model.translation.x += -2.5f;
    model_armadillo2.transformation_model.translation.x += -2.5f;
    model_armadillo3.transformation_model.translation.x += -2.5f;
    model_armadillo4.transformation_model.translation.x += -2.5f;
    model_armadillo5.transformation_model.translation.x += -2.5f;
    model_armadillo6.transformation_model.translation.x += -2.5f;
    model_armadillo7.transformation_model.translation.x += -2.5f;
    model_armadillo8.transformation_model.translation.x += -2.5f;


    init_parameter = 0; //on passe le init_parameter à 1 car on a terminé d'initialiser les paramètres de départ, on a plus besoin de rentrer dans cette condition
  }
  if (reset_parameter == 1) {
    model_armadillo.transformation_model.translation.z = -10.0f;
    model_armadillo2.transformation_model.translation.z = -10.0f;
    model_armadillo3.transformation_model.translation.z = -10.0f;
    model_armadillo4.transformation_model.translation.z = -10.0f;

    model_armadillo5.transformation_model.translation.z = -10.0f;
    model_armadillo6.transformation_model.translation.z = -10.0f;
    model_armadillo7.transformation_model.translation.z = -10.0f;
    model_armadillo8.transformation_model.translation.z = -10.0f;

    reset_parameter = 0;
  }
  if (cpt_boucle%70 == 0) {
    model_armadillo.transformation_model.translation.z += move_forward;
    model_armadillo2.transformation_model.translation.z += move_forward;
    model_armadillo3.transformation_model.translation.z += move_forward;
    model_armadillo4.transformation_model.translation.z += move_forward;
    model_armadillo5.transformation_model.translation.z += move_forward;
    model_armadillo6.transformation_model.translation.z += move_forward;
    model_armadillo7.transformation_model.translation.z += move_forward;
    model_armadillo8.transformation_model.translation.z += move_forward;
    move_side = -move_side;
    cpt_boucle = -70;
  }
  model_armadillo.transformation_model.translation.x += move_side;
  model_armadillo2.transformation_model.translation.x += move_side;
  model_armadillo3.transformation_model.translation.x += move_side; 
  model_armadillo4.transformation_model.translation.x += move_side;
  model_armadillo5.transformation_model.translation.x += move_side;
  model_armadillo6.transformation_model.translation.x += move_side;
  model_armadillo7.transformation_model.translation.x += move_side; 
  model_armadillo8.transformation_model.translation.x += move_side;

  cpt_boucle += 1;
}

void player_collision_detection() {
  /*Cette fonction test la collision entre les enemies et le joueur : si les enemies touchent le joueur on lance le game over*/
  if (model_armadillo.transformation_model.translation.z >= -1.8f) {
    game_over();
    model_armadillo.transformation_model.rotation = matrice_rotation(move_rotation, 3.0f, 1.0f, 0.0f);
    model_armadillo2.transformation_model.rotation = matrice_rotation(move_rotation, 3.0f, 1.0f, 0.0f);
    model_armadillo3.transformation_model.rotation = matrice_rotation(move_rotation, 3.0f, 1.0f, 0.0f);
    model_armadillo4.transformation_model.rotation = matrice_rotation(move_rotation, 3.0f, 1.0f, 0.0f);
    model_armadillo5.transformation_model.rotation = matrice_rotation(move_rotation, 3.0f, 1.0f, 0.0f);
    model_armadillo6.transformation_model.rotation = matrice_rotation(move_rotation, 3.0f, 1.0f, 0.0f);
    model_armadillo7.transformation_model.rotation = matrice_rotation(move_rotation, 3.0f, 1.0f, 0.0f);
    model_armadillo8.transformation_model.rotation = matrice_rotation(move_rotation, 3.0f, 1.0f, 0.0f);
    move_rotation += 0.5f;
  }
}

void reset_wave() {
  if (test_draw_1 == 0 && test_draw_2 == 0 && test_draw_3 == 0 && test_draw_4 == 0 && test_draw_5 == 0 && test_draw_6 == 0 && test_draw_7 == 0 && test_draw_8 == 0) {
    //on active l'autorisation de dessiner les enemies
    test_draw_1 = 1;
    test_draw_2 = 1;
    test_draw_3 = 1;
    test_draw_4 = 1;
    test_draw_5 = 1;
    test_draw_6 = 1;
    test_draw_7 = 1;
    test_draw_8 = 1;

    //on active les parametres de reset pour remettre les enemies à leur position d'origine
    reset_parameter = 1;
  }
}

void bullet_collision_detection() {
  float taille_hitbox = 1.0f;
  if (model_balle1.transformation_model.translation.z <= model_armadillo.transformation_model.translation.z+0.5 && model_balle1.transformation_model.translation.z >= model_armadillo.transformation_model.translation.z-0.5 ){
    if (model_armadillo.transformation_model.translation.x-taille_hitbox <= model_balle1.transformation_model.translation.x && model_balle1.transformation_model.translation.x <= model_armadillo.transformation_model.translation.x+taille_hitbox && test_draw_1 == 1){
      if (model_armadillo.transformation_model.translation.y-taille_hitbox <= model_balle1.transformation_model.translation.y && model_balle1.transformation_model.translation.y <= model_armadillo.transformation_model.translation.y+taille_hitbox){
        //Armadillo1 touché
        test_draw_1 = 0;
        score += 175;
      }
    }
    if (model_armadillo2.transformation_model.translation.x-taille_hitbox <= model_balle1.transformation_model.translation.x && model_balle1.transformation_model.translation.x <= model_armadillo2.transformation_model.translation.x+taille_hitbox && test_draw_2 == 1) {
      if (model_armadillo2.transformation_model.translation.y-taille_hitbox <= model_balle1.transformation_model.translation.y && model_balle1.transformation_model.translation.y <= model_armadillo2.transformation_model.translation.y+taille_hitbox){
        //Armadillo2 touché
        test_draw_2 = 0;
        score += 175;
      }
    }
    if (model_armadillo3.transformation_model.translation.x-taille_hitbox <= model_balle1.transformation_model.translation.x && model_balle1.transformation_model.translation.x <= model_armadillo3.transformation_model.translation.x+taille_hitbox && test_draw_3 == 1) {
      if (model_armadillo3.transformation_model.translation.y-taille_hitbox <= model_balle1.transformation_model.translation.y && model_balle1.transformation_model.translation.y <= model_armadillo3.transformation_model.translation.y+taille_hitbox){
        //Armadillo3 touché
        test_draw_3 = 0;
        score += 175;
      }
    }
    if (model_armadillo4.transformation_model.translation.x-taille_hitbox <= model_balle1.transformation_model.translation.x && model_balle1.transformation_model.translation.x <= model_armadillo4.transformation_model.translation.x+taille_hitbox && test_draw_4 == 1) {
      if (model_armadillo4.transformation_model.translation.y-taille_hitbox <= model_balle1.transformation_model.translation.y && model_balle1.transformation_model.translation.y <= model_armadillo4.transformation_model.translation.y+taille_hitbox){
        //Armadillo4 touché
        test_draw_4 = 0;
        score += 175;
      }
    }
    if (model_armadillo5.transformation_model.translation.x-taille_hitbox <= model_balle1.transformation_model.translation.x && model_balle1.transformation_model.translation.x <= model_armadillo5.transformation_model.translation.x+taille_hitbox && test_draw_5 == 1) {
      if (model_armadillo5.transformation_model.translation.y-taille_hitbox <= model_balle1.transformation_model.translation.y && model_balle1.transformation_model.translation.y <= model_armadillo5.transformation_model.translation.y+taille_hitbox){
        //Armadillo5 touché
        test_draw_5 = 0;
        score += 175;
      }
    }
    if (model_armadillo6.transformation_model.translation.x-taille_hitbox <= model_balle1.transformation_model.translation.x && model_balle1.transformation_model.translation.x <= model_armadillo6.transformation_model.translation.x+taille_hitbox && test_draw_6 == 1) {
      if (model_armadillo6.transformation_model.translation.y-taille_hitbox <= model_balle1.transformation_model.translation.y && model_balle1.transformation_model.translation.y <= model_armadillo6.transformation_model.translation.y+taille_hitbox){
        //Armadillo6 touché
        test_draw_6 = 0;
        score += 175;
      }
    }
    if (model_armadillo7.transformation_model.translation.x-taille_hitbox <= model_balle1.transformation_model.translation.x && model_balle1.transformation_model.translation.x <= model_armadillo7.transformation_model.translation.x+taille_hitbox && test_draw_7 == 1) {
      if (model_armadillo7.transformation_model.translation.y-taille_hitbox <= model_balle1.transformation_model.translation.y && model_balle1.transformation_model.translation.y <= model_armadillo7.transformation_model.translation.y+taille_hitbox){
        //Armadillo7 touché
        test_draw_7 = 0;
        score += 175;
      }
    }
    if (model_armadillo8.transformation_model.translation.x-taille_hitbox <= model_balle1.transformation_model.translation.x && model_balle1.transformation_model.translation.x <= model_armadillo8.transformation_model.translation.x+taille_hitbox && test_draw_8 == 1) {
      if (model_armadillo8.transformation_model.translation.y-taille_hitbox <= model_balle1.transformation_model.translation.y && model_balle1.transformation_model.translation.y <= model_armadillo8.transformation_model.translation.y+taille_hitbox){
        //Armadillo8 touché
        test_draw_8 = 0;
        score += 175;
      }
    }
    model_balle1.transformation_model.translation.z = 0.0f;
    tir1 = 0;
  }
  if (model_balle2.transformation_model.translation.z <= model_armadillo.transformation_model.translation.z+0.5 && model_balle2.transformation_model.translation.z >= model_armadillo.transformation_model.translation.z-0.5 ){
    if (model_armadillo.transformation_model.translation.x-taille_hitbox <= model_balle2.transformation_model.translation.x && model_balle2.transformation_model.translation.x <= model_armadillo.transformation_model.translation.x+taille_hitbox && test_draw_1 == 1){
      if (model_armadillo.transformation_model.translation.y-taille_hitbox <= model_balle2.transformation_model.translation.y && model_balle2.transformation_model.translation.y <= model_armadillo.transformation_model.translation.y+taille_hitbox){
        //Armadillo1 touché
        test_draw_1 = 0;
        score += 175;
      }
    }
    if (model_armadillo2.transformation_model.translation.x-taille_hitbox <= model_balle2.transformation_model.translation.x && model_balle2.transformation_model.translation.x <= model_armadillo2.transformation_model.translation.x+taille_hitbox && test_draw_2 == 1) {
      if (model_armadillo2.transformation_model.translation.y-taille_hitbox <= model_balle2.transformation_model.translation.y && model_balle2.transformation_model.translation.y <= model_armadillo2.transformation_model.translation.y+taille_hitbox){
        //Armadillo2 touché
        test_draw_2 = 0;
        score += 175;
      }
    }
    if (model_armadillo3.transformation_model.translation.x-taille_hitbox <= model_balle2.transformation_model.translation.x && model_balle2.transformation_model.translation.x <= model_armadillo3.transformation_model.translation.x+taille_hitbox && test_draw_3 == 1) {
      if (model_armadillo3.transformation_model.translation.y-taille_hitbox <= model_balle2.transformation_model.translation.y && model_balle2.transformation_model.translation.y <= model_armadillo3.transformation_model.translation.y+taille_hitbox){
        //Armadillo3 touché
        test_draw_3 = 0;
        score += 175;
      }
    }
    if (model_armadillo4.transformation_model.translation.x-taille_hitbox <= model_balle2.transformation_model.translation.x && model_balle2.transformation_model.translation.x <= model_armadillo4.transformation_model.translation.x+taille_hitbox && test_draw_4 == 1) {
      if (model_armadillo4.transformation_model.translation.y-taille_hitbox <= model_balle2.transformation_model.translation.y && model_balle2.transformation_model.translation.y <= model_armadillo4.transformation_model.translation.y+taille_hitbox){
        //Armadillo4 touché
        test_draw_4 = 0;
        score += 175;
      }
    }
    if (model_armadillo5.transformation_model.translation.x-taille_hitbox <= model_balle2.transformation_model.translation.x && model_balle2.transformation_model.translation.x <= model_armadillo5.transformation_model.translation.x+taille_hitbox && test_draw_5 == 1) {
      if (model_armadillo5.transformation_model.translation.y-taille_hitbox <= model_balle2.transformation_model.translation.y && model_balle2.transformation_model.translation.y <= model_armadillo5.transformation_model.translation.y+taille_hitbox){
        //Armadillo5 touché
        test_draw_5 = 0;
        score += 175;
      }
    }
    if (model_armadillo6.transformation_model.translation.x-taille_hitbox <= model_balle2.transformation_model.translation.x && model_balle2.transformation_model.translation.x <= model_armadillo6.transformation_model.translation.x+taille_hitbox && test_draw_6 == 1) {
      if (model_armadillo6.transformation_model.translation.y-taille_hitbox <= model_balle2.transformation_model.translation.y && model_balle2.transformation_model.translation.y <= model_armadillo6.transformation_model.translation.y+taille_hitbox){
        //Armadillo6 touché
        test_draw_6 = 0;
        score += 175;
      }
    }
    if (model_armadillo7.transformation_model.translation.x-taille_hitbox <= model_balle2.transformation_model.translation.x && model_balle2.transformation_model.translation.x <= model_armadillo7.transformation_model.translation.x+taille_hitbox && test_draw_7 == 1) {
      if (model_armadillo7.transformation_model.translation.y-taille_hitbox <= model_balle2.transformation_model.translation.y && model_balle2.transformation_model.translation.y <= model_armadillo7.transformation_model.translation.y+taille_hitbox){
        //Armadillo7 touché
        test_draw_7 = 0;
        score += 175;
      }
    }
    if (model_armadillo8.transformation_model.translation.x-taille_hitbox <= model_balle2.transformation_model.translation.x && model_balle2.transformation_model.translation.x <= model_armadillo8.transformation_model.translation.x+taille_hitbox && test_draw_8 == 1) {
      if (model_armadillo8.transformation_model.translation.y-taille_hitbox <= model_balle2.transformation_model.translation.y && model_balle2.transformation_model.translation.y <= model_armadillo8.transformation_model.translation.y+taille_hitbox){
        //Armadillo8 touché
        test_draw_8 = 0;
        score += 175;
      }
    }
    model_balle2.transformation_model.translation.z = 0.0f;
    tir2 = 0;
  }
  if (model_balle3.transformation_model.translation.z <= model_armadillo.transformation_model.translation.z+0.5 && model_balle3.transformation_model.translation.z >= model_armadillo.transformation_model.translation.z-0.5 ){
    if (model_armadillo.transformation_model.translation.x-taille_hitbox <= model_balle3.transformation_model.translation.x && model_balle3.transformation_model.translation.x <= model_armadillo.transformation_model.translation.x+taille_hitbox && test_draw_1 == 1){
      if (model_armadillo.transformation_model.translation.y-taille_hitbox <= model_balle3.transformation_model.translation.y && model_balle3.transformation_model.translation.y <= model_armadillo.transformation_model.translation.y+taille_hitbox){
        //Armadillo1 touché
        test_draw_1 = 0;
        score += 175;
      }
    }
    if (model_armadillo2.transformation_model.translation.x-taille_hitbox <= model_balle3.transformation_model.translation.x && model_balle3.transformation_model.translation.x <= model_armadillo2.transformation_model.translation.x+taille_hitbox && test_draw_2 == 1) {
      if (model_armadillo2.transformation_model.translation.y-taille_hitbox <= model_balle3.transformation_model.translation.y && model_balle3.transformation_model.translation.y <= model_armadillo2.transformation_model.translation.y+taille_hitbox){
        //Armadillo2 touché
        test_draw_2 = 0;
        score += 175;
      }
    }
    if (model_armadillo3.transformation_model.translation.x-taille_hitbox <= model_balle3.transformation_model.translation.x && model_balle3.transformation_model.translation.x <= model_armadillo3.transformation_model.translation.x+taille_hitbox && test_draw_3 == 1) {
      if (model_armadillo3.transformation_model.translation.y-taille_hitbox <= model_balle3.transformation_model.translation.y && model_balle3.transformation_model.translation.y <= model_armadillo3.transformation_model.translation.y+taille_hitbox){
        //Armadillo3 touché
        test_draw_3 = 0;
        score += 175;
      }
    }
    if (model_armadillo4.transformation_model.translation.x-taille_hitbox <= model_balle3.transformation_model.translation.x && model_balle3.transformation_model.translation.x <= model_armadillo4.transformation_model.translation.x+taille_hitbox && test_draw_4 == 1) {
      if (model_armadillo4.transformation_model.translation.y-taille_hitbox <= model_balle3.transformation_model.translation.y && model_balle3.transformation_model.translation.y <= model_armadillo4.transformation_model.translation.y+taille_hitbox){
        //Armadillo4 touché
        test_draw_4 = 0;
        score += 175;
      }
    }
    if (model_armadillo5.transformation_model.translation.x-taille_hitbox <= model_balle3.transformation_model.translation.x && model_balle3.transformation_model.translation.x <= model_armadillo5.transformation_model.translation.x+taille_hitbox && test_draw_5 == 1) {
      if (model_armadillo5.transformation_model.translation.y-taille_hitbox <= model_balle3.transformation_model.translation.y && model_balle3.transformation_model.translation.y <= model_armadillo5.transformation_model.translation.y+taille_hitbox){
        //Armadillo5 touché
        test_draw_5 = 0;
        score += 175;
      }
    }
    if (model_armadillo6.transformation_model.translation.x-taille_hitbox <= model_balle3.transformation_model.translation.x && model_balle3.transformation_model.translation.x <= model_armadillo6.transformation_model.translation.x+taille_hitbox && test_draw_6 == 1) {
      if (model_armadillo6.transformation_model.translation.y-taille_hitbox <= model_balle3.transformation_model.translation.y && model_balle3.transformation_model.translation.y <= model_armadillo6.transformation_model.translation.y+taille_hitbox){
        //Armadillo6 touché
        test_draw_6 = 0;
        score += 175;
      }
    }
    if (model_armadillo7.transformation_model.translation.x-taille_hitbox <= model_balle3.transformation_model.translation.x && model_balle3.transformation_model.translation.x <= model_armadillo7.transformation_model.translation.x+taille_hitbox && test_draw_7 == 1) {
      if (model_armadillo7.transformation_model.translation.y-taille_hitbox <= model_balle3.transformation_model.translation.y && model_balle3.transformation_model.translation.y <= model_armadillo7.transformation_model.translation.y+taille_hitbox){
        //Armadillo7 touché
        test_draw_7 = 0;
        score += 175;
      }
    }
    if (model_armadillo8.transformation_model.translation.x-taille_hitbox <= model_balle3.transformation_model.translation.x && model_balle3.transformation_model.translation.x <= model_armadillo8.transformation_model.translation.x+taille_hitbox && test_draw_8 == 1) {
      if (model_armadillo8.transformation_model.translation.y-taille_hitbox <= model_balle3.transformation_model.translation.y && model_balle3.transformation_model.translation.y <= model_armadillo8.transformation_model.translation.y+taille_hitbox){
        //Armadillo8 touché
        test_draw_8 = 0;
        score += 175;
      }
    }
    model_balle3.transformation_model.translation.z = 0.0f;
    tir3 = 0;
  }
}

void enemy_bullet_collision() {
  float taille_hitbox = 0.08f;
  if (model_balleEnnemie1.transformation_model.translation.z >= -0.8f) {
    if (model_dinosaure.transformation_model.translation.x-taille_hitbox <= model_balleEnnemie1.transformation_model.translation.x && model_balleEnnemie1.transformation_model.translation.x <= model_dinosaure.transformation_model.translation.x+taille_hitbox) {
      if (model_dinosaure.transformation_model.translation.y-taille_hitbox <= model_balleEnnemie1.transformation_model.translation.y && model_balleEnnemie1.transformation_model.translation.y <= model_dinosaure.transformation_model.translation.y+taille_hitbox){
        //Joueur touché = fin de partie
        game_over();
      }
    }
  if (model_balleEnnemie2.transformation_model.translation.z >= -0.8f) {
      if (model_dinosaure.transformation_model.translation.y-taille_hitbox <= model_balleEnnemie2.transformation_model.translation.y && model_balleEnnemie2.transformation_model.translation.y <= model_dinosaure.transformation_model.translation.y+taille_hitbox){
        //Joueur touché = fin de partie
        game_over();
      }
    }
  }
  if (model_balleEnnemie3.transformation_model.translation.z >= -0.8f) {
    if (model_dinosaure.transformation_model.translation.x-taille_hitbox <= model_balleEnnemie3.transformation_model.translation.x && model_balleEnnemie3.transformation_model.translation.x <= model_dinosaure.transformation_model.translation.x+taille_hitbox) {
      if (model_dinosaure.transformation_model.translation.y-taille_hitbox <= model_balleEnnemie3.transformation_model.translation.y && model_balleEnnemie3.transformation_model.translation.y <= model_dinosaure.transformation_model.translation.y+taille_hitbox){
        //Joueur touché = fin de partie
        game_over();
      }
    }
  }
  if (model_balleEnnemie4.transformation_model.translation.z >= -0.8f) {
    if (model_dinosaure.transformation_model.translation.x-taille_hitbox <= model_balleEnnemie4.transformation_model.translation.x && model_balleEnnemie4.transformation_model.translation.x <= model_dinosaure.transformation_model.translation.x+taille_hitbox) {
      if (model_dinosaure.transformation_model.translation.y-taille_hitbox <= model_balleEnnemie4.transformation_model.translation.y && model_balleEnnemie4.transformation_model.translation.y <= model_dinosaure.transformation_model.translation.y+taille_hitbox){
        //Joueur touché = fin de partie
        game_over();
      }
    }
  }
}

void game_over() {
  test_draw_player = 0;
  tirEnnemi1 = 0;
  tirEnnemi2 = 0;
  tirEnnemi3 = 0;
  tirEnnemi4 = 0;
  //afficher game over
}