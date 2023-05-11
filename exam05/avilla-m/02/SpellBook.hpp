# pragma once

# include <iostream>
# include <map>
# include "ASpell.hpp"

using namespace std;
typedef string str;

class SpellBook {
    private:
        map<str, ASpell *> _sB;
        
        SpellBook(const SpellBook &);
        SpellBook & operator=(const SpellBook &);

    public:
        SpellBook(): _sB(){}
        ~SpellBook() {
                for (map<str, ASpell *>::iterator it = this->_sB.begin(); it != _sB.end(); it++) 
                    delete (it->second);
                this->_sB.clear(); // not necessary but cleaner
            }
        
        void    learnSpell(ASpell *s) {
                if (s)
                    this->_sB.insert(make_pair(s->getName(), s->clone()));
            }
        void    forgetSpell(const str & s) {
                if (this->_sB.find(s) != this->_sB.end()){
                    delete this->_sB[s];
                    this->_sB.erase(s);
                }
            }
        ASpell* createSpell(const str & s) {
                if (this->_sB.find(s) != this->_sB.end())
                    return this->_sB[s];
                else
                    return nullptr;
            }
};
