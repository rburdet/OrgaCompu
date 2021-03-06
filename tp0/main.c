#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define MAX_VAL 255
#define USAGE_ERROR -1
#define IO_ERROR -2
static const char VERSION[] = "1.0.1";

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

int parseCenter(char* centerString, double* center);

/** Dadas las partes real e imaginara de un numero complejo devuelve 1 si el modulo es mayor que 2, 0 en caso contrario
 * @param double parte real de un numero complejo
 * @param double parte imaginaria de un numero complejo
 * return int 1 si es el modulo es mayor que 2, 0 si es menor o igual
 */

int stop(double re, double im);

/** Dados los parametros de interes calcula los conjuntos de mandelbrot
 * @param int* vector de 2 enteros para especificar la resolucion
 * @param double* vector de 2 doubles para especificar la coordenada de origen
 * @param double ancho del rectangulo a graficar
 * @param double alto del rectangulo a graficar
 * @param FILE* archivo donde escribe los conjuntos en formato pgm
 * return int 0 si no hubo error
 */
int print(int* res, double* center, double width , double height,FILE* file);


int main(int argc, char* argv[]){
	int opt;
	int res[2] = {640,480};
	double  width = 4;
	double  height = 4;
	double center[2] = {0,0};
	char* outDir = NULL;
	FILE* file = stdout;
	int parseCenterResult = 0;
	int parseResolutionResult = 0;
	int noArguments = 0;

	static struct option long_options[] ={
		{"version",		no_argument,			0,	'V'},
		{"help",		no_argument,			0,	'h'},
		{"resolution",	required_argument,		0,	'r'},
		{"center",		required_argument,		0,	'c'},
		{"width",		required_argument,		0,	'w'},
		{"height",		required_argument,		0,	'H'},
		{"output",		required_argument,		0,	'o'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long (argc, argv, "r:c:w:H:o:Vh", long_options, NULL)) != -1){
		switch(opt){
			case 'r':
				noArguments++;
				parseResolutionResult = parseResolution(optarg, res);
				break;
			case 'w':
				noArguments++;
				width = atof(optarg);
				break;
			case 'H':
				noArguments++;
				height = atof(optarg);
				break;
			case 'o':
				noArguments++;
				outDir = optarg;
				break;
			case 'c':
				noArguments++;
				parseCenterResult = parseCenter(optarg, center);
				break;
			case 'h':
				usage(argv[0]);
				return 0;
			case 'V':
				version(argv[0]);
				return 0;
			case '?':
				printf("Error\n");
				return USAGE_ERROR;
		}
	}

	if (noArguments == 0) {
		usage(argv[0]);
		return 0;
	}

	if (parseCenterResult == -1){
		usage(argv[0]);
		if (fprintf(stderr, "fatal: invalid center specification.\n") < 0)
			return IO_ERROR;
		return USAGE_ERROR;
	}

	if (parseResolutionResult == -1){
		usage(argv[0]);
		if (fprintf(stderr, "fatal: invalid resolution specification.\n") < 0)
			return IO_ERROR;
		return USAGE_ERROR;
	}

	if (outDir == NULL){
		return USAGE_ERROR;
	}

	if (strcmp(outDir,"-") != 0){
		file = fopen(outDir,"w");
		if (!file){
			usage(argv[0]);
			if (fprintf(stderr, "fatal: cannot open output file.\n") < 1)
				return IO_ERROR;
			return USAGE_ERROR;
		}
	}

	if (res[0] <= 0 || res[1] <= 0)
		usage(argv[0]);
	else{
		if (fprintf(file, "P2\n%d\n%d\n%d\n", res[0],res[1],MAX_VAL) < 0)
			return IO_ERROR;
			print(res,center,width,height,file);
	}

	if (file != stdout)
		if (fclose(file) < 0)
			return IO_ERROR;

	return 0;
}

int print(int* res, double* center, double width , double height,FILE* file){
	double stepX = width / res[0];
	double stepY = height/ res[1];
	int i, j, k;
	double x, y;
	double zx, zy;
	double zx2, zy2;
	x= center[0] + (-width+stepX)/2;
	y= center[1] + (height-stepY)/2;
	for ( i = 1 ; i <= res[1] ; ++i ) {
		for ( j = 1 ; j <= res[0] ; ++j) {


			zx = 0;
			zy = 0;
			zx2 = zx*zx;
			zy2 = zy*zy;
			for ( k = -1 ; k < MAX_VAL && !stop(zx,zy); k++ ){
				zy = 2*zx*zy + y;
				zx = zx2 - zy2 + x;
				zx2 = zx*zx;
				zy2 = zy*zy;
			}
			if (fprintf(file,"%d",k) < 0)
				return IO_ERROR;
			if (fputc(' ',file) < 0)
				return IO_ERROR;
			x= center[0] + (-width+2*j*stepX)/2;
		}
		y= center[1] +(height-2*i*stepY)/2;
		if (fputc('\n',file) < 0)
			return IO_ERROR;
	}
	return 0;
}

int parseResolution(char* str, int* res){
	if (strlen(str) < 3)
		return USAGE_ERROR;
	char* aux ;
	if ( ( aux = strtok(str,"x")) != NULL){
		res[0] = atoi(aux);
		aux = strtok(NULL,"x");
		res[1] = atoi(aux);
	}else
		return USAGE_ERROR;
	return 0;
}

int parseCenter(char* centerString, double* center){
	int realSign = 1;
	int complexSign = 1;
	unsigned int i = 1;
	unsigned int found = 0;
	if (centerString[strlen(centerString)-1] != 'i') {
		return USAGE_ERROR;
	}
	if (centerString[0] == '-') {
		realSign = -1;
	}
	while (found == 0 && i < strlen(centerString)) {
		if (centerString[i] == '+') {
			found = 1;
		}
		else if (centerString[i] == '-') {
			found = 1;
			complexSign = -1;
		}
		else {
			i++;
		}
	}
	if (found == 0) {
		return USAGE_ERROR;
	}
	center[0] = atof(strtok(centerString, "+-i")) * realSign;
	center[1] = atof(strtok(NULL, "+-i")) * complexSign;
	return 0;
}


void version(char* name){
	printf("%s version: %s: \n",name, VERSION);
}

int stop(double re, double im){
	if ((re*re)+(im*im)>4)
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
	printf("\t %s -r 1x1 -o - \n",name);
}
