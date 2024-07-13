#Make shell
make

#Test empty line no crash
echo Test empty line no crash
./shell56 << EOF

EOF
status=$?
if [ $status == 0 ] ; then
	echo SUCCESS
else
	echo FAILED
fi

echo

echo Test control+D character exit
#Test control+D character exit
./shell56 << EOF
EOF
status=$?
if [ $status == 0 ] ; then
	echo SUCCESS
else
	echo FAILED
fi

echo

echo Test pwd
#Test pwd
./shell56 > out.txt << EOF
pwd
EOF
status=$?
if [ $status == 0 -a "$(cat out.txt)" == "/home/cs5600/Desktop/team-23-lab1" ] ; then
	echo SUCCESS
else
	echo FAILED 
fi

echo

echo Test pwd wrong number of arguments
#Test pwd
./shell56 >& out.txt << "EOF"
pwd 3
echo $? > status.txt
EOF
if [ "$(cat status.txt)" == 1 -a "$(cat out.txt)" == "pwd: wrong number of arguments" ] ; then
	echo SUCCESS
else
	echo FAILED 
fi

echo

echo Test cd with no argument
#Test cd with no argument
./shell56 > out.txt << "EOF"
cd
echo $? > status.txt
pwd
EOF
if [ "$(cat $HOME/status.txt)" == 0 -a "$(cat out.txt)" == $HOME ] ; then
	echo SUCCESS
else
	echo FAILED 
fi

echo

echo Test cd with 1 argument
#Test cd with 1 argument
./shell56 > out.txt << "EOF"
cd /tmp
echo $? > status.txt
pwd
EOF

if [ "$(cat /tmp/status.txt)" == 0 -a "$(cat out.txt)" == "/tmp" ] ; then
	echo SUCCESS
else
	echo FAILED 
fi

echo

echo Test cd with invalid argument
#Test cd with invalid argument
./shell56 >& out.txt << "EOF"
cd /fake-directory
echo $? > status.txt
EOF
if [ "$(cat status.txt)" == 1 -a "$(cat out.txt)" == "cd: No such file or directory" ] ; then
	echo SUCCESS
else
	echo FAILED 
fi

echo

echo Test cd with more than 1 argument
#Test cd with more than 1 argument
./shell56 >& out.txt << "EOF"
cd /tmp /home
echo $? > status.txt
EOF
if [ "$(cat status.txt)" == 1 -a "$(cat out.txt)" == "cd: wrong number of arguments" ] ; then
	echo SUCCESS
else
	echo FAILED 
fi

echo

echo Test exit with 0
#Test exit with 0
./shell56 << EOF
exit
EOF
status=$?
if [ $status == 0 ] ; then
	echo SUCCESS
else
	echo FAILED 
fi

echo

echo Test exit with other status code
#Test exit with other status code
./shell56 << EOF
exit 7
EOF
status=$?
if [ $status == 7 ] ; then
	echo SUCCESS
else
	echo FAILED 
fi

echo

echo Test exit failure too many arguments
#Test exit failure too many arguments
./shell56 >& out.txt << "EOF"
exit 5 6
echo $? > status.txt
EOF
if [ "$(cat status.txt)" == 1 -a "$(cat out.txt)" == "exit: too many arguments" ] ; then
	echo SUCCESS
else
	echo FAILED 
fi

echo

echo Test ls external command
#Test ls external command
./shell56 > out.txt << EOF
ls
EOF
status=$?
if [ $status == 0 -a "$(ls)" == "$(cat out.txt)" ] ; then
	echo SUCCESS
else
	echo FAILED 
fi

echo

echo Test ls invalid argument
#Test ls invalid argument
./shell56 >& out.txt << "EOF"
ls 1
echo $? > status.txt
EOF
if [ "$(cat status.txt)" == 2 -a "$(cat out.txt)" == "ls: cannot access '1': No such file or directory" ] ; then
	echo SUCCESS
else
	echo FAILED 
fi

echo

echo Test echo command
#Test echo command
./shell56 > out.txt << EOF
echo a b c
EOF
status=$?
if [ $status == 0 -a "$(cat out.txt)" == "a b c" ] ; then
	echo SUCCESS
else
	echo FAILED 
fi

echo

echo Test invalid command
#Test invalid command
./shell56 >& out.txt << "EOF"
not-a-command
echo $? > status.txt
EOF
if [ "$(cat status.txt)" == 1 -a "$(cat out.txt)" == "not-a-command: No such file or directory" ] ; then
	echo SUCCESS
else
	echo FAILED 
fi

echo

echo Test single pipe
#Test single pipe
./shell56 > out.txt <<EOF
echo foo | cat
EOF
if [ $? == 0 -a "$(cat out.txt)" == foo ] ; then
  	echo SUCCESS
else
 	echo FAILED
fi

echo

echo Test multiple pipe
#Test multiple pipe
./shell56 > out.txt <<EOF
echo foo | cat | cat
EOF
if [ $? == 0 -a "$(cat out.txt)" == foo ] ; then
	echo SUCCESS
else
	echo FAILED
fi

echo

echo Test bad pipes
#Test bad pipes
./shell56 > out.txt <<"EOF"
echo foo | | cat
| | cat
| |
|||
cat |||
echo $? > status.txt
EOF
if [ "$(cat status.txt)" == 0 -a "$(cat out.txt)" == "" ] ; then
	echo SUCCESS
else
	echo FAILED
fi

echo

echo Test output redirection for single command
#Test I/O redirection for single command
./shell56 <<"EOF"
echo foo > out.txt
echo $? > status.txt
EOF
if [ "$(cat status.txt)" == 0 -a "$(cat out.txt)" == foo ] ; then
	echo SUCCESS
else
	echo FAILED
fi

echo

echo Test input redirection for single command
#Test I/O redirection for single command
./shell56 > output.txt <<"EOF"
cat < out.txt
echo $? > status.txt
EOF
if [ "$(cat status.txt)" == 0 -a "$(cat output.txt)" == foo ] ; then
	echo SUCCESS
else
	echo FAILED
fi

echo

echo Test input and output redirection for single command
#Test I/O redirection for single command
./shell56 <<"EOF"
cat < out.txt > output.txt
echo $? > status.txt
EOF
if [ "$(cat status.txt)" == 0 -a "$(cat output.txt)" == foo ] ; then
	echo SUCCESS
else
	echo FAILED
fi

echo

echo Test input redirection at start with pipe
#Test I/O redirection
./shell56 > output.txt <<"EOF"
ls > out.txt
cat < out.txt | cat
echo $? > status.txt
EOF
if [ "$(cat status.txt)" == 0 -a "$(cat output.txt)" == "$(ls)" ] ; then
	echo SUCCESS
else
	echo FAILED
fi

echo

echo Test output redirection at end with pipe
#Test I/O redirection
./shell56 <<"EOF"
echo foo | cat > out.txt
echo $? > status.txt
EOF
if [ "$(cat status.txt)" == 0 -a "$(cat out.txt)" == foo ] ; then
	echo SUCCESS
else
	echo FAILED
fi

echo

echo Test invalid I/O redirection with no command
#Test I/O redirection invalidity
./shell56 <<"EOF"
 >>>>>
echo $? > status.txt
EOF
if [ "$(cat status.txt)" == 0 ] ; then
	echo SUCCESS
else
	echo FAILED
fi

echo

echo Test invalid I/O redirection with invalid command
#Test I/O redirection invalidity
./shell56 <<"EOF"
foo >>>>>
echo $? > status.txt
EOF
if [ "$(cat status.txt)" == 0 ] ; then
	echo SUCCESS
else
	echo FAILED
fi

echo

echo Test invalid I/O redirection and pipe combinations
#Test I/O redirection and pipe invalidity
./shell56 <<"EOF"
foo >>>>>
< > <<<<
>>>>> < < ls
foobar < | >>> |
echo $? > status.txt
EOF
if [ "$(cat status.txt)" == 0 ] ; then
	echo SUCCESS
else
	echo FAILED
fi

rm -rf output.txt
rm -rf status.txt
rm -rf out.txt
rm -rf /tmp/status.txt
rm -rf $HOME/status.txt
rm -rf shell56
