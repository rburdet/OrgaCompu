#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

void usage(char* nombre);
void version(char* nombre);
int parseWidth(char* optArg);
int parseHeight(char* optArg);

//void print_board(FILE* file, int width, int height);

int main(int argc, char* argv[]){
	int opt;
	int width=640;
	int height=480;

	static struct option long_options[] ={
		{"resolution",	optional_argument,		0,	'r'},
		{"center",		optional_argument,		0,	'c'},
		{"width",		optional_argument,		0,	'w'},
		{"height",		optional_argument,		0,	'H'},
		{"output",		optional_argument,		0,	'o'},
		{"version",		optional_argument,		0,	'V'},
		{"help",		optional_argument,		0,	'h'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long (argc, argv, "r:c:w:H:o:V:h:", long_options, NULL)) != -1){
		switch(opt){
			case 'r':
				width = parseWidth(optarg);
				height = parseHeight(optarg);
				break;
			case '?':
				printf("Error\n");
				break;
		}
	}
}

int parseWidth(char* optArg){
	printf("%s \n",optArg);
	return 0;
}

int parseHeight(char* optArg){
	printf("%s \n",optArg);
	return 0;
}
