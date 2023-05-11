# pragma once

# include <iostream>
# include <map>
# include "ATarget.hpp"

using namespace std;
typedef string str;

class TargetGenerator {
    private:
        map<str, ATarget *> _tG;
        
        TargetGenerator(const TargetGenerator &);
        TargetGenerator & operator=(const TargetGenerator &);

    public:
        TargetGenerator(): _tG(){}
        ~TargetGenerator() {
                for (map<str, ATarget *>::iterator it = this->_tG.begin(); it != _tG.end(); it++) 
                    delete (it->second);
                this->_tG.clear();
            }
        
        void    learnTargetType(ATarget *s) {
                if (s)
                    this->_tG.insert(make_pair(s->getType(), s->clone()));
            }
        void    forgetTargetType(str const & s) {
                if (this->_tG.find(s) != this->_tG.end()){
                    delete this->_tG[s];
                    this->_tG.erase(s);
                }
            }
        ATarget* createTarget(str const & s) {
                if (this->_tG.find(s) != this->_tG.end())
                    return this->_tG[s];
                else
                    return nullptr;
            }
};
