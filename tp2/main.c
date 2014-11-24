#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

#include <defs.h>
#include <debug.h>
#include <param.h>

#ifndef VERSION
#define VERSION "0.0.1-cvs"
#endif

#ifndef no_argument
#define no_argument 0
#endif

#ifndef required_argument
#define required_argument 1
#endif

#ifndef optional_argument
#define optional_argument 2
#endif

static void pthreads_plot(void);
extern void generic_plot(param_t *);
extern void sse_plot(param_t *);

/*
 * Parámetros globales.
 */

int x_res = 640;		/* Ancho de imagen por defecto. */
int y_res = 480;		/* Alto de imagen, por defecto. */
float upper_left_re = +0.2795;	/* Extremo superior izquierzo (re). */
float upper_left_im = -0.0075;	/* Extremo superior izquierzo (im). */
float lower_right_re = +0.2845;	/* Extremo inferior derecho (re). */
float lower_right_im = -0.0125;	/* Extremo inferior derecho (im). */
size_t nthreads = 1;            /* Cantidad de threads de cómputo. */
FILE *output;
void (*plot)(param_t *);

typedef struct {
	const char *name;
	void (*plot)(param_t *);
} METHOD;

static METHOD methods[] = {
#ifdef USE_SSE_ASSEMBLY
	{ "sse", sse_plot },
#endif
#ifdef USE_MIPS32_ASSEMBLY
	{ "mips32", mips32_plot },
#endif
	{ "generic", generic_plot },
	{ NULL, NULL, }
};

static void parse_cmdline(int, char * const []);
static void do_usage(const char *, int);
static void do_version(const char *);
static void do_resolution(const char *, const char *);
static void do_geometry(const char *, const char *);
static void do_center(const char *, const char *);
static void do_width(const char *, const char *);
static void do_height(const char *, const char *);
static void do_nthreads(const char *, const char *);
static void do_method(const char *, const char *);
static void do_output(const char *, const char *);

int
main(int argc, char * const argv[], char * const envp[])
{
	parse_cmdline(argc, argv);
	pthreads_plot();

	return 0;
}

static void
parse_cmdline(int argc, char * const argv[])
{
	int ch;
	int index = 0;

	struct option options[] = {
		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'V'},
		{"geometry", required_argument, NULL, 'g'},
		{"resolution", required_argument, NULL, 'r'},
		{"center", required_argument, NULL, 'c'},
		{"width", required_argument, NULL, 'w'},
		{"height", required_argument, NULL, 'H'},
		{"method", required_argument, NULL, 'm'},
		{"nthreads", required_argument, NULL, 'n'},
		{"output", required_argument, NULL, 'o'},
	};

	while ((ch = getopt_long(argc, argv, 
	                         "hc:H:m:n:o:r:w:g:V", options, &index)) != -1) {
		switch (ch) {
		case 'h':
			do_usage(argv[0], 0);
			break;
		case 'V':
			do_version(argv[0]);
			break;
		case 'g':
			do_geometry(argv[0], optarg);
			break;
		case 'r':
			do_resolution(argv[0], optarg);
			break;
		case 'c':
			do_center(argv[0], optarg);
			break;
		case 'w':
			do_width(argv[0], optarg);
			break;
		case 'H':
			do_height(argv[0], optarg);
			break;
		case 'm':
			do_method(argv[0], optarg);
			break;
		case 'n':
			do_nthreads(argv[0], optarg);
			break;
		case 'o':
			do_output(argv[0], optarg);
			break;
		default:
			do_usage(argv[0], 1);
		}
	}

	if (plot == NULL)
		do_method(argv[0], "generic");
	if (!output) {
		fprintf(stderr, "no output file.\n");
		exit(1);
	}
}

static void
do_usage(const char *name, int status)
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "  %s -h\n", name);
	fprintf(stderr, "  %s -V\n", name);
	fprintf(stderr, "  %s [options]\n", name);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -r, --resolution "
	                " Set bitmap resolution to WxH pixels.\n");
	fprintf(stderr, "  -c, --center     "
	                " Set coordinates for the center of the image.\n");
	fprintf(stderr, "  -w, --width      "
	                " Change the width of the spanned region.\n");
	fprintf(stderr, "  -H, --height     "
	                " Change the height of the spanned region.\n");
	fprintf(stderr, "  -o, --output     "
	                " Path to output file.\n");
	fprintf(stderr, "Examples:\n");
	fprintf(stderr, "  %s -o output.pgm\n", name);
	fprintf(stderr, "  %s -r 1600x1200 -o output.pgm\n", name);
	fprintf(stderr, "  %s -c +0.282+0.01i -o output.pgm\n", name);
	exit(status);
}

static void
do_version(const char *name)
{
	fprintf(stderr, "%s\n", VERSION);
	exit(0);
}

static void
do_resolution(const char *name, const char *spec)
{
	int x;
	int y;
	char c;
	char d;

	if (sscanf(spec, "%d%c%d %c", &x, &c, &y, &d) != 3
	    || x <= 0
	    || c != 'x'
	    || y <= 0)
		do_usage(name, 1);

	/* Set new resolution. */
	x_res = x;
	y_res = y;
}

static void
do_geometry(const char *name, const char *spec)
{
	double re_1, im_1;
	double re_2, im_2;
	char comma;
	char sg_1;
	char sg_2;
	char ii_1;
	char ii_2;
	char ch;

#define PLUS_OR_MINUS(c)  ((c) == '+' || (c) == '-')
#define IMAGINARY_UNIT(x) ((x) == 'i' || (x) == 'j')

	if (sscanf(spec, 
	           "%lf %c %lf %c %c %lf %c %lf %c %c", 
	           &re_1,
	           &sg_1,
	           &im_1,
	           &ii_1,
	           &comma,
	           &re_2,
	           &sg_2,
	           &im_2,
	           &ii_2,
	           &ch) != 9
	    || !PLUS_OR_MINUS(sg_1)
	    || !PLUS_OR_MINUS(sg_2)
	    || !IMAGINARY_UNIT(ii_1)
	    || !IMAGINARY_UNIT(ii_2)
	    || comma != ',') {
		fprintf(stderr, "invalid geometry specification.\n");
		exit(1);
	}

#define MINIMUM(x, y) ((x) <= (y) ? (x) : (y))
#define MAXIMUM(x, y) ((x) >= (y) ? (x) : (y))
#define SIGN(c) ((c) == '-' ? -1.0 : +1.0)

	/* Sign-adjust. */
	im_1 *= SIGN(sg_1);
	im_2 *= SIGN(sg_2);

	/*
	 * We have two edges of the rectangle. Now, find the upper-left 
	 * (i.e. the one with minimum real part and maximum imaginary
	 * part) and lower-right (maximum real part, minimum imaginary)
	 * corners of the rectangle.
	 */
	upper_left_re = MINIMUM(re_1, re_2);
	upper_left_im = MAXIMUM(im_1, im_2);
	lower_right_re = MAXIMUM(re_1, re_2);
	lower_right_im = MINIMUM(im_1, im_2);
}

static void
do_center(const char *name, const char *spec)
{
	double width;
	double height;
	double re, im;
	char ii;
	char sg;
	char ch;

	if (sscanf(spec, 
	           "%lf %c %lf %c %c", 
	           &re,
	           &sg,
	           &im,
	           &ii,
	           &ch) != 4
	    || !PLUS_OR_MINUS(sg)
	    || !IMAGINARY_UNIT(ii)) {
		fprintf(stderr, "invalid center specification.\n");
		exit(1);
	}

	im *= SIGN(sg);
	width = fabs(upper_left_re - lower_right_re);
	height = fabs(upper_left_im - lower_right_im);

	upper_left_re = re - width / 2;
	upper_left_im = im + height / 2;
	lower_right_re = re + width / 2;
	lower_right_im = im - height / 2;
}

static void
do_height(const char *name, const char *spec)
{
	double width;
	double height;
	double re, im;
	char ch;

	if (sscanf(spec, 
	           "%lf %c", 
	           &height,
	           &ch) != 1
	    || height <= 0.0) {
		fprintf(stderr, "invalid height specification.\n");
		exit(1);
	}

	re = (upper_left_re + lower_right_re) / 2;
	im = (upper_left_im + lower_right_im) / 2;
	width = fabs(upper_left_re - lower_right_re);

	upper_left_re = re - width / 2;
	upper_left_im = im + height / 2;
	lower_right_re = re + width / 2;
	lower_right_im = im - height / 2;
}

static void
do_nthreads(const char *name, const char *spec)
{
	int nt;
	char c;

	if (sscanf(spec, "%d %c", &nt, &c) != 1 
	    || nt <= 0)
		do_usage(name, 1);

	/* Set new resolution. */
	nthreads = nt;
}

static void
do_width(const char *name, const char *spec)
{
	double width;
	double height;
	double re, im;
	char ch;

	if (sscanf(spec, 
	           "%lf %c", 
	           &width,
	           &ch) != 1
	    || width <= 0.0) {
		fprintf(stderr, "invalid width specification.\n");
		exit(1);
	}

	re = (upper_left_re + lower_right_re) / 2;
	im = (upper_left_im + lower_right_im) / 2;
	height = fabs(upper_left_im - lower_right_im);

	upper_left_re = re - width / 2;
	upper_left_im = im + height / 2;
	lower_right_re = re + width / 2;
	lower_right_im = im - height / 2;
}

static void
do_method(const char *name, const char *spec)
{
	METHOD *m;

#define STREQ(x, y) (strcmp((x), (y)) == 0)
	for (m = methods; m->plot; ++m) {
		if (STREQ(spec, m->name)) {
			plot = m->plot;
		}
	}

	if (!plot) {
		fprintf(stderr, "no such method.\n");
		exit(1);
	}
}

static void
do_output(const char *name, const char *spec)
{
	if (output != NULL) {
		fprintf(stderr, "multiple do output files.");
		exit(1);
	}

	if (strcmp(spec, "-") == 0) {
		output = stdout;
	} else {
		if (!(output = fopen(spec, "w"))) {
			fprintf(stderr, "cannot open output file.\n");
			exit(1);
		}
	}
}

struct thread_info {
	pthread_t tid;
	param_t parms;
};

static void
pthreads_plot(void)
{
	struct thread_info *tinfo;
	const char *sep = "";
	size_t length = 0;
	param_t parms;
	uint8_t *u8;
	size_t y0;
	size_t yd;
	size_t tn;
	size_t y;
	size_t x;
	int n;

	memset(&parms, 0, sizeof(parms));
	parms.UL_re = upper_left_re;
	parms.UL_im = upper_left_im;
	parms.LR_re = lower_right_re;
	parms.LR_im = lower_right_im;
	parms.d_re = (lower_right_re - upper_left_re) / x_res;
	parms.d_im = (upper_left_im - lower_right_im) / y_res;
	parms.x_res = x_res;
	parms.y_res = y_res;
	parms.shades = 255;
	parms.y0 = 0;
	parms.y1 = y_res;

#ifndef SZ
#define SZ(x) sizeof(x)
#endif

	if ((parms.bitmap = u8 = malloc(SZ(uint8_t) * x_res * y_res)) == 0) {
		fprintf(stderr, "cannot allocate memory.\n");
		exit(1);
	}

	if ((tinfo = malloc(SZ(struct thread_info) * nthreads)) == NULL) {
		fprintf(stderr, "cannot allocate memory.\n");
		exit(1);
	}

	y0 = 0;
	yd = y_res / nthreads;

	for (tn = 0; tn < nthreads; ++tn) {
		memcpy(&tinfo[tn].parms, &parms, SZ(parms));
		tinfo[tn].parms.y0 = y0;
		tinfo[tn].parms.y1 = tn < nthreads - 1
		                     ? (y0 += yd)
		                     : (y_res);

		if (pthread_create(&tinfo[tn].tid, 
		                   NULL, 
		                   (void *)plot,
		                   &tinfo[tn].parms) != 0) {
			fprintf(stderr, "cannot create thread.\n");
			exit(1);
		}
	}

	for (tn = 0; tn < nthreads; ++tn) {
		if (pthread_join(tinfo[tn].tid, NULL) != 0) {
			fprintf(stderr, "cannot join thread.\n");
			exit(1);
		}
	}

	/* Header PGM. */
	fprintf(output, "P2\n");
	fprintf(output, "%u\n", (unsigned)parms.x_res);
	fprintf(output, "%u\n", (unsigned)parms.y_res);
	fprintf(output, "%u\n", (unsigned)parms.shades);

	for (y = 0; y < parms.y_res; ++y)
	for (x = 0; x < parms.x_res; ++x) {
		/*
		 * XXX Write this pixel. Try to detect stream errors as 
		 * soon as possible. Stick with the PGM recommendation 
		 * against lines longer than 70 chars.
		 */
		if ((n = fprintf(output,
		                 "%s%u",
		                 sep,
		                 (unsigned) *u8++)) < 0) {
			fprintf(stderr, "i/o error.\n");
			exit(1);
		}

		if ((length += n) >= 65) {
			ASSERT(length < 70);
			if (fprintf(output, "\n") < 0) {
				fprintf(stderr, "i/o error.\n");
				exit(1);
			}

			length = 0;
			sep = "";
		} else {
			sep = " ";
		}
	}

	/* Flush any buffered information before quit. */
	if (fflush(output) != 0) {
		fprintf(stderr, "cannot flush output file.\n");
		exit(1);
	}

	/* Deallocate memory. */
	if (parms.bitmap != NULL)
		free(parms.bitmap), parms.bitmap = NULL;
}
