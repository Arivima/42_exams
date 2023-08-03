# EXAM 06  
## Advices :
1. Write your own code, make it work.  
=> anything that doesn't work means that you didn't understand something that you should understand
2. Study other students' code to learn their approach and optimize your own code.  
    => This is an exam, the code doesn't need to be bullet proof or best practice, it needs to work and be short so you are able to write and debug in 2 hours  
3. Write your own tester
4. Practice in exam conditions (vim + term, silence, max 2/3h) once before the exam
5. Use the main.c given in subject folder to be the base for your mini_serv.c

## References :
100 lines code, array of fds :  
https://github.com/luta-wolf/42-examrank.git => I used this one <i>(no err check)</i>     
https://github.com/pasqualerossi/42-School-Exam-Rank-06.git <i>(w/ error checking, but no fd_ISSET on write - which doesn't pass the exam, add it)</i>  

100 lines code, list of clients:  
https://github.com/markveligod/examrank-02-03-04-05-06.git <i>(better way of handling connections, but longer)</i> 

300 lines:  
https://github.com/Glagan/42-exam-rank-06.git

grademe tester  :  

https://github.com/JCluzet/42_EXAM.git <i>(didn't work at the time I trained but may be updated in the meantime)</i>  

## leaks check : 
basic :

    clear && gcc -g -Wall -Wextra -Werror mini_serv.c -o mini_serv && valgrind ./mini_serv 8080

more details :

    clear && gcc -g -Wall -Wextra -Werror mini_serv.c -o mini_serv && valgrind --show-leak-kinds=all --leak-check=full ./mini_serv 8080

## Testing your mini_serv
### Telnet
check your mini_serv with other 3 terminals :

    telnet 127.0.0.1 <port used in mini_serv>

### Tester --WIP
in "./tester"

    ./mini_cli [port]  

default ip : 127.0.0.1  
default port : 8080  
default max nb of clients : 1024

TODO :
- test inbox, currently only showling first message received
- update array by a list of clients
- check for all sys clals to be protected adequately
- maybe a way to improve inbox, currently only reading all clients socket when using this command

11. Use dynamic memory allocation for clients: OK
Instead of using a fixed-size array for clients, consider using dynamic memory allocation (e.g., malloc) to handle clients more efficiently. This allows for a more scalable solution.

