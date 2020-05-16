#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#define OUTFILE "mandelbrot.out"
double XMIN=-2;
double YMIN=-2;
double XMAX=2;
double YMAX=2;
double RESOLUTION=0.0006;
int NITERMAX=2000;
int main( int argc, char **argv )
{
    //l'identifieur de chaque processus et la taille de tous les processus
    int rank, size;
    // le fichier de sortie
    FILE * file;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    /*calcul du nombre de pixel*/
    int nbpixelx = ceil((XMAX - XMIN) / RESOLUTION);
    int nbpixely = ceil((YMAX - YMIN) / RESOLUTION);
    //découpage de l'intervale des abscises en des sous intervales 
    int etape = nbpixelx/size;
    //le tableau que doit remplire chaque processus	
    int *itertabp = malloc(sizeof(int)*nbpixely*etape);
    /* le calcule de la fractale */
    for(int xpixel= 0; xpixel<etape; xpixel++)
        for(int ypixel=0;ypixel<=nbpixely;ypixel++) {
		double xinit = XMIN + (xpixel+rank*etape)*RESOLUTION;
		double yinit = YMIN + ypixel * RESOLUTION;
		double x=xinit;
		double y=yinit;
		int iter=0;
		for(iter=0;iter<NITERMAX;iter++) {
			double prevy=y,prevx=x;
			if( (x*x + y*y) > 4 )
			break;
			x= prevx*prevx - prevy*prevy + xinit;
			y= 2*prevx*prevy + yinit;
		}
		itertabp[xpixel*nbpixely+ypixel]=iter;}
    //le tableau qui content le resultat finale aprés rassembler tous les tableaux 
    int *itertab;
    if(rank==0)  itertab = malloc(sizeof(int)*nbpixelx*nbpixely);
    else  itertab  = NULL;

    // rassemblage des résultats en utilisant mpi_gather
    MPI_Gather(itertabp, nbpixely*etape, MPI_INT, itertab, nbpixely*etape, MPI_INT, 0, MPI_COMM_WORLD);
 
    if(rank == 0)
    {//remplissage du fichier de sortie par le processuss root
	/*output des resultats compatible gnuplot*/
	if( (file=fopen(OUTFILE,"w")) == NULL ) {
	printf("Erreur à l'ouverture du fichier de sortie : errno %d (%s) .\n",errno,strerror(errno));
	return EXIT_FAILURE;
	}
	int xpixel=0,ypixel=0;
	for(xpixel=0;xpixel<nbpixelx;xpixel++) {
	    for(ypixel=0;ypixel<nbpixely;ypixel++) {
		double x = XMIN + xpixel * RESOLUTION;
		double y = YMIN + ypixel * RESOLUTION;
		fprintf(file,"%f %f %d\n", x, y,
		itertab[xpixel*nbpixely+ypixel]);
	    }
	fprintf(file,"\n");
	}
	fclose(file);
    }
    MPI_Finalize();
    return 0;
}
