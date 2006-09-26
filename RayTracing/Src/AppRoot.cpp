#include "AppRoot.h"

#include "GLUTHandler.h"
#include "OGLRenderSystem.h"
#include "Scene.h"

#include "Filter.h"
#include "Image.h"
#include "Sphere.h"
#include "Plane.h"
#include "Quaternion.h"
#include "SceneLoader.h"

#include <cstdio>
using namespace std;

AppRoot::AppRoot(void)
: pressedKeyFORWARD(false),
pressedKeyBACKWARD(false),
pressedKeyLEFT(false),
pressedKeyRIGHT(false),
pressedKeyUP(false),
pressedKeyDOWN(false),
pressedButtonLEFT(false),
pressedButtonRIGHT(false),
velocity(2.0),
SBEnable(false),
SBFullScreen(false),
SBIntensity(2.0),
SBmaxSpreadFactor(0.05),
SBSamples(9)
{
}

AppRoot::~AppRoot(void)
{
	delete[] this->frameBuffer;
}

void AppRoot::init( int argc, char **args )
{	
	//this->rayTracer.init( );
	//this->scene.init( );
	
	SceneLoader::loadScene("Scene1.txt", *this, this->rayTracer, this->scene);
	
	this->cameraController.setCamera( &this->scene.getCamera() );
	
	this->frameBuffer = new FrameBuffer(this->rayTracer.getWidth(), this->rayTracer.getHeight());

	this->filter = new Filter( 2, this->rayTracer.getWidth(), this->rayTracer.getHeight() );

	// Inicializa o GLUT (sistema de janelas)
	GLUTHandler::setListener( this );
	GLUTHandler::startGLUT( argc, args, this->rayTracer.getWidth(), this->rayTracer.getHeight());
	////////////////////////////////////////////////////////////
	// ESTE TRECHO S� � ALCANCADO QUANDO O PROGRAMA � FINALIZADO
	////////////////////////////////////////////////////////////
}

void uniformDistribution( float c, int xPoint, int yPoint, ColorRGBf *buffer1, ColorRGBf *buffer2 )
{
//	if( c > 2 )
	{
		float r = c/2;
		float a = 3.14*r*r;
		ColorRGBf temp = buffer1[xPoint + yPoint * 300];
		ColorRGBf intensity = temp/a;

		for( int x = xPoint-r; x < xPoint+r; x++ )
		{
			for( int y = yPoint-r; y < yPoint+r; y++ )
			{
				if( x >= 0 && x < 300 &&
					y >= 0 && y < 300 )
				{
					if( (x-xPoint)*(x-xPoint) + (y-yPoint)*(y-yPoint) < r*r)
					{
						buffer2[x + y * 300] = intensity + buffer2[x + y * 300];
					}
				}
			}
		}
	}
//	else 
	{
//		buffer2[xPoint + yPoint * 300] = buffer1[xPoint + yPoint * 300];
	}
}

void DoF( FrameBuffer *buffer1 )
{
	ColorRGBf *buffer2 = new ColorRGBf[300*300];

	for( int i = 0; i < 300*300; i++ )
	{
		buffer2[i] = ColorRGBf(0.0,0.0,0.0);
	}


	for( int x = 0; x < 300; x++ )
	{
		for( int y = 0; y < 300; y++ )
		{	
			float E = 10.0;
			float df = 5.0;
			float dr = 1.0;
			float f = 1.0/(1.0/df + 1.0/dr);

			float d = buffer1->zBuffer[x + y*300];

			float Vd = (f*d)/(d-f);
			float Vf = (f*df)/(df-f);

			//float CoCDiameter = abs(a*(s*(1.0/f - 1.0/depth) - 1.0));
			float CoCDiameter = (abs(Vd - Vf) * E/Vd)*4.0;

			if(x == 150 && y == 150)
			{
				//printf("d = %.2f, CoC = %f\n", d, CoCDiameter);
			}

			uniformDistribution( CoCDiameter, x, y, buffer1->colorBuffer, buffer2  );
		}
	}

	for( int i = 0; i < 300; i++ )
	{
		for( int j = 0; j < 300; j++ )
		{
			if( i > 100 )
			{
				buffer1->colorBuffer[i + j*300] = buffer2[i + j*300];
			}
		}		
	}
}

void AppRoot::idleFunc( double deltaTime )
{
	// Executa ray tracing
	this->rayTracer.executeRayTracer( this->frameBuffer, &scene );

	//GLUTHandler::drawPixels( this->filter->scanImage( this->frameBuffer->colorBuffer ));

	OGLRenderSystem &renderSystem = OGLRenderSystem::getSingleton();

	renderSystem.drawPixels( this->frameBuffer->colorBuffer );

	// Faz specular bloom quando ele est� habilitado
	if( this->SBEnable )
	{
		renderSystem.drawPixelsOverBufferSB( this->frameBuffer->highIntensityBuffer, this->SBFullScreen, this->SBmaxSpreadFactor, this->SBIntensity, this->SBSamples );
		//renderSystem.drawPixelsOverBufferSB( this->frameBuffer->colorBuffer, this->SBFullScreen, this->SBmaxSpreadFactor, this->SBIntensity, this->SBSamples );
	}

	//renderSystem.drawPixelsOverBufferDoF(this->frameBuffer, false, 10);

	this->handleKeyboardInputs( deltaTime );

	///////////////////////////////////////////////////////////////////////////////////////
	//// Lixo
	//////////////////////////////////////////////////////////////////////////////////////
	//static float angle = 0;
	//angle += 0.1;
	//float alplitude = 15;

	//Light * light =  scene.getSceneLights().at(0);
	//light->setWorldPosition(Vector3D(alplitude*sin(angle),alplitude*cos(angle),-10));	
	///////////////////////////////////////////////////////////////////////////////////////
}

void AppRoot::handleKeyboardInputs( double deltaTime )
{	
	float offset = this->velocity*deltaTime;

	// Translacoes
	if( this->pressedKeyFORWARD )		
		this->cameraController.moveOverVector(offset, CameraController::VECTOR_N );		
	if( this->pressedKeyBACKWARD )
		this->cameraController.moveOverVector(-offset, CameraController::VECTOR_N );
	if( this->pressedKeyLEFT )
		this->cameraController.moveOverVector(offset, CameraController::VECTOR_U );
	if( this->pressedKeyRIGHT )
		this->cameraController.moveOverVector(-offset, CameraController::VECTOR_U );
	if( this->pressedKeyUP )
		this->cameraController.moveOverVector(offset, CameraController::VECTOR_V );
	if( this->pressedKeyDOWN )
		this->cameraController.moveOverVector(-offset, CameraController::VECTOR_V );
}

void AppRoot::keyboardFunc(unsigned char key, int x, int y)
{
	switch( key )
	{
	case AppRoot::keyFORWARD:
		this->pressedKeyFORWARD = true;
		break;
	case AppRoot::keyBACKWARD:
		this->pressedKeyBACKWARD = true;
		break;
	case AppRoot::keyLEFT:
		this->pressedKeyLEFT = true;
		break;
	case AppRoot::keyRIGHT:
		this->pressedKeyRIGHT = true;
		break;
	case AppRoot::keyUP:
		this->pressedKeyUP = true;
		break;
	case AppRoot::keyDOWN:
		this->pressedKeyDOWN = true;
		break;
	}

	// Seta a velocidade
	if( key >= 49 && key <= 57 )
	{
		this->velocity = (key-48)*(key-48);
	}
	//printf("Pressed = %c\n", key);
}

void AppRoot::keyboardUpFunc( unsigned char key, int x, int y )
{
	switch(key)
	{
	case AppRoot::keyFORWARD:
		this->pressedKeyFORWARD = false;
		break;
	case AppRoot::keyBACKWARD:
		this->pressedKeyBACKWARD = false;
		break;
	case AppRoot::keyLEFT:
		this->pressedKeyLEFT = false;
		break;
	case AppRoot::keyRIGHT:
		this->pressedKeyRIGHT = false;
		break;
	case AppRoot::keyUP:
		this->pressedKeyUP = false;
		break;
	case AppRoot::keyDOWN:
		this->pressedKeyDOWN = false;
		break;

	default:
		break;
	}
	//printf("Released = %c\n", key);
}

void AppRoot::motionFunc( int x, int y )
{
	float degree = 1;
	float rad = 2*3.1415*degree/360.0;

	// Deltas
	int deltaX = (x - this->xOldPosition);
	int deltaY = (y - this->yOldPosition);

	if( this->pressedButtonLEFT )
	{
		//
		this->cameraController.rotateAroundVector( -deltaX*rad, CameraController::VECTOR_V );
		this->cameraController.rotateAroundVector( deltaY*rad, CameraController::VECTOR_U );
		
		this->xOldPosition = x;
		this->yOldPosition = y;
	}
	else if( this->pressedButtonRIGHT )
	{
		//
		this->cameraController.rotateAroundVector( deltaX*rad, CameraController::VECTOR_N );
		
		this->xOldPosition = x;
	}

	//printf("MotionFunc\n");
}

void AppRoot::mouseFunc( int button, int state, int x, int y )
{
	// Se for o botao esquerdo
	if( button == GLUT_LEFT_BUTTON )
	{
		// Se soltou o botao
		if( state == GLUT_UP )
		{
			this->pressedButtonLEFT = false;
		}
		else
		{
			this->xOldPosition = x;
			this->yOldPosition = y;

			this->pressedButtonLEFT = true;
			//printf( "xLEFTClick = %i, yLEFTClick = %i\n", this->xOldPosition, this->yOldPosition );
		}
	}
	else if( button == GLUT_RIGHT_BUTTON )
	{
		if( state == GLUT_UP )
		{
			this->pressedButtonRIGHT = false;
		}
		else
		{
			this->xOldPosition = x;

			this->pressedButtonRIGHT = true;
			//printf( "xRIGHTClick = %i\n", this->xOldPosition );
		}
	}
	//printf("MouseFunc\n");
}