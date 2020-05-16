#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#define OUTFILE "mandelbrot.out"
double XMIN=-2;
double YMIN=-2;
double XMAX=2;
double YMAX=2;
double RESOLUTION=0.001;
int NITERMAX=2000;
int main(int argc,char **argv)
{
int * itertab;
int nbpixelx;
int nbpixely;
int xpixel=0,ypixel=0;
FILE * file;
/*calcul du nombre de pixel*/
nbpixelx = ceil((XMAX - XMIN) / RESOLUTION);
nbpixely = ceil((YMAX - YMIN) / RESOLUTION);
/*allocation du tableau de pixel*/
if( (itertab=malloc(sizeof(int)*nbpixelx*nbpixely)) == NULL)
{
printf("ERREUR d'allocation de itertab, errno : %d(%s) .\n",errno,strerror(errno));
return EXIT_FAILURE;
}
/*calcul des points*/
for(xpixel=0;xpixel<nbpixelx;xpixel++)
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
/*output des resultats compatible gnuplot*/
if( (file=fopen(OUTFILE,"w")) == NULL ) {
        printf("Erreur à l'ouverture du fichier de sortie : errno %d (%s) .\n",errno,strerror(errno));
return EXIT_FAILURE;
}
for(xpixel=0;xpixel<nbpixelx;xpixel++) {
for(ypixel=0;ypixel<nbpixely;ypixel++) {
double x = XMIN + xpixel * RESOLUTION;
double y = YMIN + ypixel * RESOLUTION;
fprintf(file,"%f %f %d\n", x, y,
itertab[xpixel*nbpixely+ypixel]);
}
fprintf(file,"\n");
}
pclose(file);
/*sortie du programme*/
return EXIT_SUCCESS;
}
