// cat > main.cpp (add includes and copy paste from subject)
// cat > output.txt (copy paste from subject)
// clang++ -fsanitize=address -Wall -Wextra -Werror -std=c++98 *.cpp -o start
// ./start | cat -e > myoutput.txt
// diff output.txt myoutput.txt

# pragma once

# include <iostream>

using namespace std;

typedef string str;

class Warlock {
    private:
        str _name;
        str _title;
        
        Warlock();
        Warlock(const Warlock &);
        Warlock & operator=(const Warlock &);

        
    public:
        Warlock(const str & n, const str & t): _name(n), _title(t) {cout<<getName()<<": This looks like another boring day."<<endl;}
        ~Warlock() {cout<<getName()<<": My job here is done!\n";}
        
        const str & getName() const{return this->_name;}
        const str & getTitle() const{return this->_title;};
        void        setTitle(const str & t){this->_title = t;}

        void        introduce() const {cout<<getName()<<": I am "<<getName()<<", "<<getTitle()<<"!"<<endl;}

};
