#ifndef COMPLEX_H
#define COMPLEX_H


class Complex
{
public:
    /// Constructor
    Complex();

    /// Getters / Setters for real and imag parts of the complex number
    void imag(double value) { _imag = value; }
    void real(double value) { _real = value; }
    double imag() { return _imag; }
    double real() { return _real; }

    /// Multiplication
    Complex operator *= (const double factor);

private:
    double _real, _imag;
};

#endif // COMPLEX_H
