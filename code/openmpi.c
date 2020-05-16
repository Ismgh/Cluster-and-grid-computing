#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#define OUTFILE "mandelbrot.out"
double XMIN=-2;
double YMIN=-2;
double XMAX=2;
double YMAX=2;
double RESOLUTION=0.001;
int NITERMAX=400;
int step;
int achievement;
int *itertab;
int nbpixelx;
int nbpixely;
pthread_mutex_t lock;
void * thread_pixels( void * p_arg ){//fonction de thread
    //section critique
    pthread_mutex_lock(&lock);
    int debut = achievement;
    int fin = step+achievement;
    achievement = step+achievement;
    pthread_mutex_unlock(&lock);
    int xpixel=0,ypixel=0;
    /*calcul des points*/
    for(xpixel=debut;xpixel<fin;xpixel++)
        for(ypixel=0;ypixel<=nbpixely/2;ypixel++) {
        double xinit = XMIN + xpixel * RESOLUTION;
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
        itertab[xpixel*nbpixely+ypixel]=iter;
        itertab[xpixel*nbpixely-ypixel+nbpixely]=iter;}
}

int main(int argc,char **argv)
{
	/*calcul du nombre de pixel*/
	nbpixelx = ceil((XMAX - XMIN) / RESOLUTION);
	nbpixely = ceil((YMAX - YMIN) / RESOLUTION);
	FILE * file;
	int size, my_rank, rc; // des variables à utiliser pour openmpi
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	int nombre_threads = size;
	/*
	printf("\t veuillez entrer le nombre de threads que vous voulez créer \t : ");
	scanf("%d",&nombre_threads);
	/*allocation du tableau de pixel*/
	if( (itertab=malloc(sizeof(int)*nbpixelx*nbpixely)) == NULL)
	{
		printf("ERREUR d'allocation de itertab, errno : %d(%s) .\n",errno,strerror(errno));
		return EXIT_FAILURE;
	}
	/*creation des threads*/
	pthread_t *threads;
	threads = malloc(sizeof(pthread_t)*nombre_threads);
	/* initialistation des variables communs entre les threads */
	step = nbpixelx/nombre_threads;
	achievement=0;
	/* devision de la tache sur les threads */
	int i;
	for(i=0;i<nombre_threads;i++){
	  pthread_create(&threads[i], NULL, thread_pixels, NULL);
	}
	for(i=0;i<nombre_threads;i++){
	  pthread_join( threads[i],NULL );
	}
	/*output des resultats compatible gnuplot*/
	if( (file=fopen(OUTFILE,"w")) == NULL ) {
		printf("Erreur  l'ouverture du fichier de sortie : errno %d (%s) .\n",errno,strerror(errno));
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
	/*sortie du programme*/
	return EXIT_SUCCESS;
}
