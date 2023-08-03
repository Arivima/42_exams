# pragma once

# include <iostream>
# include "ASpell.hpp"

using namespace std;
typedef string str;

class ASpell;

class ATarget {
    protected:
        str _type;

    public:
        ATarget();
        ATarget(const ATarget &cpy){*this = cpy;}
        ATarget & operator=(const ATarget &cpy){this->_type = cpy.getType(); return *this;}
        
        ATarget(const str & t): _type(t){}
        virtual ~ATarget(){}
        
        const str &  getType() const {return this->_type;}

        virtual ATarget *    clone() const =0;

        void getHitBySpell(const ASpell &) const;
};
