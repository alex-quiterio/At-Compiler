#===============================================================================
            #=========================================#
            #            Baterias de testes           #
            #=========================================#
# Directorio do output
OUT_DIR=/home/klang/Desktop/061/run_process/User_OutPut

# Directorio do output experado
EXP_DIR=/home/klang/Desktop/061/run_process/expected/

# Numero e tipo de testes
TOTAL=1
TESTS_A=8
TESTS_B=27
TESTS_C=20
TESTS_D=3
TESTS_E=11
TESTS_F=4
TESTS_J=9
TESTS_K=11
TESTS_L=3
TESTS_M=2
TESTS_O=3
TESTS_P=7
TESTS_Q=7
TESTS_U=6
TESTS_W=3
#===============================================================================
#=========================================#
#            GAMA TESTES TOTAL            #
#=========================================#
test_case()
{
    MAX_SIZE=$1
    TEST_NAME=$2
      echo "================Bateria de testes de $TEST_NAME ========================="
      echo ""
    for ((  i = 1 ;  i <= $MAX_SIZE;  i++, TOTAL++  ))
    do
      echo "Welcome to test number $TOTAL!"
      echo "Enjoy it :)"

      
      if [ $i -lt 10 ]
        then ./at.sh $TEST_NAME-0$i-$TOTAL-N-ok > $TEST_NAME-0$i-$TOTAL-N-ok-user.out  
        else  
            if [ $i -eq 18 -a $TEST_NAME = "B" ]
                then i=20
            fi
      ./at.sh $TEST_NAME-$i-$TOTAL-N-ok > $TEST_NAME-0$i-$TOTAL-N-ok-user.out  
      fi
        mv  $TEST_NAME-0$i-$TOTAL-N-ok-user.out $OUT_DIR
    done
}

#===============================================================================
#=========================================#
#            GAMA DIFFS TOTAL             #
#=========================================#

diff_case()
{
    MAX_SIZE=$1
    TEST_NAME=$2
    echo ""
    echo "================Bateria de testes de $TEST_NAME ========================="
    echo ""
    for ((  i = 1 ;  i <= $MAX_SIZE;  i++, TOTAL++  ))
    do      
      if [ $i -lt 10 ]
        then
            echo "=================>$TEST_NAME-0$i-$TOTAL-N-ok<========================"
            diff $EXP_DIR$TEST_NAME-0$i-$TOTAL-N-ok.out $OUT_DIR/$TEST_NAME-0$i-$TOTAL-N-ok-user.out 
        else  
            if [ $i -eq 18 -a $TEST_NAME = "B" ]
                then i=20
            fi
            echo "=================>$TEST_NAME-0$i-$TOTAL-N-ok<========================"
            diff $EXP_DIR$TEST_NAME-$i-$TOTAL-N-ok.out $OUT_DIR/$TEST_NAME-0$i-$TOTAL-N-ok-user.out 
      fi
    done
}    
#===============================================================================
#=========================================#
#            REMOVE NEW LINES             #
#=========================================#

remove_newlines()
{
    MAX_SIZE=$1
    TEST_NAME=$2
        echo "================Bateria de testes de $TEST_NAME ========================="
    for ((  i = 1 ;  i <= $MAX_SIZE;  i++, TOTAL++  ))
    do      
      if [ $i -lt 10 ]
        then
            tr -d '\n' < $OUT_DIR/$TEST_NAME-0$i-$TOTAL-N-ok-user.out > buffer.out
            cp buffer.out $OUT_DIR/$TEST_NAME-0$i-$TOTAL-N-ok-user.out
            tr -d '\n' < $EXP_DIR/$TEST_NAME-0$i-$TOTAL-N-ok.out > buffer.out
            cp buffer.out $EXP_DIR/$TEST_NAME-0$i-$TOTAL-N-ok.out            
            rm buffer.out
        else  
            if [ $i -eq 18 -a $TEST_NAME = "B" ]
                then i=20
            fi
            tr -d '\n' < $OUT_DIR/$TEST_NAME-0$i-$TOTAL-N-ok-user.out > buffer.out
            cp buffer.out $OUT_DIR/$TEST_NAME-0$i-$TOTAL-N-ok-user.out
            tr -d '\n' < $EXP_DIR/$TEST_NAME-$i-$TOTAL-N-ok.out > buffer.out
            cp buffer.out $EXP_DIR/$TEST_NAME-$i-$TOTAL-N-ok.out                        
            rm buffer.out
      fi
    done
}    

#===============================================================================
#=========================================#
#            DIFF'S                       #
#=========================================#
test_diff()
{
    echo " Removing new lines from all the files"
    echo ""
    rm_all
    TOTAL=1
    echo "done"
    echo ""
    echo "============ DIFF CASES ============"
    echo ""
    diff_case TESTS_A 'A'
    diff_case TESTS_B 'B'
    diff_case TESTS_C 'C'
    diff_case TESTS_D 'D'
    diff_case TESTS_E 'E'
    diff_case TESTS_F 'F'
    diff_case TESTS_J 'J'
    diff_case TESTS_K 'K'                            
    diff_case TESTS_L 'L'
    diff_case TESTS_M 'M'
    diff_case TESTS_O 'O'
    diff_case TESTS_P 'P'
    diff_case TESTS_Q 'Q'
    diff_case TESTS_U 'U'
    diff_case TESTS_W 'W'
    echo "DONE"
    echo ""
}
#===============================================================================
#=========================================#
#            GAMA TESTES TOTAL            #
#=========================================#
test_all()
{
    test_case TESTS_A 'A'
    test_case TESTS_B 'B'
    test_case TESTS_C 'C'
    test_case TESTS_D 'D'
    test_case TESTS_E 'E'
    test_case TESTS_F 'F'
    test_case TESTS_J 'J'
    test_case TESTS_K 'K'                            
    test_case TESTS_L 'L'
    test_case TESTS_M 'M'
    test_case TESTS_O 'O'
    test_case TESTS_P 'P'
    test_case TESTS_Q 'Q'
    test_case TESTS_U 'U'
    test_case TESTS_W 'W'        
}
#===============================================================================
#=========================================#
#            REMOVE NEWLINES TOTAL        #
#=========================================#
rm_all()
{
    remove_newlines TESTS_A 'A'
    remove_newlines TESTS_B 'B'
    remove_newlines TESTS_C 'C'
    remove_newlines TESTS_D 'D'
    remove_newlines TESTS_E 'E'
    remove_newlines TESTS_F 'F'
    remove_newlines TESTS_J 'J'
    remove_newlines TESTS_K 'K'                            
    remove_newlines TESTS_L 'L'
    remove_newlines TESTS_M 'M'
    remove_newlines TESTS_O 'O'
    remove_newlines TESTS_P 'P'
    remove_newlines TESTS_Q 'Q'
    remove_newlines TESTS_U 'U'
    remove_newlines TESTS_W 'W'        
}
#===============================================================================
#=========================================#
#                   USAGE                 #
#=========================================#
print_help()
{
	   echo "Usage: $0 -c -m"; 
	   echo "Where -c test all bateries";
	   echo "Where -m diff all bateries";	   
	   return
}
#===============================================================================
#=========================================#
#       Main procedure start here         #
#=========================================#

if [ $# -eq 0 ] ; then
    print_help
    exit 1
fi    

while getopts cdme: opt
do
    case "$opt" in
	c) test_all;;
	m) test_diff;;
	esac
done        	   
	   

