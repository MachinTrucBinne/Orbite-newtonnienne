/*
Noé Aubin-Cadot.
Codé en novembre 2008.
Adapté pour SDL2 en juin 2017.
Déposé sur GitHub en septembre 2019.
C'est une simulation de l'orbite d'une particule ponctuelle dans le champ gravitationnel de deux corps massifs.
*/
// Pour compiler :
// gcc -o main main.c -framework SDL2 -framework SDL2_image -framework OpenGL -framework GLUT -Wno-deprecated
// ou encore :
// gcc $(sdl2-config --cflags) -Wall -o main  main.c $(sdl2-config --libs) -lSDL2_image -framework OpenGL -framework GLUT -Wno-deprecated

// Pour C :
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

// Pour SDL :
#include <SDL2/SDL.h>
//#include <SDL2_image/SDL_image.h>
#include <SDL2/SDL_image.h>

// Pour OpenGL
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

void Dessiner_planete(double Xorb,double Yorb,double Zorb,double Xterre,double Yterre,double Zterre,double Rterre,double Xlune,double Ylune,double Zlune, double Rlune);
double norme2(double X, double Y, double Z);
double ForceX(double X, double Y, double Z, double x, double y, double z, double R);
double ForceY(double X, double Y, double Z, double x, double y, double z, double R);
double ForceZ(double X, double Y, double Z, double x, double y, double z, double R);
double Ftotal(double X, double Y, double Z, double xterre, double yterre, double zterre, double Rterre, double xlune, double ylune, double zlune, double Rlune, double direction,double FORCE); //direction 1:x, 2:y, 3:z

double angleX = 0;
double angleY = 0;
double angleZ = 0;
double vueX = 0.001;
double vueY = 0;
double vueZ = 16;
int L=25;
double ROT = 0;
double SINGULARITE=10;
double facteur=2;

SDL_Window  *fenetre;
SDL_GLContext contexte;

int main(int argc, char *argv[])
{
    // Initialisation de SDL
    SDL_Init(SDL_INIT_VIDEO);
    
    // Création d'une fenêtre
    fenetre = SDL_CreateWindow("Particule ponctuelle entre la terre et la lune", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
    contexte = SDL_GL_CreateContext(fenetre);
    SDL_SetWindowResizable(fenetre,SDL_TRUE);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective(70,(double)640/480,1,1000);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
    
	Uint32 last_time = SDL_GetTicks();
    Uint32 current_time,ellapsed_time;
    Uint32 start_time;
	
	int i,k;
	int N=3;
	int TOURNE=0;
	int ONDEgo=0;
	double FORCE=3;
	int inerationPARplot=100;
	double eps=0.0005;
	double *X;X=(double *) malloc((N)*sizeof(double));
	double *Y;Y=(double *) malloc((N)*sizeof(double));
	double *Z;Z=(double *) malloc((N)*sizeof(double));
	double *X1;X1=(double *) malloc((N)*sizeof(double)); // X1 = dX/dt
	double *Y1;Y1=(double *) malloc((N)*sizeof(double)); // Y1 = dY/dt
	double *Z1;Z1=(double *) malloc((N)*sizeof(double)); // Z1 = dZ/dt
	
	for (i=0;i<N;i++)
	{
		// INITILISATION DES POSITIONS ( X Y Z ) INITIALES DE L'ORBITE
		X[i]=5;
		Y[i]=0;
		Z[i]=0;
		// INITILISATION DES VITESSES (X' Y' Z') INITIALES DU SATELLITE
		X1[i]=0;
		Y1[i]=2;
		Z1[i]=0;
	}
	
	//double G=1;
	//double Mterre = 3 * G;
	//double Mlune = 1 * G;
	double Rterre = 2;
	double Rlune = 1;
	double *terre;terre=(double *) malloc((3)*sizeof(double)); // position du centre de la terre
	double *lune;lune=(double *) malloc((3)*sizeof(double));   // position du centre de la lune
	
	// INITILISATION DU CENTRE DE LA TERRE (pour l'instant la terre et la lune sont fixes, il serait bien de les faire Žvoluer !)
	terre[0]=0; // X
	terre[1]=0; // Y
	terre[2]=0; // Z
	// INITILISATION DU CENTRE DE LA LUNE (pour l'instant la terre et la lune sont fixes, il serait bien de les faire Žvoluer !)
	lune[0]=6; // X
	lune[1]=0; // Y
	lune[2]=0; // Z
	
    SDL_Event event;
    while(1)
    {
        start_time = SDL_GetTicks();
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
					exit(0);
					break;
				case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
				{
                    case SDLK_ESCAPE: // Veut arrter le jeu
                        exit(0);
						break;
					case SDLK_SPACE: // Stop ou arrte le fait que le plan d'onde tourne
                        {int EVENT=0;
						if (TOURNE)
							{TOURNE=0;EVENT=1;}
						if (!TOURNE && !EVENT)
							{TOURNE=1;}
						break;
						}
					case SDLK_RETURN: // Stop ou arrte le fait que l'onde Žvolue
                        {int EVENT=0;
						if (ONDEgo)
							{ONDEgo=0;EVENT=1;}
						if (!ONDEgo && !EVENT)
							{ONDEgo=1;}
						break;
						}
					case SDLK_UP:
						vueZ-=vueZ/10;
						break;
					case SDLK_DOWN:
						vueZ+=vueZ/10;
						break;
					case SDLK_RIGHT:
						angleZ-=30;
						break;
					case SDLK_LEFT:
						angleZ+=30;
						break;
					case SDLK_o:
						//FORCE--;
						FORCE-=0.1;
						break;
					case SDLK_p:
						//FORCE++;
						FORCE+=0.1;
						break;
					case SDLK_i:
						ROT+=2;
						break;
					case SDLK_k:
						ROT-=2;
						break;
					case SDLK_s:
						inerationPARplot-=10;
						break;
					case SDLK_f:
						inerationPARplot+=10;
						break;
				}
                break;
            }
        }

        current_time = SDL_GetTicks();
        ellapsed_time = current_time - last_time;
        last_time = current_time;
		
		if (TOURNE)
		{
			angleZ += 0.01 * ellapsed_time;
		}
		
        Dessiner_planete(X[0],Y[0],Z[0],terre[0],terre[1],terre[2],Rterre,lune[0],lune[1],lune[2],Rlune);
		if (ONDEgo)
		{
			for (k=0;k<inerationPARplot;k++)
			{
				
				/*
				X[2]=X[1];
				Y[2]=Y[1];
				Z[2]=Z[1];
				X1[2]=X1[1];
				Y1[2]=Y1[1];
				Z1[2]=Z1[1];
				*/
				X[1]=X[0];
				Y[1]=Y[0];
				Z[1]=Z[0];
				X1[1]=X1[0];
				Y1[1]=Y1[0];
				Z1[1]=Z1[0];
				/*
				double CONST=0.000000000005;
				double r3demi=pow(X[0]*X[0]+Y[0]*Y[0]+Z[0]*Z[0],(double)3/2); 
				X[0]=-CONST*X[0]/r3demi+2*X[2]-X[1];
				Y[0]=-CONST*Y[0]/r3demi+2*Y[2]-Y[1];
				Z[0]=-CONST*Z[0]/r3demi+2*Z[2]-Z[1];
				*/	
				
				/*
				// évolution Terre Lune
				X1[0]=X1[1]+eps*FORCE*Ftotal(X[1],Y[1],Z[1], terre[0], terre[1], terre[2], Rterre, lune[0], lune[1], lune[2], Rlune, 1,FORCE);  // polygone Euler (à changer par Runge puis Runge-Kutta ou même Runge-Euler (méthode emboîtées))
				Y1[0]=Y1[1]+eps*FORCE*Ftotal(X[1],Y[1],Z[1], terre[0], terre[1], terre[2], Rterre, lune[0], lune[1], lune[2], Rlune, 2,FORCE);  // polygone Euler
				Z1[0]=Z1[1]+eps*FORCE*Ftotal(X[1],Y[1],Z[1], terre[0], terre[1], terre[2], Rterre, lune[0], lune[1], lune[2], Rlune, 3,FORCE);  // polygone Euler 
				*/
				// évolution sans la lune (donc juste la terre)
				/*
				// Polygone d'Euler
				// Avec juste la terre, on voit que l'Žvolution n'est pas exactement une ellipse... Et que l'on doit utiliser non pas les polygones d'Euler, mais Runge-Euler ou Runge-Kutta ordre 4
				X1[0]=X1[1]+eps*FORCE*ForceX(X[1],Y[1],Z[1], terre[0], terre[1], terre[2], Rterre);
				Y1[0]=Y1[1]+eps*FORCE*ForceY(X[1],Y[1],Z[1], terre[0], terre[1], terre[2], Rterre);
				Z1[0]=Z1[1]+eps*FORCE*ForceZ(X[1],Y[1],Z[1], terre[0], terre[1], terre[2], Rterre);
				*/
				/*
				// Runge sans la lune
				double fx=FORCE*ForceX(X[1],Y[1],Z[1], terre[0], terre[1], terre[2], Rterre);
				double fy=FORCE*ForceY(X[1],Y[1],Z[1], terre[0], terre[1], terre[2], Rterre);
				double fz=FORCE*ForceZ(X[1],Y[1],Z[1], terre[0], terre[1], terre[2], Rterre);
				X1[0]=X1[1]+FORCE*eps*ForceX(X[1]+eps*fx/2,Y[1]+eps*fy/2,Z[1]+eps*fz/2, terre[0], terre[1], terre[2], Rterre);
				Y1[0]=Y1[1]+FORCE*eps*ForceY(X[1]+eps*fx/2,Y[1]+eps*fy/2,Z[1]+eps*fz/2, terre[0], terre[1], terre[2], Rterre);
				Z1[0]=Z1[1]+FORCE*eps*ForceZ(X[1]+eps*fx/2,Y[1]+eps*fy/2,Z[1]+eps*fz/2, terre[0], terre[1], terre[2], Rterre);
				*/
				/*
				// Runge-Kutta ordre 4 sans la lune // Méthode d'ordre 4
				double k1x=FORCE*ForceX(X[1],Y[1],Z[1], terre[0], terre[1], terre[2], Rterre);
				double k1y=FORCE*ForceY(X[1],Y[1],Z[1], terre[0], terre[1], terre[2], Rterre);
				double k1z=FORCE*ForceZ(X[1],Y[1],Z[1], terre[0], terre[1], terre[2], Rterre);
				double k2x=FORCE*ForceX(X[1]+eps*k1x/2,Y[1]+eps*k1y/2,Z[1]+eps*k1z/2, terre[0], terre[1], terre[2], Rterre);
				double k2y=FORCE*ForceY(X[1]+eps*k1x/2,Y[1]+eps*k1y/2,Z[1]+eps*k1z/2, terre[0], terre[1], terre[2], Rterre);
				double k2z=FORCE*ForceZ(X[1]+eps*k1x/2,Y[1]+eps*k1y/2,Z[1]+eps*k1z/2, terre[0], terre[1], terre[2], Rterre);
				double k3x=FORCE*ForceX(X[1]+eps*k2x/2,Y[1]+eps*k2y/2,Z[1]+eps*k2z/2, terre[0], terre[1], terre[2], Rterre);
				double k3y=FORCE*ForceY(X[1]+eps*k2x/2,Y[1]+eps*k2y/2,Z[1]+eps*k2z/2, terre[0], terre[1], terre[2], Rterre);
				double k3z=FORCE*ForceZ(X[1]+eps*k2x/2,Y[1]+eps*k2y/2,Z[1]+eps*k2z/2, terre[0], terre[1], terre[2], Rterre);
				double k4x=FORCE*ForceX(X[1]+eps*k3x,Y[1]+eps*k3y,Z[1]+eps*k3z, terre[0], terre[1], terre[2], Rterre);
				double k4y=FORCE*ForceY(X[1]+eps*k3x,Y[1]+eps*k3y,Z[1]+eps*k3z, terre[0], terre[1], terre[2], Rterre);
				double k4z=FORCE*ForceZ(X[1]+eps*k3x,Y[1]+eps*k3y,Z[1]+eps*k3z, terre[0], terre[1], terre[2], Rterre);
				double ktotx=eps*(k1x+2*k2x+2*k3x+k4x)/6;
				double ktoty=eps*(k1y+2*k2y+2*k3y+k4y)/6;
				double ktotz=eps*(k1z+2*k2z+2*k3z+k4z)/6;
				X1[0]=X1[1]+FORCE*eps*ForceX(X[1]+ktotx,Y[1]+ktoty,Z[1]+ktotz, terre[0], terre[1], terre[2], Rterre);
				Y1[0]=Y1[1]+FORCE*eps*ForceY(X[1]+ktotx,Y[1]+ktoty,Z[1]+ktotz, terre[0], terre[1], terre[2], Rterre);
				Z1[0]=Z1[1]+FORCE*eps*ForceZ(X[1]+ktotx,Y[1]+ktoty,Z[1]+ktotz, terre[0], terre[1], terre[2], Rterre);
				*/
				
				// RUNGE AVEC LA LUNE (donc pratiquement code final) // Méthode d'ordre 2
				double fx=Ftotal(X[1],Y[1],Z[1], terre[0], terre[1], terre[2], Rterre, lune[0], lune[1], lune[2], Rlune, 1,FORCE);
				double fy=Ftotal(X[1],Y[1],Z[1], terre[0], terre[1], terre[2], Rterre, lune[0], lune[1], lune[2], Rlune, 2,FORCE);
				double fz=Ftotal(X[1],Y[1],Z[1], terre[0], terre[1], terre[2], Rterre, lune[0], lune[1], lune[2], Rlune, 3,FORCE);
				X1[0]=X1[1]+eps*Ftotal(X[1]+eps*fx/2,Y[1]+eps*fy/2,Z[1]+eps*fz/2, terre[0], terre[1], terre[2], Rterre, lune[0], lune[1], lune[2], Rlune, 1,FORCE);
				Y1[0]=Y1[1]+eps*Ftotal(X[1]+eps*fx/2,Y[1]+eps*fy/2,Z[1]+eps*fz/2, terre[0], terre[1], terre[2], Rterre, lune[0], lune[1], lune[2], Rlune, 2,FORCE);
				Z1[0]=Z1[1]+eps*Ftotal(X[1]+eps*fx/2,Y[1]+eps*fy/2,Z[1]+eps*fz/2, terre[0], terre[1], terre[2], Rterre, lune[0], lune[1], lune[2], Rlune, 3,FORCE);	
				
				X[0]=X[1]+eps*X1[0]; // polygone Euler
				Y[0]=Y[1]+eps*Y1[0]; // polygone Euler
				Z[0]=Z[1]+eps*Z1[0]; // polygone Euler
					
			}
		}
        ellapsed_time = SDL_GetTicks() - start_time;
        if (ellapsed_time < 10)
        {
            SDL_Delay(10 - ellapsed_time);
        }
    }
    return 0;
}

void Dessiner_planete(double Xorb,double Yorb,double Zorb,double Xterre,double Yterre,double Zterre,double Rterre,double Xlune,double Ylune,double Zlune, double Rlune)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );

    gluLookAt(vueX,vueY,vueZ,0,0,ROT,0,0,1);

    glRotated(angleZ,0,0,1);
    glRotated(angleY,0,1,0);
	glRotated(angleX,1,0,0);

    GLUquadric* params = gluNewQuadric();
    gluQuadricDrawStyle(params,GLU_LINE);
	
	// terre
	glColor3ub(0,255,0);
	glTranslated(Xterre,Yterre,Zterre);
	gluSphere(params,Rterre,20,20);
	glColor3ub(255,255,255);
	//lune
	glTranslated(Xlune-Xterre,Ylune-Yterre,Zlune-Zterre);
    gluSphere(params,Rlune,20,20);
	
	//orbite
	glColor3ub(0,0,255);
	glTranslated(Xorb-Xlune,Yorb-Ylune,Zorb-Zlune);
    gluSphere(params,0.1,20,20);

	glEnd();
    glFlush();
    SDL_GL_SwapWindow(fenetre);
}

double norme2(double X, double Y, double Z)
{
	return X*X + Y*Y + Z*Z;
}
double ForceX(double X, double Y, double Z, double x, double y, double z, double R)
{
	double Xx=X-x;
	double Yy=Y-y;
	double Zz=Z-z;
	double dist2=norme2(Xx,Yy,Zz);
	if (dist2<R)
	{
		return -Xx;
	}
	else
	{
		return -Xx*R*R*R/pow(dist2,(double)3/2);
	}
}
double ForceY(double X, double Y, double Z, double x, double y, double z, double R)
{
	double Xx=X-x;
	double Yy=Y-y;
	double Zz=Z-z;
	double dist2=norme2(Xx,Yy,Zz);
	if (dist2<R)
	{
		return -Yy;
	}
	else
	{
		return -Yy*R*R*R/pow(dist2,(double)3/2);
	}
}
double ForceZ(double X, double Y, double Z, double x, double y, double z, double R)
{
	double Xx=X-x;
	double Yy=Y-y;
	double Zz=Z-z;
	double dist2=norme2(Xx,Yy,Zz);
	if (dist2<R)
	{
		return -Zz;
	}
	else
	{
		return -Zz*R*R*R/pow(dist2,(double)3/2);
	}
}
double Ftotal(double X, double Y, double Z, double xterre, double yterre, double zterre, double Rterre, double xlune, double ylune, double zlune, double Rlune, double direction,double FORCE) //direction 1:x, 2:y, 3:z
{
	if (direction == 1)
		return FORCE*(ForceX(X, Y, Z, xterre, yterre, zterre, Rterre) + ForceX(X, Y, Z, xlune, ylune, zlune, Rlune)); 
	else if (direction == 2)
		return FORCE*(ForceY(X, Y, Z, xterre, yterre, zterre, Rterre) + ForceY(X, Y, Z, xlune, ylune, zlune, Rlune));
	else if (direction == 3)
		return FORCE*(ForceZ(X, Y, Z, xterre, yterre, zterre, Rterre) + ForceZ(X, Y, Z, xlune, ylune, zlune, Rlune));
	return 0;
}