#ifndef BIGNUM_H
#define BIGNUM_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdio>

using namespace std;

#define MAX_VEC1 0x10000000
#define MAX_VEC2 0x10000

class BigNumber
{
private:
    bool neg;
    string unsigned_string;
    int deg; //有幾位數
    //乘法分割成4位時 開int 依然有overflow問題 因此用long long
    //(ffff * ffff)_16 = 4294836225_10 即使這個大小做幾次的加法也遠小於long long int
    vector<long long int> vec1; //for + and -
    vector<long long int> vec2; //for multi
    void init_vector(vector<long long int> &, int);
    void init_vector1();
    void init_vector2();

    void init(string);                             // initalizeing the string
    BigNumber(vector<long long int> &, int, bool); //constructor

    BigNumber add(BigNumber &);   // function for add
    BigNumber minus(BigNumber &); //function for add negative num

    int estimate(string tempt, vector<string> &mult); //試商
    void divide(string ls, string rs, string &ans);   //ls 被除數  rs為除數
    void negative();                                  //乘以-1
    bool isAbsEqGreater(BigNumber &);
    bool isAbsGreater(BigNumber &);

    static int strcmp_val(string a, string b);         //compare two string "hex" value 而不是字典序，並且不考慮正負
    static bool isStringGreater(string a, string b);   // hex value a 是否 > b
    static bool isStringEqGreater(string a, string b); // hex value a 是否 >= b
public:
    BigNumber() { this->init("0"); }
    template <class T>
    BigNumber(T);
    template <class T>
    BigNumber operator=(T);

    BigNumber operator+(BigNumber);
    BigNumber operator-(BigNumber);
    BigNumber operator-(); //負號
    BigNumber operator*(const BigNumber &);
    BigNumber operator/(BigNumber &);
    BigNumber operator%(BigNumber &);
    bool operator>(BigNumber &);
    bool operator==(BigNumber &);
    bool operator>=(BigNumber &comp) { return *this > comp || *this == comp; }
    bool operator!=(BigNumber &comp) { return !(*this == comp); }
    bool operator<(BigNumber &comp) { return !(*this >= comp); }
    bool operator<=(BigNumber &comp) { return *this < comp || *this == comp; }
    //IO
    string getString();
    void print()
    {
        if (neg)
            putchar('-');
        cout << this->unsigned_string << endl;
        //cout<<this->unsigned
    }
};
template <class T>
BigNumber::BigNumber(T template_init)
{ //using template can construct Bignumber conviniently.
    // more over, can assign value by "=" like any other variable
    string s;
    stringstream ss;
    ss << std::hex << template_init;
    ss >> s;
    this->init(s);
}
//reference https://stackoverflow.com/questions/12802536/c-multiple-definitions-of-operator
//inline function can be definded more than onece.
template <class T>
inline BigNumber BigNumber::operator=(T t)
{
    (*this) = BigNumber(t);
    return (*this);
}
template <>
inline BigNumber BigNumber::operator=<BigNumber>(BigNumber tmp)
{
    this->init(tmp.getString());
    return (*this);
}
#endif
