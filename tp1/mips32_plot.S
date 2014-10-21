#include <mips/regdef.h>
#include <sys/syscall.h>

#defino las posiciones de los argumentos dentro del struct paramt_t
#define UL_re 0
#define UL_im 4
#define LR_re 8
#define LR_im 12
#define d_re 16
#define d_im 20
#define x_res 24
#define y_res 28
#define shades 32
#define FD 36

#define descriptor 14



#define FRAME_SIZE	72
#define GP_POS		FRAME_SIZE-16
#define FP_POS		FRAME_SIZE-12
#define RA_POS		FRAME_SIZE-8
#define LTA_POS		16


#mips32_plot(param_t *);
	.text
	.align	2
	.extern write_int
	.globl	mips32_plot
	.ent	mips32_plot
mips32_plot:
	.frame	$fp, FRAME_SIZE, ra
	.set	noreorder
	.cpload	t9
	.set	reorder
	subu	sp, sp, FRAME_SIZE
	.cprestore GP_POS
	sw	$fp, FP_POS(sp)
	sw	ra,  RA_POS(sp)
	move	$fp, sp
	sw	a0,	FRAME_SIZE($fp)
	sw	a1,	FRAME_SIZE+4($fp)
	sw	a2,	FRAME_SIZE+8($fp)
	sw	a3,	FRAME_SIZE+12($fp)

	#t0 = &parms
	lw	t0,FRAME_SIZE($fp)
	lw	t1,FD(t0)
	#t1 = FD
	lh	t1,descriptor(t1)	#me muevo al campo _file mediante un short
	
	#imprimir cabecera
	#imprimo P2
	li	v0,SYS_write
	move	a0,t1
	la a1,msgP2
	li a2,4
	syscall
	#TODO: revisar error

	#imprimo xres
	lw	t0,FRAME_SIZE($fp)
	lw	t1,FD(t0)
	#t1 = FD
	lh	t1,descriptor(t1)	#me muevo al campo _file mediante un short
	lw	t0,x_res(t0)
	move	a0,t0
	move	a1,t1
	jal	write_int


	#Imprimir fin de linea
	lw	t0,FRAME_SIZE($fp)
	lw	t1,FD(t0)
	lh	t1,descriptor(t1)	#me muevo al campo _file mediante un short

	#t1 = FD
	li	v0,SYS_write
	move	a0,t1
	la a1,endln
	li a2,2
	syscall


	#imprimo yres
	lw	t0,FRAME_SIZE($fp)
	lw	t1,FD(t0)
	#t1 = FD
	lh	t1,descriptor(t1)	#me muevo al campo _file mediante un short
	lw	t0,y_res(t0)
	move	a0,t0
	move	a1,t1
	jal	write_int


	#Imprimir fin de linea
	lw	t0,FRAME_SIZE($fp)
	lw	t1,FD(t0)
	lh	t1,descriptor(t1)	#me muevo al campo _file mediante un short
	#t1 = FD
	li	v0,SYS_write
	move	a0,t1
	la a1,endln
	li a2,1
	syscall

	#inicializo loop_y
	li	t0,0
	sw	t0,LTA_POS($fp)		#guardo y
	lw	t0,FRAME_SIZE($fp)
	l.s	$f4,UL_im(t0)
	s.s	$f4,LTA_POS+4($fp)	#ci
loop_y:
	#cargo y_res
	lw	t0,FRAME_SIZE($fp)
	lw	t0,y_res(t0)	#t0 = parms->y_res
	lw	t1,LTA_POS($fp)	#t1 = y
	bge	t1,t0,end

	#inicializo loop_x
	li	t0,0
	sw	t0,LTA_POS+8($fp)	#guardo x
	lw	t0,FRAME_SIZE($fp)
	l.s	$f4,UL_re(t0)
	s.s	$f4,LTA_POS+12($fp)	#cr
loop_x:
	#cargo x_res
	lw	t0,FRAME_SIZE($fp)
	lw	t0,x_res(t0)	#t0 = parms->x_res
	lw	t1,LTA_POS+8($fp)	#t1 = x
	bge	t1,t0,inc_y

	l.s	$f4,LTA_POS+4($fp)
	l.s	$f6,LTA_POS+12($fp)
	s.s	$f4,LTA_POS+16($fp)	# zi
	s.s	$f6,LTA_POS+20($fp) # zr

	#inicializo loop_c
	li	t0,0
	sw	t0,LTA_POS+24($fp)	# c
	lw	t1,FRAME_SIZE($fp)
	lw	t1,shades(t1)	#t1 = parms->shades
	bge	t1,t0,inc_x

loop_c:
	li		a0,0
	#t0 = &parms
	lw	t0,FRAME_SIZE($fp)
	lw	t1,FD(t0)
	#t1 = FD
	lh	t1,descriptor(t1)	#me muevo al campo _file mediante un short
	move	a1,t1
	jal	write_int


inc_x:
inc_y:
##############
	#l.s	$f0,d_zero	#f0 cr
	#l.s	$f2,d_zero	#f2 ci
	#l.s	$f4,d_zero	#f4 zr
	#l.s	$f6,d_zero	#f6 zi
	#l.s	$f8,d_zero	#f8 sr
	#l.s	$f10,d_zero	#f10 si
	#l.s	$f12,d_zero	#f12 absz
	#l.s	$f14,d_limit#f14 4.00
	#l.s $f16,d_zero	#f16 registro temporal1=0
	#l.s $f18,d_three#f18 registro temporal2=3
#
#	mul.s	$f16,$f4,$f4	#tmp = zr*zr
#	mov.s	$f12,$f16		#absz=zr*zr
#	mul.s	$f16,$f6,$f6	#tmp = zi*zi
#	add.s	$f12,$f12,$f16	#absz = zr*zr + zi*zi
#
#	c.lt.s	$f14,$f12		#if 4.00 < absz {set flag}
#	bc1t	loop2
#	mul.s	$f16,$f4,$f4	#tmp=zr*zr
#	mul.s	$f16,$f16,$f4	#tmp=zr*zr*zr
#	add.s	$f18,$f16,$f0	#sr =zr*zr*zr+cr
#	mul.s	$f16,$f6,$f6	#tmp=zi*zi
#	mul.s	$f16,$f16,$f6	#tmp=zi*zi*zi
#	mul.s	$f16,$f18,$f16	#tmp=zi*zi*zi*3
#	add.s	$f8,$f18,$f16	#sr=todo el resto
#
#	mul.s	$f16,$f18,$f4	#tmp1 = 3*zr
#	mul.s	$f16,$f16,$f4	#tmp1 = 3*zr*zr
#	mul.s	$f16,$f16,$f6	#tmp1 = 3*zr*zr*zi
#	add.s	$f10,$f2,$f16	#si=3*zr*zr*zi+ci
#	mul.s	$f16,$f6,$f6	#tmp = zi*zi
#	mul.s	$f16,$f16,$f6	#tmp = zi*zi*zi
#	sub.s	$f10,$f10,$f16	#si = 3*zr*zr*zi+ci-zi*zi*zi
#
#	mov.s	$f4,$f8			#zr = sr
#	mov.s	$f6,$f10		#zi = si






	#PRINTF FUNCIONANDO, imprime t0!! 
miprintf:
	la	a0,print
	move	a1,t0
	lw	t9,%call16(printf)(gp)
	jalr	t9

	#PRINTF DE FLOTS FUNCIONANDO, CARGAR FLOAT CON l.s en $f0
printff:
	cvt.d.s $f0,$f0
	la	a0,print_float
	mfc1	a2,$f0
	mfc1	a3,$f1
	lw	t9,%call16(printf)(gp)
	jalr	t9

end:
	lw	ra,RA_POS($fp)
	lw	gp,GP_POS($fp)
	lw	$fp,FP_POS($fp)
	addu	sp,sp,FRAME_SIZE
	jr	ra

	.end	mips32_plot


	.rdata
endln:
	.asciiz	"\n"
msgP2:
	.asciiz	"P2\n"
space:
	.asciiz	" "


print:
	.asciiz "valor : %d \n"
print_float:
	.asciiz "valor float  : %f \n"
