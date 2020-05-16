#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#define OUTFILE "mandelbrot.out"
typedef struct parametres_thread {
   int debut;
   int fin;
} parametres_thread ;
double XMIN=-2;
double YMIN=-2;
double XMAX=2;
double YMAX=2;
double RESOLUTION=0.01;
int NITERMAX=400;
int *itertab;
int nbpixelx;
int nbpixely;
void * thread_pixels( void * p_arg ){//fonction de thread
    parametres_thread *params;
    params = (parametres_thread *) p_arg;
    int debut = params->debut;
    int fin = params->fin;
    int xpixel=0,ypixel=0;
    /*calcul des points*/
    for(xpixel=debut;xpixel<fin;xpixel++)
        for(ypixel=0;ypixel<nbpixely;ypixel++) {
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
            itertab[xpixel*nbpixely+ypixel]=iter;}
}


int main(int argc,char **argv)
{
pthread_t threads[2];
/*calcul du nombre de pixel*/
nbpixelx = ceil((XMAX - XMIN) / RESOLUTION);
nbpixely = ceil((YMAX - YMIN) / RESOLUTION);
FILE * file;
/*allocation du tableau de pixel*/
if( (itertab=malloc(sizeof(int)*nbpixelx*nbpixely)) == NULL)
{
printf("ERREUR d'allocation de itertab, errno : %d(%s) .\n",errno,strerror(errno));
return EXIT_FAILURE;
}
/*creation des threads*/
parametres_thread *params1;
parametres_thread *params2;
params1 = malloc(sizeof(parametres_thread));
params2 = malloc(sizeof(parametres_thread));
params1->debut = 0;
params1->fin = nbpixelx/2;
params2->debut= nbpixelx - nbpixelx/2;
params2->fin = nbpixelx;
pthread_create(&threads[0], NULL, thread_pixels, (void *)params1);
pthread_create(&threads[1], NULL, thread_pixels, (void *)params2);
pthread_join( threads[1],NULL);
pthread_join( threads[0],NULL );
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
/*sortie du programme*/
return EXIT_SUCCESS;
}

