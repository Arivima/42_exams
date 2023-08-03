# 42_exams
## Contents of this github
| exam #  | exercise | subject | rendu | tester | tips | references |  
| :- | :- | :-: | :-: | :-: | :-: | :-: |   
| exam02 | printf | :green_circle: | :green_circle: | :green_circle: | - | - |
| exam02 | gnl | :green_circle: | :green_circle: | :green_circle: | - | - |
| exam02 | union | :green_circle: | :green_circle: | :green_circle: | - | - |
| exam02 | inter | :green_circle: | :green_circle: | :green_circle: | - | - |
| exam03 | micro_paint | :green_circle: | :green_circle: | - | - | :green_circle: |
| exam03 | mini_paint | :green_circle: | - | - |  - | :green_circle: |
| exam04 | microshell | :green_circle: | :green_circle: | - | -  | - |
| exam05 | Warlock.cpp | :green_circle: | :green_circle: | :green_circle: | :green_circle: | - |
| exam06 | mini_serv | :green_circle: | :green_circle: | :green_circle: | :green_circle: | :green_circle: |


## Tips for approaching exams
1. Write your own code from scratch, without studying other students'code.
2. Make it work  
    if you need to look at someone else code / if there are still "minor" mistakes <i>(that you consider stupid/minor but in all honnesty cannot correct without external help)</i> => there are still things you need to learn/understand.
3. Read peer code to (1) learn from them and (2) optimize your code for exam conditions (shorter, more efficient etc.).  
    => This is an exam, the code doesn't need to be bullet proof or best practice, it needs to work and be short so you are able to write and debug in 2 hours  
4. Write your own tester
5. Practice in exam conditions (vim + term, silence, max 2/3h) once before the exam  

## Tips to write exams more efficiently
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
    :%s/<old>/<new>/gc  // replace all old with new throughout file with confirmations

####     Vim cheatsheet : additional shortcuts
https://devhints.io/vim  
https://vim.rtorr.com/

### 3. Compilation, leaks and testing

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