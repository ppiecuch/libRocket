
#include "OpenGL.h"
#include "GLGrid.h"

//
// ++==+==+==++==+==+==++
// ||  |  |  ||  |  |  ||
// ||  |  |  ||  |  |  ||
// ++--+--+--++--+--+--++
// ||  |  |  ||  |  |  ||
// ||  |  |  ||  |  |  ||
// ++==+==+==++==+==+==++
// ||  |  |  ||  |  |  ||
// ||  |  |  ||  |  |  ||
// ++--+--+--++--+--+--++
// ||  |  |  ||  |  |  ||
// ||  |  |  ||  |  |  ||
// ++==+==+==++==+==+==++
//

void RenderGrid(int main_window_width, int main_window_height, float scale, int rows, int columns, int subdivs, bool bg) {

    GLboolean isDepth = glIsEnabled(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);

    /* background gradient */
    glBegin(GL_QUADS);
    {
        glColor3ub(194, 227, 253);
        glVertex2f(0, 0); 
        glVertex2f(main_window_width, 0);
        glColor3ub(243, 250, 255);
        glVertex2f(main_window_width, main_window_height);
        glVertex2f(0, main_window_height);
    }
    glEnd();

    /* Render grid over 0..rows, 0..columns. */
    GLboolean isBlend = glIsEnabled(GL_BLEND);
    glEnable(GL_BLEND);

    /* Subdivisions */    
    glColor4f(0, 0, 0, 0.25);
    // glLineStipple(0, 0x8888); glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    {
        /* Horizontal lines. */
        float stepx = float(main_window_height) / float(rows*subdivs);
        for (int i=0; i<=rows*subdivs; i++) {
            glVertex2f(0, i*stepx);
            glVertex2f(main_window_width, i*stepx); 
        }
        
        /* Vertical lines. */
        float stepy = float(main_window_width) / float(columns*subdivs);
        for (int i=0; i<=columns*subdivs; i++) {
            glVertex2f(i*stepy, 0);
            glVertex2f(i*stepy, main_window_height); 
        }
    }
    glEnd(); glDisable(GL_LINE_STIPPLE);

    /* Regular grid */
    glColor4f(0, 0, 0, 0.25);
    glBegin(GL_LINES);
    {
        /* Horizontal lines. */
        float stepx = float(main_window_height) / float(rows);
        for (int i=0; i<=rows; i++) {
            glVertex2f(0, i*stepx);
            glVertex2f(main_window_width, i*stepx); 
        }
        
        /* Vertical lines. */
        float stepy = float(main_window_width) / float(columns);
        for (int i=0; i<=columns; i++) {
            glVertex2f(i*stepy, 0);
            glVertex2f(i*stepy, main_window_height); 
        }
    }
    glEnd();
    /* Borders */
    glColor4f(0, 0, 0, 0.2);
    glLineWidth(2.5); glBegin(GL_LINES);
    {
        { glVertex2f(0, 0); glVertex2f(main_window_width, 0); }
        { glVertex2f(main_window_width, 0); glVertex2f(main_window_width, main_window_height); }
        { glVertex2f(main_window_width, main_window_height); glVertex2f(0, main_window_height); }
        { glVertex2f(0, main_window_height); glVertex2f(0, 0); }
        { glVertex2f(main_window_width/2, 0); glVertex2f(main_window_width/2, main_window_height); }
        { glVertex2f(0, main_window_height/2); glVertex2f(main_window_width, main_window_height/2); }
    }
    glEnd();
    
    glLineWidth(1.0); glColor4f(1, 1, 1, 1);
    if (!isBlend) glDisable(GL_BLEND);
    if (isDepth) glEnable(GL_DEPTH_TEST);
}

