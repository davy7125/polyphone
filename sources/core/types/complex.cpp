#include "complex.h"

Complex::Complex()
{

}

Complex Complex::operator *= (const double factor)
{
    _real *= factor;
    _imag *= factor;
    return *this;
}
