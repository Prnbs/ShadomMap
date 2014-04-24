
#include "Utils.h"
#include <iostream>
#include "CurrentState.h"
#include "World.h"
#define WINDOW_TITLE_PREFIX "Chapter 4"
#define DEPTH_TEXTURE_SIZE      4096

using namespace std;
//1520,1000
int CurrentWidth = 1220,
	CurrentHeight = 650,
	WindowHandle = 0,
	scrCenterX = 0,
	scrCenterY = 0;

unsigned FrameCount = 0;

Matrix
	ProjectionMatrix,
	OrthoProjectionMatrix,
	ViewMatrix,
	ShadowViewMatrix,
	ModelMatrix;

Vector
	Eye,
	LookAt,
	LightDir;

bool meshON = false;

bool shot = false;

clock_t LastTime = 0;

 Vector InvDirectLight;

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
Vector DirectLight;

int main(int argc, char* argv[])
{
	
	DirectLight.v[0] = 0.0f;
	DirectLight.v[1] = 2.0f;
	DirectLight.v[2] = 0.0f;

	state.SetLightDirection(DirectLight);
	
	Initialize(argc, argv);

	glutMainLoop();
	cout << "Calling Exit " << endl;
	exit(EXIT_SUCCESS);
}

void Initialize(int argc, char* argv[])
{
	GLenum GlewInitResult;

	InitWindow(argc, argv);
	OrthoProjectionMatrix = CreateOrthoProjectionMatrix(
			-10,10,
			-10,10,
			-1, 100);
	state.SetOrthoProjectionMatrix(OrthoProjectionMatrix);

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
	ShadowViewMatrix = IDENTITY_MATRIX;
	Eye.v[0] = -0.2;	Eye.v[1] = -0.4;	Eye.v[2] = -2.0f;
	LookAt.v[0] = 0;	LookAt.v[1] = 0;	LookAt.v[2] = 0.0f;
   
	ScaleVector(&state.GetLightDirection(), -1.0, &InvDirectLight);
   // Vector ShadowLookAt = DirectLight;
   //GOLDEN FOR VIEW
    CreateViewMatrix(&ViewMatrix, Eye, LookAt);
    TranslateMatrix(&ViewMatrix, Eye.v[0], Eye.v[1], Eye.v[2]);

    //PUTTING CAMERA IN LIGHT"S POSN
 /*   CreateViewMatrix(&ViewMatrix, InvDirectLight, Eye);
    TranslateMatrix(&ViewMatrix, Eye.v[0], Eye.v[1], Eye.v[2]);*/



    //GOLDEN FOR SHADOW
    CreateViewMatrix(&ShadowViewMatrix, InvDirectLight, Eye);
    TranslateMatrix(&ShadowViewMatrix, Eye.v[0], Eye.v[1], Eye.v[2]);
	
	state.SetEye(Eye);
	state.SetLookAt(LookAt);
	state.SetViewMatrix(ViewMatrix);
	state.SetActualViewMatrix(ViewMatrix);
	state.SetShadowViewMatrix(ShadowViewMatrix);
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

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);

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
	static const GLfloat zero[] = { 0.0f };
	++FrameCount;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

	glBindFramebuffer(GL_FRAMEBUFFER, state.shadow_Fbuffer);
	glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE); 
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(4.0f, 4.0f);
	static const GLenum buffs[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, buffs);
	glClearBufferfv(GL_DEPTH, 0, zero);
	

	ScaleVector(&state.GetLightDirection(), -1.0, &InvDirectLight);
	CreateViewMatrix(&ShadowViewMatrix, InvDirectLight, Eye);
    TranslateMatrix(&ShadowViewMatrix, Eye.v[0], Eye.v[1], Eye.v[2]);
	state.SetShadowViewMatrix(ShadowViewMatrix);

	state.SetViewMatrix(state.GetShadowViewMatrix());
    world.DrawBodies(GL_FALSE);

	glDisable(GL_POLYGON_OFFSET_FILL);
	ExitOnGLError("ERROR:  glDisable(GL_POLYGON_OFFSET_FILL)");

	if(shot)
	{
		int row_size = ((DEPTH_TEXTURE_SIZE * 3 + 3) & ~3);
		int data_size = row_size * DEPTH_TEXTURE_SIZE;
		unsigned char * data = (unsigned char *) malloc (data_size);
		screenShot(DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE, "shot.tga", GL_DEPTH_COMPONENT, GL_BYTE, data, data_size);
		shot = false;
	}
	 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, CurrentWidth, CurrentHeight); 
	glClear(GL_DEPTH_BUFFER_BIT);

	state.SetViewMatrix(state.GetActualViewMatrix());
    world.DrawBodies(GL_TRUE);

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
			400.0f
		);
	state.SetProjectionMatrix(ProjectionMatrix);
}

void MouseFunction(int X, int Y)
{
	/*int diffX = -X + scrCenterX;
	int diffY = -Y + scrCenterY;	
	ViewMatrix = state.GetViewMatrix();
	RotateAboutY(&ViewMatrix, (float)diffX/6000.0f);
	state.SetViewMatrix(ViewMatrix);

	glutTimerFunc(1000, ResetMouse, 1);*/
	
}

void ResetMouse(int Value)
{
	glutWarpPointer(CurrentWidth/2.0f, CurrentHeight/2.0f);
}

void KeyboardFunction(unsigned char Key, int X, int Y)
{
	X; Y; // Resolves warning C4100: unreferenced formal parameter
	ViewMatrix = state.GetActualViewMatrix();

	switch (Key)
	{
		case 'W':
		case 'w':
			{
				ViewMatrix.m[14] += 0.1;
				state.SetActualViewMatrix(ViewMatrix);
				break;
			}
		case 'S':
		case 's':
			{
				ViewMatrix.m[14] -= 0.1;
				state.SetActualViewMatrix(ViewMatrix);
				break;
			}
		case 'A':
		case 'a':
			{
				ViewMatrix.m[12] += 0.1;
				state.SetActualViewMatrix(ViewMatrix);
				break;
			}

		case 'D':
		case 'd':
			{
				ViewMatrix.m[12] -= 0.1;
				state.SetActualViewMatrix(ViewMatrix);
				break;
			}
		case 'q':
		case 'Q':
			{
				ViewMatrix.m[13] -= 0.1;
				state.SetActualViewMatrix(ViewMatrix);
				break;
			}
		case 'z':
		case 'Z':
			{
				ViewMatrix.m[13] += 0.1;
				state.SetActualViewMatrix(ViewMatrix);
				break;
			}
		case 'l':
		case 'L':
			{
				LightDir = state.GetLightDirection();
				LightDir.v[0] += 0.1;
				state.SetLightDirection(LightDir);
				PrintVector(LightDir);
				break;
			}
		case 'o':
		case 'O':
			{
				LightDir = state.GetLightDirection();
				LightDir.v[0] -= 0.1;
				state.SetLightDirection(LightDir);
				PrintVector(LightDir);
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
		case 'r':
		case 'R':
			{
				shot = true;
				break;
			}
		default:
			break;
	}
}


