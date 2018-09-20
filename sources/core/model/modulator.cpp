#include "modulator.h"

Modulator::Modulator(int id) :
    _index(id),
    _hidden(false)
{

}

void Modulator::setHidden(bool isHidden)
{
    _hidden = isHidden;
}
