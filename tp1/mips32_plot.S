# Similar a 00-holamundo.S (imprime un mensaje por salida 
# estándar, usando write(2)); pero, en este caso, vamos a
# calcular dinámicamente la longitud del string impreso.
#
# Para esto, escribimos un método, mystrlen(), que opera sobre
# un string, calculando y retornando, su longitud.
#
# Además, y debido a que este programa hace llamadas a código
# en funciones, deberemos respetar la ABI usada en el sistema.
#
# $Date: 2008/04/21 17:24:06 $

#include <mips/regdef.h>
#include <sys/syscall.h>

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


	#salvo a0
	sw	a0,32(sp)
	lw	t1,32(sp)
	# Calculamos la longitud del string.
	#
	la	a0, msg
	jal	mystrlen
	sw      v0, 8(sp)

	# Llamamos a write(2) para imprimir, ignorando el valor 
	# de retorno, y notando que los registros se preservan 
	# al cruzar la interfaz de llamadas al sistema, algo que
        # no ocurre en el caso de llamadas a procedimientos.
	#
	#li	v0, 1 # ver dentro de <sys/syscall.h>.
	#li	a0, 1         # a0: standard output file descriptor.
	#li	a1, 2       # a1: data pointer.
	#li	a2, 1     # a2: longitud, según mystrlen().

	li v0,SYS_write
	lw a0,32(sp)
	la a1,msg
	lw a2,8(sp)
	syscall

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

	.rdata
msg:
	.asciiz	"Hola mundo.\n"
