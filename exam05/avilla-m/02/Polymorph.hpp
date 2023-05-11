# pragma once

# include "ASpell.hpp"

class Polymorph : public ASpell {
    public:
        Polymorph() : ASpell("Polymorph", "turned into a critter") {}
        ~Polymorph(){}
        ASpell *    clone() const {return new Polymorph();}
};
