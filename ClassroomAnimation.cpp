#include <GL/glut.h>
#include <cmath>
#include <string>
#include <ctime>
#include <vector>

// Camera settings
float cameraAngle = 0.0f; // Angle for camera rotation around the scene
float cameraHeight = 5.0f;
float cameraDistance = 15.0f;

// Lecturer position
float lecturerX = 0.0f;
float lecturerZ = -5.0f;

// Student position
float studentX = 0.0f;
float studentZ = 0.0f;

// Animation variables
float lecturerArmAngle = 0.0f;
bool armDirection = true;

// Clock variables
bool showClock = true; // Toggle clock visibility
float clockX = 7.0f;   // Clock position on the wall
float clockY = 4.0f;
float clockZ = -9.5f;

// Projector screen variables
bool projectorOn = false; // Toggle projector screen
float screenX = -7.0f;    // Projector screen position
float screenY = 2.5f;
float screenZ = -9.5f;

// Door variables
float doorAngle = 0.0f; // Door rotation angle
bool doorOpen = false;  // Door state (open/closed)

// Curtain variables
float curtainAngle = 0.0f; // Curtain rotation angle
bool curtainsOpen = false; // Curtain state (open/closed)

// Projector position and light source
float projectorX = -7.0f; // Projector position
float projectorY = 5.0f;
float projectorZ = -8.0f;

float lightPos[] = { projectorX, projectorY, projectorZ, 1.0f }; // Light position (attached to projector)
float lightDir[] = { 0.0f, -1.0f, -0.5f }; // Light direction (towards the screen)

bool projectorLightOn = false; // Toggle projector light
bool classroomLightOn = false; // Toggle classroom light

// Automatic movement variables
bool autoMove = false; // Toggle automatic movement
float moveSpeed = 0.02f; // Speed of automatic movement

// Student exit/enter variables
bool studentsExit = false; // Toggle students exiting
bool studentsEnter = false; // Toggle students entering
int currentExitingStudent = 0; // Index of the student currently exiting
int currentEnteringStudent = 0; // Index of the student currently entering
std::vector<std::pair<float, float>> studentPositions; // Store initial student positions
std::vector<std::pair<float, float>> studentTargetPositions; // Store target positions for students

// Function to draw a simple chair
void drawChair() {
    // Seat
    glColor3f(0.5f, 0.35f, 0.05f); // Brown color
    glPushMatrix();
    glScalef(1.0f, 0.1f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Backrest
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, -0.5f);
    glScalef(1.0f, 1.0f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Function to draw a simple desk
void drawDesk() {
    glColor3f(0.3f, 0.2f, 0.1f); // Dark brown color
    glPushMatrix();
    glScalef(2.0f, 0.1f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Function to draw a simple student
void drawStudent(float x, float z) {
    glColor3f(0.0f, 0.0f, 1.0f); // Blue color
    glPushMatrix();
    glTranslatef(x, 0.5f, z);
    glutSolidSphere(0.3f, 20, 20); // Head
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x, 0.2f, z);
    glScalef(0.5f, 1.0f, 0.5f);
    glutSolidCube(0.5f); // Body
    glPopMatrix();
}

// Function to draw the lecturer
void drawLecturer() {
    glColor3f(1.0f, 0.0f, 0.0f); // Red color
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glutSolidSphere(0.4f, 20, 20); // Head
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.2f, 0.0f);
    glScalef(0.6f, 1.2f, 0.6f);
    glutSolidCube(0.6f); // Body
    glPopMatrix();

    // Arms
    glPushMatrix();
    glTranslatef(-0.4f, 0.5f, 0.0f);
    glRotatef(lecturerArmAngle, 1.0f, 0.0f, 0.0f);
    glScalef(0.2f, 0.8f, 0.2f);
    glutSolidCube(0.5f); // Left arm
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.4f, 0.5f, 0.0f);
    glRotatef(-lecturerArmAngle, 1.0f, 0.0f, 0.0f);
    glScalef(0.2f, 0.8f, 0.2f);
    glutSolidCube(0.5f); // Right arm
    glPopMatrix();
}

// Function to draw the whiteboard with a grey border
void drawWhiteboard() {
    // Draw the grey border
    glColor3f(0.5f, 0.5f, 0.5f); // Grey color
    glPushMatrix();
    glTranslatef(0.0f, 2.5f, -9.5f); // Position at the front of the classroom
    glScalef(6.0f, 4.0f, 0.2f); // Size of the border
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw the whiteboard inside the border
    glColor3f(1.0f, 1.0f, 1.0f); // White color
    glPushMatrix();
    glTranslatef(0.0f, 2.5f, -9.4f); // Slightly in front of the border
    glScalef(5.8f, 3.8f, 0.1f); // Slightly smaller than the border
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Function to draw text on the whiteboard
void drawText(float x, float y, float z, const std::string& text) {
    glColor3f(0.0f, 0.0f, 0.0f); // Black color for text
    glRasterPos3f(x, y, z); // Position the text

    // Draw each character of the string
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c); // Use Helvetica 18 font
    }
}

// Function to draw a digital clock
void drawClock() {
    if (!showClock) return; // Skip if clock is hidden

    // Get current time
    time_t now = time(0);
    tm* localTime = localtime(&now);
    char timeStr[9];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", localTime);

    // Draw the clock background
    glColor3f(0.2f, 0.2f, 0.2f); // Dark grey color
    glPushMatrix();
    glTranslatef(clockX, clockY, clockZ);
    glScalef(2.0f, 1.0f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw the time
    glColor3f(1.0f, 1.0f, 1.0f); // White color
    drawText(clockX - 0.8f, clockY, clockZ + 0.1f, timeStr);
}

// Function to draw the projector structure
void drawProjector() {
    // Projector body (a box)
    glColor3f(0.3f, 0.3f, 0.3f); // Dark grey color
    glPushMatrix();
    glTranslatef(projectorX, projectorY, projectorZ);
    glScalef(0.5f, 0.3f, 0.5f); // Size of the projector
    glutSolidCube(1.0f);
    glPopMatrix();

    // Projector lens (a small sphere)
    glColor3f(0.1f, 0.1f, 0.1f); // Black color
    glPushMatrix();
    glTranslatef(projectorX, projectorY - 0.1f, projectorZ - 0.3f);
    glutSolidSphere(0.1f, 20, 20); // Lens
    glPopMatrix();
}

// Function to draw a projector screen
void drawProjectorScreen() {
    // Set material properties for the screen
    float matAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f }; // Light grey
    float matDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f }; // Light grey
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);

    glColor3f(0.8f, 0.8f, 0.8f); // Light grey color
    glPushMatrix();
    glTranslatef(screenX, screenY, screenZ);
    glScalef(4.0f, 3.0f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    if (projectorOn) {
        // Draw "Projector On" text
        glColor3f(0.0f, 0.0f, 0.0f); // Black color
        glPushMatrix();
        glTranslatef(screenX, screenY, screenZ + 0.1f);
        glRotatef(-cameraAngle * 180 / M_PI, 0.0f, 1.0f, 0.0f); // Rotate text to face the camera
        drawText(-1.5f, 1.5f, 0.0f, "Projector On");
        glPopMatrix();
    }
}

// Function to draw a door
void drawDoor() {
    // Draw the door frame
    glColor3f(0.4f, 0.2f, 0.1f); // Dark brown color for the frame
    glPushMatrix();
    glTranslatef(-9.5f, 2.5f, 0.0f);
    glScalef(0.2f, 5.2f, 2.2f); // Slightly larger than the door
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw the door
    if (doorOpen) {
        glColor3f(0.7f, 0.7f, 0.9f); // Light blue color for the door when open
    } else {
        glColor3f(0.6f, 0.4f, 0.2f); // Light brown color for the door when closed
    }
    glPushMatrix();
    glTranslatef(-9.5f, 2.5f, 0.0f); // Position on the left wall
    glRotatef(doorAngle, 0.0f, 1.0f, 0.0f); // Rotate the door
    glScalef(0.1f, 5.0f, 2.0f); // Size of the door
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw a black quad over the door when closed
    if (!doorOpen) {
        glColor3f(0.0f, 0.0f, 0.0f); // Black color
        glBegin(GL_QUADS);
        glVertex3f(-9.5f, 0.0f, -1.0f); // Bottom-left
        glVertex3f(-9.5f, 5.0f, -1.0f); // Top-left
        glVertex3f(-9.5f, 5.0f, 1.0f);  // Top-right
        glVertex3f(-9.5f, 0.0f, 1.0f);  // Bottom-right
        glEnd();
    }
}

// Function to draw windows
void drawWindows() {
    // Left wall window
    if (curtainsOpen) {
        glColor3f(0.7f, 0.7f, 0.9f); // Light blue color for glass
        glPushMatrix();
        glTranslatef(-9.5f, 3.5f, -5.0f);
        glScalef(0.1f, 2.0f, 3.0f);
        glutSolidCube(1.0f);
        glPopMatrix();
    } else {
        // Draw a black quad over the window when curtains are closed
        glColor3f(0.0f, 0.0f, 0.0f); // Black color
        glBegin(GL_QUADS);
        glVertex3f(-9.5f, 2.5f, -6.5f); // Bottom-left
        glVertex3f(-9.5f, 4.5f, -6.5f); // Top-left
        glVertex3f(-9.5f, 4.5f, -3.5f); // Top-right
        glVertex3f(-9.5f, 2.5f, -3.5f); // Bottom-right
        glEnd();
    }

    // Right wall window
    if (curtainsOpen) {
        glColor3f(0.7f, 0.7f, 0.9f); // Light blue color for glass
        glPushMatrix();
        glTranslatef(9.5f, 3.5f, -5.0f);
        glScalef(0.1f, 2.0f, 3.0f);
        glutSolidCube(1.0f);
        glPopMatrix();
    } else {
        // Draw a black quad over the window when curtains are closed
        glColor3f(0.0f, 0.0f, 0.0f); // Black color
        glBegin(GL_QUADS);
        glVertex3f(9.5f, 2.5f, -6.5f); // Bottom-left
        glVertex3f(9.5f, 4.5f, -6.5f); // Top-left
        glVertex3f(9.5f, 4.5f, -3.5f); // Top-right
        glVertex3f(9.5f, 2.5f, -3.5f); // Bottom-right
        glEnd();
    }
}

// Function to draw curtains
void drawCurtains() {
    glColor3f(0.5f, 0.2f, 0.2f); // Red color for curtains

    // Left curtain
    glPushMatrix();
    glTranslatef(-9.5f, 3.5f, -5.0f);
    glRotatef(curtainAngle, 0.0f, 1.0f, 0.0f); // Rotate the curtain
    glScalef(0.1f, 2.0f, 1.5f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Right curtain
    glPushMatrix();
    glTranslatef(-9.5f, 3.5f, -5.0f);
    glRotatef(-curtainAngle, 0.0f, 1.0f, 0.0f); // Rotate the curtain
    glScalef(0.1f, 2.0f, 1.5f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Function to set up lighting
void setupLighting() {
    // Classroom light (GL_LIGHT1)
    if (classroomLightOn) {
        glEnable(GL_LIGHT1); // Enable classroom light

        // Set light properties
        float lightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };  // Ambient light
        float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // Diffuse light (white)
        float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Specular light (white)

        glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);

        // Set light position (above the center of the room)
        float lightPos1[] = { 0.0f, 10.0f, 0.0f, 1.0f }; // Position above the room
        glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    } else {
        glDisable(GL_LIGHT1); // Disable classroom light
    }

    // Projector light (GL_LIGHT0)
    if (projectorLightOn) {
        glEnable(GL_LIGHT0); // Enable projector light

        // Set light properties
        float lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };  // Ambient light
        float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // Diffuse light (white)
        float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Specular light (white)

        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

        // Set light position and direction
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDir);

        // Configure spotlight properties
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0f); // Spotlight cone angle
        glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0f); // Spotlight focus
    } else {
        glDisable(GL_LIGHT0); // Disable projector light
    }
}

// Function to render the classroom scene
void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set the camera
    gluLookAt(cameraDistance * cos(cameraAngle), cameraHeight, cameraDistance * sin(cameraAngle),
              0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    // Set up lighting
    setupLighting();

    // Draw the whiteboard
    drawWhiteboard();

    // Draw text on the whiteboard
    glPushMatrix();
    glTranslatef(0.0f, 2.5f, -9.4f); // Position the text on the whiteboard
    glRotatef(-cameraAngle * 180 / M_PI, 0.0f, 1.0f, 0.0f); // Rotate text to face the camera
    drawText(-2.5f, 0.5f, 0.0f, "Welcome to OpenGL Class!");
    drawText(-2.5f, 0.0f, 0.0f, "Today: 3D Animations");
    glPopMatrix();

    // Draw the clock
    drawClock();

    // Draw the projector structure
    drawProjector();

    // Draw the projector screen
    drawProjectorScreen();

    // Draw the door
    drawDoor();

    // Draw the windows
    drawWindows();

    // Draw the curtains
    drawCurtains();

    // Draw the lecturer
    glPushMatrix();
    glTranslatef(lecturerX, 0.0f, lecturerZ);
    drawLecturer();
    glPopMatrix();

    // Draw the controlled student
    glPushMatrix();
    glTranslatef(studentX, 0.0f, studentZ);
    drawStudent(studentX, studentZ);
    glPopMatrix();

    // Draw students and desks
    for (int i = 0; i < studentPositions.size(); i++) {
        glPushMatrix();
        glTranslatef(studentPositions[i].first, 0.0f, studentPositions[i].second);
        drawDesk();
        glTranslatef(0.0f, 0.5f, 0.5f);
        drawStudent(studentPositions[i].first, studentPositions[i].second);
        glPopMatrix();
    }

    glutSwapBuffers();
}

// Function to handle animation
void animate(int value) {
    // Animate lecturer's arms
    if (armDirection) {
        lecturerArmAngle += 1.0f;
        if (lecturerArmAngle > 45.0f) armDirection = false;
    } else {
        lecturerArmAngle -= 1.0f;
        if (lecturerArmAngle < -45.0f) armDirection = true;
    }

    // Animate door
    if (doorOpen && doorAngle < 90.0f) {
        doorAngle += 1.0f; // Open the door
    } else if (!doorOpen && doorAngle > 0.0f) {
        doorAngle -= 1.0f; // Close the door
    }

    // Animate curtains
    if (curtainsOpen && curtainAngle < 90.0f) {
        curtainAngle += 1.0f; // Open the curtains
    } else if (!curtainsOpen && curtainAngle > 0.0f) {
        curtainAngle -= 1.0f; // Close the curtains
    }

    // Automatic movement for lecturer and student
    if (autoMove) {
        lecturerX += moveSpeed;
        if (lecturerX > 5.0f || lecturerX < -5.0f) moveSpeed = -moveSpeed; // Reverse direction at boundaries
        studentZ += moveSpeed;
        if (studentZ > 5.0f || studentZ < -5.0f) moveSpeed = -moveSpeed; // Reverse direction at boundaries
    }

    // Students exit via the door one by one
    if (studentsExit && currentExitingStudent < studentPositions.size()) {
        if (studentPositions[currentExitingStudent].first > -9.5f) {
            studentPositions[currentExitingStudent].first -= 0.05f; // Move student toward the door
        } else {
            currentExitingStudent++; // Move to the next student
        }
    }

    // Students enter via the door one by one
    if (studentsEnter && currentEnteringStudent < studentPositions.size()) {
        if (studentPositions[currentEnteringStudent].first < studentTargetPositions[currentEnteringStudent].first) {
            studentPositions[currentEnteringStudent].first += 0.05f; // Move student toward their target position
        } else {
            currentEnteringStudent++; // Move to the next student
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, animate, 0); // 60 FPS
}

// Function to handle window resizing
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Function to handle lecturer movement (arrow keys)
void specialKeys(int key, int x, int y) {
    float moveSpeed = 0.1f;

    switch (key) {
        case GLUT_KEY_UP:
            lecturerZ -= moveSpeed;
            break;
        case GLUT_KEY_DOWN:
            lecturerZ += moveSpeed;
            break;
        case GLUT_KEY_LEFT:
            lecturerX -= moveSpeed;
            break;
        case GLUT_KEY_RIGHT:
            lecturerX += moveSpeed;
            break;
    }
    glutPostRedisplay();
}

// Function to handle student movement (WASD keys) and other interactions
void keyboard(unsigned char key, int x, int y) {
    float moveSpeed = 0.1f;
    float zoomSpeed = 0.5f; // Speed for zooming in and out

    switch (key) {
        case 'w':
        case 'W':
            studentZ -= moveSpeed;
            break;
        case 's':
        case 'S':
            studentZ += moveSpeed;
            break;
        case 'a':
        case 'A':
            studentX -= moveSpeed;
            break;
        case 'd':
        case 'D':
            studentX += moveSpeed;
            break;
        case ',': // Rotate camera left
            cameraAngle -= 0.1f;
            break;
        case '.': // Rotate camera right
            cameraAngle += 0.1f;
            break;
        case 'c': // Toggle clock visibility
            showClock = !showClock;
            break;
        case 'p': // Toggle projector light
            projectorLightOn = !projectorLightOn;
            break;
        case 'b': // Toggle classroom light
            classroomLightOn = !classroomLightOn;
            break;
        case 'o': // Open/close door
            doorOpen = !doorOpen;
            break;
        case 'k': // Open/close curtains
            curtainsOpen = !curtainsOpen;
            break;
        case 'g': // Zoom in
            cameraDistance -= zoomSpeed;
            if (cameraDistance < 5.0f) cameraDistance = 5.0f; // Prevent zooming too close
            break;
        case 'h': // Zoom out
            cameraDistance += zoomSpeed;
            if (cameraDistance > 30.0f) cameraDistance = 30.0f; // Prevent zooming too far
            break;
        case ' ': // Toggle automatic movement
            autoMove = !autoMove;
            break;
        case 'e': // Start students exiting one by one
            studentsExit = true;
            studentsEnter = false; // Stop entering if exiting is triggered
            break;
        case 'r': // Start students entering one by one
            studentsEnter = true;
            studentsExit = false; // Stop exiting if entering is triggered
            break;
    }
    glutPostRedisplay();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Classroom Animation");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING); // Enable lighting globally
    glEnable(GL_COLOR_MATERIAL); // Enable color tracking for materials

    // Initialize student positions and target positions
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            studentPositions.push_back({ -10.0f, j * 2.0f }); // Start outside the classroom
            studentTargetPositions.push_back({ i * 2.0f, j * 2.0f }); // Target positions inside the classroom
        }
    }

    glutDisplayFunc(renderScene);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, animate, 0);

    // Register keyboard callbacks
    glutSpecialFunc(specialKeys); // For arrow keys (lecturer)
    glutKeyboardFunc(keyboard);   // For WASD keys (student) and other interactions

    glutMainLoop();
    return 0;
}
