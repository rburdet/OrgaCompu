#include <mips/regdef.h>
#include <sys/syscall.h>

#mips32_plot(param_t *);
	.text
	.align	2
	.globl	mips32_plot
	.ent	mips32_plot
mips32_plot:
	.frame	$fp, 32, ra
	.set	noreorder
	.cpload	t9
	.set	reorder
	subu	sp, sp, 32
	.cprestore 20
	sw	ra,  28(sp)
	sw	$fp, 24(sp)
	move	$fp, sp
	sw	a0,	32(sp)
	sw	a1,	36(sp)
	sw	a2,	40(sp)
	sw	a3,	44(sp)

	#ESTO FUNCIONA =)
	sw	a0,32(sp)	#salvo a0 en stack
	la	t0,0(a0)
	lw	t0,36(t0)	#cargo en t0 el FILE*
	lhu	t0,14(t0)	#me muevo al campo _file mediante un short
	sw	t0,48(sp)	#guardo el file descriptor 

	#mock para pruebas 
	#define	UL_im	-2.0
	#define y_res	480
	#define	UL_re	-2.0
	#define d_im	4

	move	t3,zero
	li	s3,255

d_zero : .double 0.0
d_limit : .double 4.0
d_three : .double 3.0
	
	l.d	$f0,d_zero	#f0 cr
	l.d	$f2,d_zero	#f2 ci
	l.d	$f4,d_zero	#f4 zr
	l.d	$f6,d_zero	#f6 zi
	l.d	$f8,d_zero	#f8 sr
	l.d	$f10,d_zero	#f10 si
	l.d	$f12,d_zero	#f12 absz
	l.d	$f14,d_limit#f14 4.00
	l.d $f16,d_zero	#f16 registro temporal1
	l.d $f18,d_three#f18 registro temporal2

	#esto no compila y no se xq 
	#c.gt.d	$f12,$f14
	bc1t	loop2
	mul.d	$f16,$f4,$f4	#tmp=zr*zr
	mul.d	$f16,$f16,$f4	#tmp=zr*zr*zr
	add.d	$f18,$f16,$f0	#sr =zr*zr*zr+cr
	mul.d	$f16,$f6,$f6	#tmp=zi*zi
	mul.d	$f16,$f16,$f6	#tmp=zi*zi*zi
	mul.d	$f16,$f18,$f16	#tmp=zi*zi*zi*3
	add.d	$f18,$f18,$f16	#sr=todo el resto



loop2:
	#


	







	la	a0, msg
	jal	mystrlen
	sw      v0, 8(sp)

	# Llamamos a write(2) para imprimir, ignorando el valor 
	# de retorno, y notando que los registros se preservan 
	# al cruzar la interfaz de llamadas al sistema, algo que
        # no ocurre en el caso de llamadas a procedimientos.
	#

	#con esto imprimo texto
	#li v0,SYS_write
	#li a0,0
	#la a1,msg
	#lw a2,8(sp)
	#syscall

	#PRINTF FUNCIONANDO, imprime t0!! 
miprintf:
	la	a0,print
	move	a1,t0
	lw	t9,%call16(printf)(gp)
	jalr	t9


	# Volvemos al sistema operativo, devolviendo un código 
	# de retorno nulo.
	#
	move	v0, zero
	lw	ra,  28(sp)
	lw	$fp, 24(sp)
	addu	sp, sp, 32
	j	ra
	.end	mips32_plot

	.ent	mystrlen
mystrlen:
	.frame	$fp, 16, ra
	.set	noreorder
	.cpload	t9
	.set	reorder
	subu	sp, sp, 16
	.cprestore 0
	sw	$fp, 4(sp)
	move	$fp, sp

	# Use v0 for the result.
	#
	li	v0, 0
mystrlen_loop:
	lb	t0, 0(a0)
	beqz	t0, mystrlen_return
	addiu	a0, a0, 1
	addiu	v0, v0, 1
	j	mystrlen_loop

mystrlen_return:
	# Destruimos el frame.
	#
	lw	$fp, 4(sp)
	addu	sp, sp, 16

	# Retorno.
	#
	j	ra
	.end	mystrlen

	lw	ra,  28(sp)
	lw	$fp, 24(sp)
	move	sp, $fp
	lw	a0,	32(sp)
	lw	a1,	36(sp)
	lw	a2,	40(sp)
	lw	a3,	44(sp)
	addu	sp,sp,32


	.rdata
msg:
	.asciiz	"Hola mundo.\n"

print:
	.asciiz "valor : %d \n"
