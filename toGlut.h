#ifndef TOGLUT_H_INCLUDED
#define TOGLUT_H_INCLUDED

/*
 * GLUT Shapes Demo
 *
 * Written by Nigel Stewart November 2003
 *
 * This program is test harness for the sphere, cone
 * and torus shapes in GLUT.
 *
 * Spinning wireframe and smooth shaded shapes are
 * displayed until the ESC or q key is pressed.  The
 * number of geometry stacks and slices can be adjusted
 * using the + and - keys.
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
//#include <GL/glut.h>
#include <GL/freeglut.h>
#endif

#include "bmpfont.h"
#include "palettes.h"
#include <stdlib.h>
#include <unistd.h>


bool bQuit = false;
bool showContacts = true;
UINT step = 1;

borderLine bl;
std::string fout = "";


void addGlutRectangle(std::vector<point> p, rgb color = {1, 0.5, 0.5}){
    glBegin (GL_LINE_LOOP);
    glColor3f(color.red, color.green, color.blue);
    point p0;
    p0.x = p[0].x;
    p0.y = p[0].y;
    glVertex2f(p0.x, p0.y);
    p0.x = p[1].x;
    p0.y = p[0].y;
    glVertex2f(p0.x, p0.y);
    p0.x = p[1].x;
    p0.y = p[1].y;
    glVertex2f(p0.x, p0.y);
    p0.x = p[0].x;
    p0.y = p[1].y;
    glVertex2f(p0.x, p0.y);
    p0.x = p[0].x;
    p0.y = p[0].y;
    glVertex2f(p0.x, p0.y);
    glEnd();
    return;
  }

  std::vector<point> glCircle(float x, float y, float r)
  {
    borderLine dummy;
      point temp;
      std::vector<point> result;
      float sen = 0.7071067811f;
      temp.x = x;
      temp.y = y + r;
      result.insert(result.end(), temp);
      temp.x = x + sen * r;
      temp.y = y + sen * r;
      result.insert(result.end(), temp);
      temp.x = x + r;
      temp.y = y;
      result.insert(result.end(), temp);
      temp.x = x + sen * r;
      temp.y = y - sen * r;
      result.insert(result.end(), temp);
      temp.x = x ;
      temp.y = y - r;
      result.insert(result.end(), temp);
      temp.x = x - sen * r;
      temp.y = y - sen * r;
      result.insert(result.end(), temp);
      temp.x = x - r;
      temp.y = y;
      result.insert(result.end(), temp);
      temp.x = x - sen * r;
      temp.y = y + sen * r;
      result.insert(result.end(), temp);
      return result;
  }


  void toOGL()
  {
      UINT i, j;
      point P;     //coordinates
      std::vector<point> temp; //stores perimeters
      std::vector<std::vector<point>> blp = bl.getBl();
      bool showPoints = bl.doIShowThis();
      glClearColor (1.0f, 1.0f, 1.0f, 0.0f);
      glClear (GL_COLOR_BUFFER_BIT);
      //define openGL scale

      scale ogl;
      palettes p;
      std::vector<UINT> colors = p.getPalette();
      //addRectangle(bl, ogl, bl.getBoundaries(2 * bl.maxRad()));
      ogl.initScale();
      //define vectors
      std::vector<point> attn = bl.getWarnings();
      //if (bl.blSettings.doCheckTopol == true){
        for (i = 0; i < blp.size(); i++)
        {
            //attention(blp[i][0].x, blp[i][0].y);
            //attention(bl[i][bl[i].size()-1].x, bl[i][bl[i].size()-1].y, 0.1);
            glBegin (GL_LINE_LOOP);
            rgb c = p.toRGB(colors[i]);
            glColor3f (c.red / 255, c.green/255, c.blue/255);
            for (j = 0; j < blp[i].size(); j++)
            {
                P = bl.place(ogl, blp[i][j]);
                glVertex2f (P.x, P.y);
            }
            glEnd ();
        }
        glBegin (GL_LINE_LOOP);

        glEnd ();
        if (showPoints){
          for (i = 0; i < blp.size(); i++){
            for (j = 0; j < blp[i].size(); j++){
              P = bl.place(ogl, blp[i][j]);
              glBegin (GL_LINE_LOOP);
              std::vector<point> cr = glCircle(P.x, P.y, 0.01);
              for (UINT k = 0; k < cr.size(); k++)
              {
                  glVertex2f (cr[k].x, cr[k].y);
              }
              glEnd ();
            }
          }

        }
        /**********
      for (UINT i = 0; i < bl.ngroups; i++){
        UINT n = twoPow(i);
        std::vector<point> q = bl.getSetBoundaries(n, 2*bl.maxRadius*AIR, true);
        addRectangle(bl, ogl, q, bl.colors[i]);
      }
      *********/
      //std::vector<point> bnd = bl.getBoundaries(bl.maxRadius, true);
      //addRectangle(bl, ogl, bnd);
      if (showContacts){
        for (i = 0; i < attn.size();  i++)
        {
            P = bl.place(ogl, attn[i]);
            //point tp;
            /*tp.x = warn[i].fx;
            tp.y = warn[i].fy;
            point P2;
            P2 = place(ogl, tp);
            glBegin (GL_LINES);
                glColor3f (0.0f, 1.0f, 0.0f);
                glVertex2f (P.x, P.y);
                glVertex2f (P2.x,P2.y);
            glEnd ();
            */
            temp = glCircle(P.x, P.y, P.radius);
            glBegin (GL_LINE_LOOP);
            glColor3f (0.0f, 1.0f, 0.0f);
            for (j = 0; j < temp.size(); j++)
            {
                glVertex2f (temp[j].x, temp[j].y);
            }
            glEnd ();
          }
      }

        bl.clearWarnings();
      //}
      //else{

        for (UINT i = 0; i < bl.circles.size() -1; i++)
        {
          if (bl.circles[i].radius > 0){
              //attention(blp[i][0].x, blp[i][0].y);
              //attention(bl[i][bl[i].size()-1].x, bl[i][bl[i].size()-1].y, 0.1);

               //Show forces
               //-----------
               /**********
              glBegin (GL_LINES);

              point t = showForce(bl.circles[i], 0.01);

              P = bl.place(ogl, bl.circles[i]);
              glVertex2f(P.x, P.y);
              P = bl.place(ogl, t);
              glVertex2f(P.x, P.y);
              glEnd ();
              *********/
              //-----------
              /*
              // Show relationships
              for (j = 0; j < bl.circles.size(); j++)
              {
                if (bl.circles[j].radius > 0){
                  for (UINT group = 0; group < bl.ngroups; group++){
                    UINT mask = 1 << group;
                    glColor3f (bl.colors[group].red, bl.colors[group].green, bl.colors[group].blue);
                    if ((bl.circles[i].n & mask) > 0 && (bl.circles[j].n & mask) > 0){
                      glLineWidth(bl.getRelationships(i, j));
                      glBegin (GL_LINES);
                      P = bl.place(ogl, bl.circles[i]);
                      glVertex2f(P.x, P.y);
                      P = bl.place(ogl, bl.circles[j]);
                      glVertex2f(P.x, P.y);
                      glEnd ();
                    }
                  }
                }
              }*/

              glLineWidth(1);
          }
        //}

      }


      for (i = 0; i < bl.circles.size();  i++)
      {
          P = bl.place(ogl, bl.circles[i]);
          temp = glCircle(P.x, P.y, P.radius);
          if ((bl.circles[i].flags & IS_OUTSIDE) > 0){
            glColor3f (0.0f, 1.0f, 0.0f);
          }
          else{
            glColor3f (1.0f, 0.0f, 0.0f);
          }
          glBegin (GL_LINE_LOOP);
          for (j = 0; j < temp.size(); j++)
          {
              glVertex2f (temp[j].x, temp[j].y);
          }
          glEnd ();
      }
      // Text
      glColor3f(0.0f, 0.0f, 1.0f);
      float yd = 0.8f;
      for (i = 0; i < bl.dataDisplay.size(); i++){
        std::string mymsg = bl.dataDisplay[i];
        //showText(mymsg); exit(0);
        glRasterPos2f(-0.9f, yd);
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        const unsigned char* t = reinterpret_cast<const unsigned char *>(mymsg.c_str());
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, t);
        yd -= 0.1;
      }
      /*for (i = 0; i < bl.dataDisplay.size(); i++){
        free(bl.dataDisplay[i]);
      }
      bl.dataDisplay.clear();
      //glFlush();
      **********/
      //glutSwapBuffers();
      /*********DEBUG**/
      if (attn.size() > 0){
        //wait();
      }
      /*********DEBUG***/
      //Sleep(1);
  }



/* GLUT callback Handlers */

static void resize(int width, int height)
{

    glViewport(0, 0, width, height);

}

static void display(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    bl.setCycle(step);
    toOGL();
    //bl.showCircleNumbers(false);
    //bl.returnError(20);
    bl.setFontSize(10);
    if (bl.isStepFinished(step)){
        std::stringstream txt;
        //txt << "/home/vqf/web/steps/v3/step" << step << ".svg";
        //bl.writeSVG(txt.str());
        step++;
        bl.setStep(step);
        std::cout << step << std::endl;
        if (step > 8){
          //if (bl.err()){
            std::cout << bl.errorMsg() << std::endl;
          //}
          exit(0);
        }
    }
    sleep(0);
    //glFlush();
    glutSwapBuffers();

}


static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27 :
        case 'q':
            if (step < 8){
                step++;
                bl.setStep(step);
                std::cout << step << std::endl;
            }
            else if (step == 8){
                bl.writeHTML(fout);
                //std::cout << bl.croack() << std::endl;
                //bl.reset();
                //std::cout << bl.croack() << std::endl;
                step++;
            }
            else{
                exit(0);
            }
            break;

        case '+':

            break;

        case '-':
            break;
    }

    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

/* Program entry point */



void glutSimulate(std::string filepath, UINT bycol = 0, const char lineSep = 0x00, std::string fname = "nvenn.txt", std::string outputFile = "result.html"){
    bl = fromSetFile(filepath, bycol);
    fout = filepath + ".html";
}

int initGlut(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(1200,1200);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB  | GLUT_DEPTH);

    glutCreateWindow("nVenn2");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

//    glXSwapIntervalEXT(0);

    glutMainLoop();

    return EXIT_SUCCESS;
}



#endif // TOGLUT_H_INCLUDED
