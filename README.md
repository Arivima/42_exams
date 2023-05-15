# 42_exams
## References
All updated subjects and testers here -> ([JCluzet](https://github.com/JCluzet/42_EXAM.git)) (+ example code and practice prog)
## Tips for approaching exams
1. read subject and write your own code in exam conditions, from scratch, without studying.
2. debug and make it work -> if you need to look at someone else code / if there are "stupid" mistakes, there are still things you need to learn/understand.
3. read peer code to find ways to make your code more efficient for exam conditions (shorter, more logical etc.). This is also a good moment to learn different styles of code.
## Tips to facilitate writing exams
### 1. Before working on vim
    cat > ~/.myvimrc
    set nu          // add numerotation of lines
    set mouse=a     // add mouse control and selection
    syntax on       // add colors
-> Check on the internet if you find other shortcuts helpful
### 2. vim essentials
####    Basics 
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
####    Word substitution:
    :%s/<old>/<new>	    // one at a time
    :%s/<old>/<new>/g   // all at same time
On term : CTRL + SHIFT + C / V to copy paste on terminal
### 3. compilation and testing
When provided with main and/or output examples

    cat > main.c                           // add includes, then copy paste main code from subject, CTRL+C to stop cat
    cat > output.txt                       // copy paste expected output from subject, CTRL+C to stop cat
    gcc -fsanitize=address -Wall -Werror -Wextra *.c -o myexecutable                    // C compilation
    clang++ -fsanitize=address -Wall -Wextra -Werror -std=c++98 *.cpp -o myexecutable   // C++ compilation
    ./myexecutable | cat -e > myoutput.txt // put your program output to a specific file
    diff output.txt myoutput.txt           // test your output against the expected output
    
Compile with -fsanitize=address to make sure there is no leaks

