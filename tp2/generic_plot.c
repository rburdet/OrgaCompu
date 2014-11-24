#include <debug.h>
#include <stdio.h>
#include <defs.h>
#include <stdint.h>
#include <param.h>

void
generic_plot(param_t *parms)
{
	uint8_t *u8;
	float cr, ci;
	float zr, zi;
	float sr, si;
	float absz;
	int x, y;
	int c;

	/* 
	 * Barremos la región rectangular del plano complejo comprendida 
	 * entre (parms->UL_re, parms->UL_im) y (parms->LR_re, parms->LR_im).
	 * El parámetro de iteración es el punto (cr, ci).
	 */
	
	ci = parms->UL_im - parms->y0 * parms->d_im;
	u8 = parms->bitmap + parms->y0 * parms->x_res;

	for (y = parms->y0; y < parms->y1; ++y) {
		cr = parms->UL_re; 

		for (x = 0; x < parms->x_res; ++x) {
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
				sr = zr * zr 
				   - zi * zi
				   + cr;
				si = 2 * zr * zi
				   + ci;
				zr = sr;
				zi = si;
			}

			/* Escribimos la intensidad del px. */
			*u8++ = (uint8_t)c;

			/* Calculamos la siguiente parte real. */
			cr += parms->d_re;
		}

		/* Calculamos la siguiente parte compleja. */
		ci -= parms->d_im;
	}
}
