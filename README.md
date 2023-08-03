# 42_exams
## Contents of this github
| exam   | subject | rendu | tester | list of references |  
| - | :-: | :-: | :-: | :-: |  
| exam02 | :green_circle: | :green_circle: |  | :green_circle: |
| exam03 | :green_circle: | :green_circle: |  | :green_circle: |
| exam04 | :green_circle: | :green_circle: |  | :green_circle: |
| exam05 | :green_circle: | :green_circle: |  | :green_circle: |
| exam06 | :green_circle: | :green_circle: | :green_circle: | :green_circle: |

## Tips for approaching exams
1. read subject and write your own code in exam conditions, from scratch, without studying other students'code.
2. debug and make it work -> if you need to look at someone else code / if there are still "minor" mistakes (that you consider stupid/minor but cannot correct without external help), there are still things you need to learn/understand.
3. read peer code to find ways to make your code more efficient for exam conditions (shorter, more logical etc.). This is also a good moment to learn different styles of code.
## Tips to facilitate writing exams
Vim documentation : https://vimdoc.sourceforge.net/  
On term : CTRL + SHIFT + C / V to copy paste on terminal

### 1. Vim configuration
    cat > ~/.myvimrc
    set nu          // add numerotation of lines
    set mouse=a     // add mouse control and selection
    syntax on       // add colors

Other useful configuration options : https://dougblack.io/words/a-good-vimrc.html

### 2. Vim shortcuts essentials
####    Very basics 
    :i      // insertion mode
    :w      // save 
    :q      // quit
    :!      // force
    :[cmd]  // apply commands to single files (example :set nu)
    dd      // delete a line
    yy      // copy line
    y       // copy word
    p       // paste
    u       // undo
    U       // restore (undo) last changed line
    Ctrl + r// redo
    .       // repeat last command
####    Word substitution:
    :%s/<old>/<new>	    // one at a time
    :%s/<old>/<new>/g   // all at same time
    :%s/<old>/<new>/gc      // replace all old with new throughout file with confirmations

###     Cheatsheet
https://devhints.io/vim  
https://vim.rtorr.com/

### 3. compilation, leaks and testing

Checking leaks with Address Sanitizer  
https://github.com/google/sanitizers/wiki/AddressSanitizer

    gcc -fsanitize=address -Wall -Werror -Wextra *.c -o myexecutable                    // C compilation
    clang++ -fsanitize=address -Wall -Wextra -Werror -std=c++98 *.cpp -o myexecutable   // C++ compilation
Checking leaks with Valgrind

    clear && gcc -g -Wall -Wextra -Werror mini_serv.c -o mini_serv && valgrind ./mini_serv 8080
    clear && gcc -g -Wall -Wextra -Werror mini_serv.c -o mini_serv && valgrind --show-leak-kinds=all --leak-check=full ./mini_serv 8080

Testing your program when provided with main and/or output examples

    cat > main.c                           // add includes, then copy paste main code from subject, CTRL+C to stop cat
    cat > output.txt                       // copy paste expected output from subject, CTRL+C to stop cat

    ./myexecutable | cat -e > myoutput.txt // put your program output to a specific file
    
    diff output.txt myoutput.txt           // test your output against the expected output
    

## References
All updated subjects here -> [JCluzet](https://github.com/JCluzet/42_EXAM.git)  