// cat > main.cpp (add includes and copy paste from subject)
// cat > output.txt (copy paste from subject)
// clang++ -fsanitize=address -Wall -Wextra -Werror -std=c++98 *.cpp -o start
// ./start | cat -e > myoutput.txt
// diff output.txt myoutput.txt

# pragma once

# include <iostream>
# include <map>
# include "ASpell.hpp"
# include "ATarget.hpp"

using namespace std;

typedef string str;

class Warlock {
    private:
        str _name;
        str _title;

        map<str, ASpell *> _spellBook;
        
        Warlock();
        Warlock(const Warlock &);
        Warlock & operator=(const Warlock &);

        
    public:
        Warlock(const str & n, const str & t): _name(n), _title(t), _spellBook() {cout<<getName()<<": This looks like another boring day."<<endl;}
        ~Warlock()                                  {for (map<str, ASpell *>::iterator it = this->_spellBook.begin();it != _spellBook.end(); it++) delete (it->second); cout<<getName()<<": My job here is done!"<<endl;}
        
        const str & getName() const                 {return this->_name;}
        const str & getTitle() const                {return this->_title;}
        void        setTitle(const str & t)         {this->_title = t;}

        void        introduce() const               {cout<<getName()<<": I am "<<getName()<<", "<<getTitle()<<"!"<<endl;}

        void        learnSpell(ASpell *s)           {if (s) this->_spellBook.insert(make_pair(s->getName(), s->clone()));}
        void        forgetSpell(str s)              {ASpell * spell = this->_spellBook[s]; if (spell) {delete spell; this->_spellBook.erase(s);}}
        void        launchSpell(str s, ATarget & t) {ASpell * spell = this->_spellBook[s]; if (spell) spell->launch(t);}

};
