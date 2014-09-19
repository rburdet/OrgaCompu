#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>

#define MAX_VAL 255

void usage(char* nombre);
void version(char* nombre);

/** Dada una resolucion en formato WIDTHxHEIGHT devuelve un vector [WIDTH,HEIGHT] por defecto es [640,480] 
 * @param char* String con resolucion del tipo WIDTHxHEIGHT
 * @param int* vector de ints de resolucion [WIDTH,HEIGHT]
 * @return int 0 si se pudo parsear la resolucion, -1 en otro caso
 */

int parseResolution(char* res,int* resArray);

/** Dada una coordenada compleja en representacion binomica ( a + bi ) devuelve un vector [posX, posY], por defecto es [0,0] 
 * @param char* String con posicion del centro de pantalla en forma binomica a+bi
 * @param double* vector de ints de centro [posX,posY]
 * @return int 0 si se pudo parsear la resolucion, -1 en otro caso
 */

int parseCenter(char* center, double* centerArray);

/** Dadas las partes real e imaginara de un numero complejo devuelve 1 si el modulo es mayor que 2, 0 en caso contrario
 * @param double parte real de un numero complejo
 * @param double parte imaginaria de un numero complejo
 * return int 1 si es el modulo es mayor que 2, 0 si es menor o igual
 */

int stop(double re, double im);


/** Dado un numero complejo asociado a un pixel de la pantalla devuelve la intensidad asociada a ese pixel segun el algoritmo para mostrar conjuntos de Mandelbrot
 * @param double parte real del complejo asociado a un pixel
 * @param double parte imaginaria del complejo asociado a un pixel
 * return char intensidad de ese pixel
 */
unsigned char getIntensity(double re, double im);

int main(int argc, char* argv[]){
	int opt;
	int res[2] = {640,480};
	double  width = 4;
	double  height = 4;
	double center[2] = {0,0};
	char* outDir;


	static struct option long_options[] ={
		{"resolution",	optional_argument,		0,	'r'},
		{"center",		optional_argument,		0,	'c'},
		{"width",		optional_argument,		0,	'w'},
		{"height",		optional_argument,		0,	'H'},
		{"output",		required_argument,		0,	'o'},
		{"version",		optional_argument,		0,	'V'},
		{"help",		optional_argument,		0,	'h'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long (argc, argv, "r:c:w:H:o:Vh", long_options, NULL)) != -1){
		switch(opt){
			case 'r':
				parseResolution(optarg,res);
				break;
			case 'w':
				width = atoi(optarg);
				break;
			case 'H':
				height = atoi(optarg);
				break;
			case 'o':
				outDir = optarg;
				break;
			case 'c':
				parseCenter(optarg,center);
				break;
			case 'h':
				usage(argv[0]);
				break;
			case 'V':
				version(argv[0]);
				break;
			case '?':
				printf("Error\n");
				break;
		}
	}
	print(res,center,width,height);
		//printf( "%d \n ", getIntensity(center[0],center[1]));
}

int print(int* res, double* center, double width , double height){
	FILE* prueba;
	prueba = fopen("prueba.pgm","wb");
	printf("width : %f \t height: %f \n",width,height);
	printf("width : %f \t height: %f \n",width,height);
	double stepX = width / res[0];
	double stepY = height/ res[1];
	printf("stepX: %f \t stepY: %f \n",stepX,stepY);
	double i ;
	double j ; 
	fprintf(prueba, "P2\n%d\n%d\n%d\n", res[0],res[1],MAX_VAL);

	for ( i = center[0]-width/2 ; i < center[0]+width/2 ; i+=stepX ) {
		for ( j = center[1]-height/2 ; j < center[1]+height/2 ; j+=stepY) {
			fprintf(prueba," numero: %f+%fi %d ",i,j,getIntensity(i,j));
			fputc(' ',prueba);
		}
		fputc('\n',prueba);
	} 
	fclose(prueba);
}

unsigned char getIntensity(double re, double im){
	unsigned char i;
	unsigned char intensity = MAX_VAL;
	double auxRe =re ;
	double auxIm =im ;
	for( i = 0 ; i < MAX_VAL ; i++ ){
		//printf( "stop: %d , [%f,%f]\n" ,stop(auxRe,auxIm),auxRe,auxIm);
		if ( stop(auxRe,auxIm) ){
			intensity = i;
			break;
		}
		auxRe = auxRe * auxRe + auxRe - auxIm * auxIm ;
		auxIm = 2*auxRe*auxIm + auxIm;
		//printf( "\t\t\t [%f,%f]\n" ,auxRe,auxIm);
	}
	return intensity;
}

int parseResolution(char* str, int* res){
	char* aux ;
	if ( ( aux = strtok(str,"x")) != NULL){
		res[0] = atoi(aux);
		aux = strtok(NULL,"x");
		res[1] = atoi(aux);
	}else
		return -1;
	printf("%d \n",res[0]);
	printf("%d \n",res[1]);
	return 0;
}

int parseCenter(char* str, double* center){
	char* aux = calloc(strlen(str),sizeof(char));
	char* first;
	int i;
	int pos;
	int real = 1;
	int signR = 1;
	int signI = 1;
	if(str[0]=='-')
		signR = -1;
	for (i = 1 ; i< strlen(str) ; i++){
		if (real){
			aux[i-1] = str[i];
			if (str[i] == '-' || str[i] == '+'){
				pos = i;
				center[0]= signR * atof(aux);
				real = 0;
				memset(aux,'\0',strlen(str));
			}
		}else{
			if (str[pos] == '-')
				signI = -1;
			aux[i-pos-1] = str[i];
		}
	}
	aux[strlen(aux)-1] = '\0';
	center[1] = signI *atof(aux);
}


void version(char* name){
	printf("%s version: 1.0.0: \n",name);
}

int stop(double re, double im){
	if (sqrt((re*re)+(im*im))>2)
		return 1;
	return 0;
}

void usage(char* name){
	printf("Usage: \n");
	printf("\t %s -h --help \n",name);
	printf("\t %s -V --version \n",name);
	printf("\t %s -r --resolution, permite cambiar la resolucion de la imagen generada. El valor por defecto sera de 640x480 puntos \n",name);
	printf("\t %s -c --center, para especificar el centro de la imagen, el punto central de la porcion del plano complejo dibujada, expresado en forma binomica. El valor por defecto sera 0 + 0i  \n",name);
	printf("\t %s -w --width, especifica el ancho del rectangulo que contiene la region del plano complejo que estamos por dibujar. El valor por defecto sera 4 \n",name);
	printf("\t %s -H --height, sirve, en forma similar, para especificar el alto del rectangulo a dibujar. El valor por defecto sera 4 \n",name);
	printf("\t %s -o --output, permite colocar la imagen de salida, (en formato PGM) en el archivo pasado como argumento; o por salida estandar -cout- si el argumento es \"-\" \n",name);
	printf("Examples: \n");
	printf("\t %s -o uno.pgm \n",name);
	printf("\t %s -c +0.282-0.01i -w 0.005 -H 0.005 -o dos.pgm \n",name);
}

