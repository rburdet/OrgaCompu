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
	li a2,3
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
	li a2,1
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


	#imprimo shades
	lw	t0,FRAME_SIZE($fp)
	lw	t1,FD(t0)
	#t1 = FD
	lh	t1,descriptor(t1)	#me muevo al campo _file mediante un short
	lw	t0,shades(t0)
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
	sw	t0,LTA_POS($fp)		#guardo y=0 -> 0
	lw	t0,FRAME_SIZE($fp)	#traigo parms
	l.s	$f4,UL_im(t0)		#f4=UL_im
	s.s	$f4,LTA_POS+4($fp)	#ci=UL_im -> 4
loop_y:

	#cargo y_res
	lw	t0,FRAME_SIZE($fp)	#traigo parms
	lw	t0,y_res(t0)	#t0 = parms->y_res
	lw	t1,LTA_POS($fp)	#t1 = y
    bge t1, t0, end	# if (y > y_res) {end}

	#inicializo loop_x
	li	t0,0
	sw	t0,LTA_POS+8($fp)	#guardo x=0 -> 8
	lw	t0,FRAME_SIZE($fp)	#traigo parms
	l.s	$f4,UL_re(t0)		#f4 = UL_re
	s.s	$f4,LTA_POS+12($fp)	#cr=UL_re -> 12

loop_x:
	#cargo x_res
	lw	t0,FRAME_SIZE($fp)	#traigo parm
	lw	t0,x_res(t0)		#t0 = parms->x_res
	lw	t1,LTA_POS+8($fp)	#t1 = x
	bge	t1,t0,inc_y			#if (x>x_rs){inc y}

	l.s	$f4,LTA_POS+4($fp)	#f4 = ci
	l.s	$f6,LTA_POS+12($fp)
	s.s	$f4,LTA_POS+16($fp)	# zi=ci -> 16
	s.s	$f6,LTA_POS+20($fp) # zr=cr -> 20

	#inicializo loop_c
	li	t0,0
	sw	t0,LTA_POS+24($fp)	# c -> 24

loop_c:

	###### ES UN ++c

	lw	t1,FRAME_SIZE($fp)	#traigo parms 
	lw	t1,shades(t1)	#t1 = parms->shades
	bge	t0,t1,write_out	# if (c>parms->shades) {write_out}

	l.s $f4, LTA_POS+20($fp)	#f4 = zr 
	l.s $f6, LTA_POS+16($fp)	#f6 = zi 
	mul.s $f4, $f4, $f4
	mul.s $f6, $f6, $f6
	add.s $f4, $f4, $f6
	li.s $f6, 4.0
	c.le.s $f6, $f4	# if 4<absz then write_out
	bc1t write_out

	l.s $f4, LTA_POS+20($fp) # f4 = zr
	mul.s $f6, $f4, $f4	# f6 = zr^2
	mul.s $f6, $f6, $f4 # f6 = zr ^ 3
	l.s $f8, LTA_POS+16($fp)	#f8 = zi
	mul.s $f8, $f8, $f8			#f8=zi = zi ^ 2
	mul.s $f8, $f4, $f8			#f8 = zi ^ 2 * zr
	li.s $f10, 3.0				#f10 = 3
	mul.s $f8, $f8, $f10		#f8 = zi^2*zr*3
	sub.s $f6, $f6, $f8			#f6 = zr^3 - zi^2*zr*3
	l.s $f8, LTA_POS+12($fp)	#f8 = cr
	add.s $f6, $f6, $f8			#f6 = zr^3 - zi^2*zr*3 + cr
	s.s $f6, LTA_POS+28($fp)	#sr -> 28

	l.s	$f4,LTA_POS+20($fp) # f4 = zr
	l.s $f6,LTA_POS+LTA_POS+16($fp) #f6 = zi
	li.s $f8,3.0
	mul.s $f4,$f4,$f4	#zr = zr^2
	mul.s $f4,$f4,$f6	#f4 = zr^2*zi
	mul.s $f4,$f4,$f8	#f4 = zr^2*zi*3

	mul.s $f8,$f6,$f6	#f8 = zi*zi
	mul.s $f6,$f8,$f6	#f6 = zi*zi*zi
	sub.s $f4,$f4,$f6	#f4 = 3*zr^2 *zi - zi^3

	l.s	$f6,LTA_POS+4($fp)
	add.s $f4, $f4,$f6
	s.s $f4,LTA_POS+32($fp)

	l.s $f4, LTA_POS+28($fp)	#f4 = sr
	l.s $f6, LTA_POS+32($fp)
	s.s $f4, LTA_POS+20($fp)	#sr -> 20
	s.s $f6, LTA_POS+16($fp)

    lw  t0,LTA_POS+24($fp)
	addi	t0,1 
	sw	t0,LTA_POS+24($fp)
	j	loop_c


write_out:
	lw t0, LTA_POS+24($fp)
	lw t1, FRAME_SIZE($fp)
	lw t1, FD(t1)
	lh t1, descriptor(t1)
	move a0, t0
	move a1, t1
	jal write_int



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

	j inc_x

inc_x:
	lw	t0,LTA_POS+8($fp)	#x
	addi	t0,1
	sw	t0,LTA_POS+8($fp)
	lw	t1,FRAME_SIZE($fp)
	l.s	$f4,d_re(t1)	#f4 = parms->d_re
	l.s $f6,LTA_POS+12($fp)	#cr
	add.s $f6,$f6,$f4
	s.s $f6,LTA_POS+12($fp)
	j	loop_x

inc_y:
	lw	t0,LTA_POS($fp)	#y
	addi	t0,1
	sw	t0,LTA_POS($fp)
	lw	t1,FRAME_SIZE($fp)
	l.s	$f4,d_im(t1)	#f4 = parms->d_im
	l.s $f6,LTA_POS+4($fp)	#ci
	sub.s $f6,$f6,$f4
	s.s $f6,LTA_POS+4($fp)
	j	loop_y


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
