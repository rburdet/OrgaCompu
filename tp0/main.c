#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

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

int parse(char* str, int* result){
	
}

void version(char* name){
	printf("%s version: 1.0.0: \n",name);
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
