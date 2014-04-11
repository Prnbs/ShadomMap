
#include "Utils.h"
#include <iostream>
#include "CurrentState.h"
#include "World.h"
#define WINDOW_TITLE_PREFIX "Chapter 4"

using namespace std;

int CurrentWidth = 1520,
	CurrentHeight = 1000,
	WindowHandle = 0,
	scrCenterX = 0,
	scrCenterY = 0;

unsigned FrameCount = 0;

Matrix
	ProjectionMatrix,
	ViewMatrix,
	ModelMatrix;

Vector
	Eye,
	LookAt;

bool meshON = false;

clock_t LastTime = 0;

void Initialize(int, char*[]);
void InitWindow(int, char*[]);
void ResizeFunction(int, int);
void RenderFunction(void);
void TimerFunction(int);
void IdleFunction(void);
void KeyboardFunction(unsigned char, int , int );
void MouseFunction(int , int );
void ResetMouse(int);
void Destroy(void);
void SetViewMatrixForStrafe();

CurrentState state;
World world;

int main(int argc, char* argv[])
{
	
	Initialize(argc, argv);

	glutMainLoop();
	
	exit(EXIT_SUCCESS);
}

void Initialize(int argc, char* argv[])
{
	GLenum GlewInitResult;

	InitWindow(argc, argv);
	
	glewExperimental = GL_TRUE;
	GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult) {
		fprintf(
			stderr,
			"ERROR: %s\n",
			glewGetErrorString(GlewInitResult)
		);
		exit(EXIT_FAILURE);
	}
	
	fprintf(
		stdout,
		"INFO: OpenGL Version: %s\n",
		glGetString(GL_VERSION)
	);

	glGetError();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	ExitOnGLError("ERROR: Could not set OpenGL depth testing options");

	ExitOnGLError("ERROR: Could not set OpenGL culling options");

	ProjectionMatrix = IDENTITY_MATRIX;
	ViewMatrix = IDENTITY_MATRIX;
	
	Eye.v[0] = 0;	Eye.v[1] = 0;	Eye.v[2] = -2.0f;
	LookAt.v[0] = 0;	LookAt.v[1] = 0;	LookAt.v[2] = 0.0f;
	CreateViewMatrix(&ViewMatrix, Eye, LookAt);
	TranslateMatrix(&ViewMatrix, Eye.v[0], Eye.v[1], Eye.v[2]);
	
	state.SetEye(Eye);
	state.SetLookAt(LookAt);
	state.SetViewMatrix(ViewMatrix);
	
	world.CreateBodies();

}

void InitWindow(int argc, char* argv[])
{
	glutInit(&argc, argv);
	
	glutInitContextVersion(4, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
	);
	
	glutInitWindowSize(CurrentWidth, CurrentHeight);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);

	if(WindowHandle < 1) {
		fprintf(
			stderr,
			"ERROR: Could not create a new rendering window.\n"
		);
		exit(EXIT_FAILURE);
	}
	
	glutReshapeFunc(ResizeFunction);
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(IdleFunction);
	glutTimerFunc(0, TimerFunction, 0);
	glutCloseFunc(Destroy);
	glutKeyboardFunc(KeyboardFunction);
	glutPassiveMotionFunc(MouseFunction);
	glutWarpPointer(CurrentWidth/2.0f, CurrentHeight/2.0f);
	glutSetCursor(GLUT_CURSOR_NONE); 
	scrCenterX = CurrentWidth/2;
	scrCenterY = CurrentHeight/2;
}

void RenderFunction(void)
{
	++FrameCount;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	world.DrawBodies();
	
	glutSwapBuffers();
	glutPostRedisplay();
}

void IdleFunction(void)
{
	glutPostRedisplay();
}

void TimerFunction(int Value)
{
	if (0 != Value) {
		char* TempString = (char*)
			malloc(512 + strlen(WINDOW_TITLE_PREFIX));

		sprintf(
			TempString,
			"%s: %d Frames Per Second @ %d x %d",
			WINDOW_TITLE_PREFIX,
			FrameCount * 4,
			CurrentWidth,
			CurrentHeight
		);

		glutSetWindowTitle(TempString);
		free(TempString);
	}

	FrameCount = 0;
	glutTimerFunc(250, TimerFunction, 1);
}


void Destroy()
{
	world.DestroyBodies();
}


void ResizeFunction(int Width, int Height)
{
	CurrentWidth = Width;
	CurrentHeight = Height;
	glViewport(0, 0, CurrentWidth, CurrentHeight);
	ProjectionMatrix =
		CreateProjectionMatrix(
			60,
			(float)CurrentWidth / CurrentHeight,
			1.0f,
			100.0f
		);
	state.SetProjectionMatrix(ProjectionMatrix);
}

void MouseFunction(int X, int Y)
{
	
	int diffX = -X + scrCenterX;
	int diffY = -Y + scrCenterY;	
	ViewMatrix = state.GetViewMatrix();
	RotateAboutY(&ViewMatrix, (float)diffX/6000.0f);
	state.SetViewMatrix(ViewMatrix);

	glutTimerFunc(1000, ResetMouse, 1);
	
}

void ResetMouse(int Value)
{
	glutWarpPointer(CurrentWidth/2.0f, CurrentHeight/2.0f);
}

void KeyboardFunction(unsigned char Key, int X, int Y)
{
	X; Y; // Resolves warning C4100: unreferenced formal parameter
	ViewMatrix = state.GetViewMatrix();

	switch (Key)
	{
		case 'W':
		case 'w':
			{
				ViewMatrix.m[14] += 0.1;
				state.SetViewMatrix(ViewMatrix);
				break;
			}
		case 'S':
		case 's':
			{
				ViewMatrix.m[14] -= 0.1;
				state.SetViewMatrix(ViewMatrix);
				break;
			}
		case 'A':
		case 'a':
			{
				ViewMatrix.m[12] += 0.1;
				state.SetViewMatrix(ViewMatrix);
				break;
			}

		case 'D':
		case 'd':
			{
				ViewMatrix.m[12] -= 0.1;
				state.SetViewMatrix(ViewMatrix);
				break;
			}
		case 'q':
		case 'Q':
			{
				ViewMatrix.m[13] -= 0.1;
				state.SetViewMatrix(ViewMatrix);
				break;
			}
		case 'z':
		case 'Z':
			{
				ViewMatrix.m[13] += 0.1;
				state.SetViewMatrix(ViewMatrix);
				break;
			}
		case 'i':
		case 'I':
			{	
				if(!meshON)
				{
					glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE ) ;
					meshON = true;
				}
				else
				{
					glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL ) ;
					meshON = false;
				}
			}
		default:
			break;
	}
}


