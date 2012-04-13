#!/bin/bash

# script criado para facilitar a execucao de programas escritos na linguagem at.
# o script abrevia toda a parte de criaçao do .asm, .o e execuçao.
# cuidado a usar, primeiro shell script =)

AT_DIR=/home/klang/Desktop/061/run_process/tests/
COMPILER_DIR=/home/klang/Desktop/061/at

at_execute()
{
    program=$1
    
    if [ -z $program ]
    then
	    echo 'usage: ./at.sh ProgramName :D Good Luck!!'
    else
     	$COMPILER_DIR $AT_DIR$program.at            # compilar (compilador at =) )
     	yasm -felf $AT_DIR$program.asm		        # gerar object (.o)
	    mv $program.o $AT_DIR			            # mover .o para directorio do codigo 
	    ld -o $program $AT_DIR$program.o -lrts		# gerar executavel
	    mv $program $AT_DIR			                # mover executavel para directorio do codigo
	    rm $AT_DIR$program.o			            # remover object (.o)
	    rm $AT_DIR$program.asm
	    $AT_DIR$program                             # executar o programa
    fi
}

at_execute $1
