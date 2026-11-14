#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include "OpenGL445Setup-2026.h"


/* Graphics Pgm 2 for Brian Clark
   EXTRA CREDIT: I have included some "Extra" options available for the 445 version of
                 this class:
                 1. When the car comes within 5 units of the edge of the canvas
                    it turns to face the other direction, if needed.
                 2. Car Color:  The car changes color when the c key is pressed.
                 3. Meteor spin:  The meteor spins as it falls if enabled by
                    the s key.

   Program Structure:

   Global Variables:  Position, orientation and state for the meteor and car
       are tracked using global variables which appear at the top of the code.
       Generally meteor variables begin "g_met" and car variables begin "g_car".

    Display:  The central display event is handled by display_func() which
       calls draw_car() draw_meteor() and drawText() to handle the distinct
       objects being displayed. The car is drawn by playing a display list 
       recorded by create_car_display_list() early in the execution. 

       Objects are positioned using legacy OpenGL transformations and the
       stack is used to return to the baseline. 

    Animation: Movement is accomplished using timer callbacks to timer_func(),
       which handles car (value 2) and meteor (value 1)  movement separately 
       to meet timing requirements. Car rotation is marshalled by keyboard_func()
       but executed in timer_func().

    Keyboard event: The keyboard_func() handles all keypresses, which start
       meteor movement, turn meteor spin on and off, change car color, and 
       move the car to new position and define edge of canvas rotation.


*/
// globals for meteorite movement
float g_met_x;	  // the current x position of the meteor's center 
float g_met_y;    // the current y position of the meteor's center
float g_met_z = -25.0f;    // the current z position of the meteor's center
bool g_met_is_spinning = false;
float g_met_rot = 0.0f;

float g_N_line;

int g_frame_timeout = 25;     // msec value for  meteor timer - target range 15-35   25

float g_fall_rate = 5.0f;        // units of movement per frame, range 3-10   5

bool g_is_falling = false;    // a flag to turn falling on

// globals for car movement
int g_car_dl;       // the display list handle
float g_car_x = 123.5f;     // car x position
float g_car_y = 38.0f;      // car y position

float g_car_move_rate = 4.0f;   // car rate of movement

float g_car_rot = 0.0f;     // car current rotational position
float g_car_rot_rate = 10.0f;   // rate of car rotation
float g_car_rot_target = 0.0f;  // car - intended rotation end point

bool g_is_car_turning = false;  // flag to turn rotation on
int g_car_rot_timeout = 55;     // car timeout

bool g_alternate_color = false; // flag to manage color changes

float left_car_limit = 0.0f;    // x to trigger car rotation on left
float right_car_limit = 0.0f;   // x to trigger car rogation on right

void timer_func(int item)
{
    // timer callback function, to progress the animation 

    // handle meteor animation tasks
    if (item == 1)
    {
        if (g_is_falling) // don't animate until the falling flag is set
        {
            if (g_met_y > 24.97f)  // only fall until we hit the bottom
            {
                g_met_y -= g_fall_rate;  // fall
                if (g_met_y < 24.97f)
                {
                    g_met_y = 24.97f;    // correct if we've fallen below bottom
                }
                // Extra Credit:  Meteor Rotation
                if (g_met_is_spinning) {
                    g_met_rot += 10.0f;
                }
                glutPostRedisplay();	// if we've changed anything, display it
            }
        }
        // set a timeout for the next interval
        glutTimerFunc(g_frame_timeout, timer_func, 1);
    }

    // handle car animation tasks
    if (item == 2)
    {
        // Car rotation still required
        if (g_car_rot != g_car_rot_target) {
            g_car_rot += g_car_rot_rate;    // modify car rotational state by rate
            // check to see if we've gone too far
            if ((g_car_rot_rate < 0 && g_car_rot_target > g_car_rot)
                || (g_car_rot_rate > 0 && g_car_rot_target < g_car_rot))
            {
                g_car_rot = g_car_rot_target;  // correct rotational state
                g_is_car_turning = false;      // turn off the rotation flag
            }
            glutPostRedisplay();
            glutTimerFunc(g_car_rot_timeout, timer_func, 2);    // trigger next rotation
        }
        else
        {
            // no need for further callbacks
            g_is_car_turning = false;
        }

    }
}

int create_car_display_list()
{
    // create the car display list
    int car_dl = glGenLists(1);

    glNewList(car_dl, GL_COMPILE);
    float zpos = 20.0;
    glBegin(GL_LINES);

    // draw the car at the origin
    // wheel1
    glVertex3f(-55.5, -37.0, zpos);
    glVertex3f(-48.5, -37.0, zpos);

    glVertex3f(-48.5, -37.0, zpos);
    glVertex3f(-48.5, -30.0, zpos);

    glVertex3f(-48.5, -30.0, zpos);
    glVertex3f(-55.5, -30.0, zpos);

    glVertex3f(-55.5, -30.0, zpos);
    glVertex3f(-55.5, -37.0, zpos);
    //wheel2
    glVertex3f(48.5, -37.0, zpos);
    glVertex3f(55.5, -37.0, zpos);

    glVertex3f(55.5, -37.0, zpos);
    glVertex3f(55.5, -30.0, zpos);

    glVertex3f(55.5, -30.0, zpos);
    glVertex3f(48.5, -30.0, zpos);

    glVertex3f(48.5, -30.0, zpos);
    glVertex3f(48.5, -37.0, zpos);
    // body
    glVertex3f(-73.5, -30.0, zpos);
    glVertex3f(73.5, -30.0, zpos);

    glVertex3f(73.5, -30.0, zpos);
    glVertex3f(-73.5, 30.0, zpos);

    glVertex3f(-73.5, 30.0, zpos);
    glVertex3f(-73.5, -30.0, zpos);
    // cabin
    glVertex3f(4.5, -2.5, zpos);
    glVertex3f(0.0, 11.5, zpos);

    glVertex3f(0.0, 11.5, zpos);
    glVertex3f(-14.5, 5.5, zpos);


    // draw the car again

    float oldZpos = zpos;
    zpos = oldZpos - 40.0f;

    // wheel1
    glVertex3f(-55.5, -37.0, zpos);
    glVertex3f(-48.5, -37.0, zpos);

    glVertex3f(-48.5, -37.0, zpos);
    glVertex3f(-48.5, -30.0, zpos);

    glVertex3f(-48.5, -30.0, zpos);
    glVertex3f(-55.5, -30.0, zpos);

    glVertex3f(-55.5, -30.0, zpos);
    glVertex3f(-55.5, -37.0, zpos);
    //wheel2
    glVertex3f(48.5, -37.0, zpos);
    glVertex3f(55.5, -37.0, zpos);

    glVertex3f(55.5, -37.0, zpos);
    glVertex3f(55.5, -30.0, zpos);

    glVertex3f(55.5, -30.0, zpos);
    glVertex3f(48.5, -30.0, zpos);

    glVertex3f(48.5, -30.0, zpos);
    glVertex3f(48.5, -37.0, zpos);
    // body
    glVertex3f(-73.5, -30.0, zpos);
    glVertex3f(73.5, -30.0, zpos);

    glVertex3f(73.5, -30.0, zpos);
    glVertex3f(-73.5, 30.0, zpos);

    glVertex3f(-73.5, 30.0, zpos);
    glVertex3f(-73.5, -30.0, zpos);
    // cabin
    glVertex3f(4.5, -2.5, zpos);
    glVertex3f(0.0, 11.5, zpos);

    glVertex3f(0.0, 11.5, zpos);
    glVertex3f(-14.5, 5.5, zpos);


    // link the two sides
        // body
    glVertex3f(-73.5, -30.0, oldZpos);
    glVertex3f(-73.5, -30.0, zpos);

    glVertex3f(73.5, -30.0, oldZpos);
    glVertex3f(73.5, -30.0, zpos);

    glVertex3f(-73.5, 30.0, oldZpos);
    glVertex3f(-73.5, 30.0, zpos);
    // cabin
    glVertex3f(4.5, -2.5, oldZpos);
    glVertex3f(4.5, -2.5, zpos);
    glVertex3f(0.0, 11.5, oldZpos);
    glVertex3f(0.0, 11.5, zpos);
    glVertex3f(-14.5, 5.5, oldZpos);
    glVertex3f(-14.5, 5.5, zpos);


    glEnd();
    glEndList();


    return car_dl;
}

void keyboard_func(unsigned char val, int x, int y)
{
    // handle keyboard input

    // we actually don't care what value key has been pressed
    // to turn meteor movement on
    if (val == 'm' || val == 'M')
    {
        g_is_falling = true;
    }

    // move car to the right on K
    if (val == 'k' || val == 'K') {
        g_car_x += g_car_move_rate;

        // Extra Credit - Cause car to spin 180 degrees when within five units
        // of the edge of the canvas
        if (g_car_x >= right_car_limit && g_is_car_turning == false)
        {
            g_is_car_turning = true;
            g_car_rot_target = 180.0f;
            g_car_rot_rate = 10.0f;
            glutTimerFunc(g_car_rot_timeout, timer_func, 2);
        }

        glutPostRedisplay();
    }
    // move car to the left on J
    if (val == 'j' || val == 'J') {
        g_car_x -= g_car_move_rate;

        // Extra Credit - Cause car to spin 180 degrees when within five units
         // of the edge of the canvas
        if (g_car_x <= left_car_limit && g_is_car_turning == false)
        {
            g_is_car_turning = true;
            g_car_rot_target = 0.0f;
            g_car_rot_rate = -10.0f;
            glutTimerFunc(g_car_rot_timeout, timer_func, 2);
        }

        glutPostRedisplay();
    }

    // Extra credit - Meteor spin
    if (val == 's' || val == 'S')
    {
        // a simple toggle on the flag
        g_met_is_spinning = !g_met_is_spinning;
    }
    // Extra credit - Car Color
    if (val == 'c' || val == 'C') {
        // a toggle the color flag
        g_alternate_color = !g_alternate_color;
        glutPostRedisplay();        // show immediately
    }



}

// handles meteor drawing process
void draw_meteor(float x, float y, float z)
{
    glPushMatrix();     // put base state on the stack
    glTranslatef(x, y, z);  // move origin to coordinates
    glRotatef(g_met_rot, 0.0, 1.0, 0.0);   // set rotational position.
    glScalef(24.0f, 24.0f, 24.0f);  // expand to meet requirements


    glColor3f(1.0f, 1.0f, 1.0f);    // draw in white

    glutWireOctahedron();           // use glut octahedron

    glPopMatrix();     // go back to base state
}

// handles car drawing process
void draw_car()
{
    // use glRotate to change the world. Then pop it back at the end.
    glPushMatrix();
    // translate to car position
    glTranslatef(g_car_x, g_car_y, -30);
    // set car rotation
    glRotatef(g_car_rot, 0.0, 1.0, 0.0);

    // Extra credit:  Allow configurable car color
    if (g_alternate_color) {
        // set pen color to gold
        glColor3f(0.645f, 0.582f, 0.375f);
    }
    else
    {
        // set pen color to red
        glColor3f(0.926f, 0.102f, 0.219f);
    }

    // execute the display list to draw the car
    glCallList(g_car_dl);

    glPopMatrix();  // return to base state
}

void drawText()
{

    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2i(250, 320);    // position raster origin point
    // display each letter.
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'P');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 's');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 's');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'M');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 't');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'o');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'S');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 't');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 't');

}

void display_func()
{
    /* this callback is automatically called whenever a window needs to
      be displayed or redisplayed.

      Usually this function contains nearly all of the drawing commands;
      the drawing/animation statments should go in this function.    */

    float zpos = -10.0; // this zpos will be used by everything

    glClearColor(0.172549f, 0.164706f, 0.160784f, 1.0f);   // UAH Black - As % values
    glClear(GL_COLOR_BUFFER_BIT);

    // display text until the meteor starts falling
    if (g_is_falling == false) {
        drawText();
    }

    // draw the meteor
    draw_meteor(g_met_x, g_met_y, g_met_z);

    // draw the car
    draw_car();

    glFlush();  // may need to uncomment this line!

}  // end of diplay_func()



#define canvas_Width 640
#define canvas_Height 640
char canvas_Name[] = "CS 445 - Program 2 - Meteor Dodge Game Core";

// #define canvas_name "CS 445 - Program 1 - Falling";


int main(int argc, char** argv)
{
    // This is our startup function, does some simple init
    // Also, it configures the callback functions we will be using.	

    // setup bolierplate
    glutInit(&argc, argv);
    my_setup(canvas_Width, canvas_Height, canvas_Name);

    // create a display list for the car
    g_car_dl = create_car_display_list();

    // initial meteor position - sets the x and y of the leftmost corner
    // relative to the canvas size.
    g_met_x = canvas_Width - 95 + 24;	   // My surname starts with C, so N = 95 units 
    g_met_y = canvas_Height - 26.97f;  // magic number: see draw_meteor()


    // determine left and right limits of car motion
    left_car_limit = 78.5;
    right_car_limit = canvas_Width - 78.5;

    g_N_line = canvas_Width - 95;

    // display function callback - draws EVERYTHING
    glutDisplayFunc(display_func);
    // keyboard callback - handles the one and only input event that matters
    glutKeyboardFunc(keyboard_func);
    // timer callback function - this drives our animation by updating
    // the "state" of our globals (ie. y coordinate) for the meteor
    glutTimerFunc(g_frame_timeout, timer_func, 1);  // use global timeout


    //glutTimerFunc(g_frame_timeout, timer_func, 2);

    // write message to user
    std::cout << "Any Key Click Will Start" << std::endl;

    glutMainLoop();
    return 0;
}
