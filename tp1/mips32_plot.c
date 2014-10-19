#include <debug.h>
#include <stdio.h>
#include <defs.h>
#include <param.h>

void
mips32_plot(param_t *parms)
{
	float cr, ci;
	float zr, zi;
	float sr, si;
	float absz;
	int x, y;
	int c;

	/* Header PGM. */
	fprintf(parms->fp, "P2\n");
	fprintf(parms->fp, "%u\n", (unsigned)parms->x_res);
	fprintf(parms->fp, "%u\n", (unsigned)parms->y_res);
	fprintf(parms->fp, "%u\n", (unsigned)parms->shades);

	/* 
	 * Barremos la región rectangular del plano complejo comprendida 
	 * entre (parms->UL_re, parms->UL_im) y (parms->LR_re, parms->LR_im).
	 * El parámetro de iteración es el punto (cr, ci).
	 */
	for (y = 0, ci = parms->UL_im; 
	     y < parms->y_res; 
	     ++y, ci -= parms->d_im) {
		for (x = 0, cr = parms->UL_re; 
		     x < parms->x_res; 
		     ++x, cr += parms->d_re) {
			zr = cr;
			zi = ci;

			/*
			 * Determinamos el nivel de brillo asociado al punto
			 * (cr, ci), usando la fórmula compleja recurrente 
			 * f = f^3 + c.
			 */
			for (c = 0; c < parms->shades; ++c) {
				if ((absz = zr*zr + zi*zi) >= 4.0f)
					break;
				sr = zr * zr * zr 
				   - 3 * zi * zi * zr
				   + cr;
				si = 3 * zr * zr * zi
				   - zi * zi * zi
				   + ci;
				zr = sr;
				zi = si;
			}

			if (fprintf(parms->fp, "%u\n", (unsigned)c) < 0) {
				fprintf(stderr, "i/o error.\n");
				exit(1);
			}
		}
	}

	/* Flush any buffered information before quit. */
	if (fflush(parms->fp) != 0) {
		fprintf(stderr, "cannot flush output file.\n");
		exit(1);
	}
}
