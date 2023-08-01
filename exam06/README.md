# EXAM 06  

## References
array : 
https://github.com/luta-wolf/42-examrank.git (no err check)
https://github.com/pasqualerossi/42-School-Exam-Rank-06.git (w/ error checking, no isset on write)


list :
https://github.com/markveligod/examrank-02-03-04-05-06.git

300 lines: 
https://github.com/Glagan/42-exam-rank-06.git

grademe tester :
https://github.com/JCluzet/42_EXAM.git


use main.c given in subject
- copy main into mini_serv.c, substitute words
- add control arguments

vim
substitution
multiselect line
vim comment
move line

clear && g++ -Wall -Wextra -Werror mini_serv.c -o mini_serv && ./mini_serv 8080
clear && g++ -Wall -Wextra -Werror mini_cli.c -o mini_cli && ./mini_cli 8080

leaks check : 
clear && gcc -g -Wall -Wextra -Werror mini_serv.c -o mini_serv && valgrind ./mini_serv 8080
clear && gcc -g -Wall -Wextra -Werror mini_serv.c -o mini_serv && valgrind --show-leak-kinds=all --leak-check=full ./mini_serv 8080

telnet 127.0.0.1 8080
