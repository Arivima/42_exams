# pragma once

# include <iostream>
# include "ATarget.hpp"

using namespace std;
typedef string str;

class ATarget;

class ASpell {
    protected:
        str _name;
        str _effects;

    public:
        
        ASpell(){}
        ASpell(const ASpell &cpy){*this = cpy;}
        ASpell & operator=(const ASpell &cpy){this->_name = cpy.getName(); this->_effects = cpy.getEffects(); return *this;}

        
        ASpell(const str & n, const str & e): _name(n), _effects(e){}
        virtual ~ASpell(){}
        
        const str &  getName() const{return this->_name;}
        const str &  getEffects() const{return this->_effects;}

        virtual ASpell *    clone() const =0;

        void launch(const ATarget &) const;

};

