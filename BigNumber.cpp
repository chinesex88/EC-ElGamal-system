#include "BigNumber.h"
/*#include <iostream>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <algorithm>*/
#include <bits/stdc++.h>
using namespace std;

void VectorToString(vector<long long int> &vec,
                    string &signed_string,
                    int realstart,
                    string append_zero,
                    int way);

inline bool XOR(bool a, bool b) // sign XOR
{
    return (a || b) && !(a && b);
}
//處理 dec 與 hex  由於vector預設儲存的是decimal int
//要將string 的hexdecimal轉成dec再輸入 相反同理
int hextodec(string a)
{
    int decimal;
    stringstream stream;
    stream << a;
    stream >> hex >> decimal;
    return decimal;
}
char dectohex(int a)
{
    char hex;
    if (a > 9)
        hex = 97 + a - 10;
    else
        hex = '0' + a;
    return hex;
}
//一樣是處理hex dec轉換 只是回傳值為string
string numToString(int v)
{
    char tmp[14];
    sprintf(tmp, "%x", v);
    return string(tmp);
}
//operator
// + - * 均為模擬直式運算 並先運算後處理進位問題
void BigNumber::negative()
{
    this->neg = !this->neg;
}
BigNumber BigNumber::operator+(BigNumber addend) //add negative num use minus
{
    if (this->getString() == "NaN")
        return BigNumber("NaN");
    else
    {
        if (this->neg && !addend.neg) //negative(this) + postive(addend) => positive(addend) - positive(this)
        {
            return addend.minus(*this);
        }
        else if (!this->neg && addend.neg) //positive(this) + negative(addend) => positive(this) - positive(addend)
        {
            return this->minus(addend);
        }
        else // same sign addition (+) + (+), (-) + (-) = -((+) + (+))
        {
            return this->add(addend);
        }
    }
}
BigNumber BigNumber::operator-(BigNumber minuend)
{
    if (this->getString() == "NaN")
        return BigNumber("NaN");
    else                          // minus is based on addition, just reverse sign
    {                             //based on addition we don't have to deal with case in subtraction
        minuend.negative();       //multi -1
        return (*this) + minuend; //based on "+"
    }
}
BigNumber BigNumber::operator-()
{
    if (this->getString() == "NaN")
        return BigNumber("NaN");
    else
    {
        BigNumber tmp = BigNumber(*this);
        tmp.negative();
        return tmp;
    }
}
BigNumber BigNumber::operator*(const BigNumber &multiplier)
{
    //special case
    if (this->getString() == "NaN")
        return BigNumber("NaN");
    else
    {
        if (this->unsigned_string == "0" || multiplier.unsigned_string == "0")
            return BigNumber("0");
        int deg_1 = this->vec2.size();
        int deg_2 = multiplier.vec2.size();
        bool isneg = XOR(this->neg, multiplier.neg);

        vector<long long int> out;
        out.assign(deg_1 + deg_2, 0);   // inital output to 0
        for (int i = 0; i < deg_1; i++) //sim 直式乘法
            for (int j = 0; j < deg_2; j++)
                out[i + j] += (this->vec2[i] * multiplier.vec2[j]);
        //carry out
        int Deg = out.size();
        for (int i = 0; i < Deg; i++)
        {
            if (out[i] >= MAX_VEC2)
            {
                out[i + 1] += out[i] / MAX_VEC2;
                out[i] %= MAX_VEC2;
            }
        }
        return BigNumber(out, 2, isneg);
    }
}

BigNumber BigNumber::operator/(BigNumber &divisor)
{
    //special case
    if (divisor.unsigned_string == "0" || this->getString() == "NaN")
    {
        return BigNumber("NaN");
    }
    else if (divisor.isAbsGreater(*this) || this->unsigned_string == "0")
    {
        return BigNumber("0");
    }
    else if (*this == divisor)
    {
        return BigNumber("1");
    }
    //normal case
    else
    {
        string out;
        bool isneg = XOR(this->neg, divisor.neg);
        if (isneg)
            out.insert(0, "-");
        divide(this->unsigned_string, divisor.unsigned_string, out);
        /*BigNumber re_val = BigNumber(out);
        BigNumber one = "1";
        if (isneg)
        {
            re_val = re_val - one;
        }*/
        return out;
    }
}
BigNumber BigNumber::operator%(BigNumber &divisor)
{
    //special case
    if (divisor.unsigned_string == "0" || this->getString() == "NaN")
        return BigNumber("NaN");
    else
    {
        BigNumber out;
        BigNumber tmp = *this / divisor;
        out = *this - tmp * divisor;
        if (out.neg != divisor.neg && out.getString() != "0")
        {
            out = out + divisor;
        }
        return out;
    }
}
int BigNumber::estimate(string temp, vector<string> &test)
{
    for (int i = 15; i >= 1; i--)
    {
        if (isStringEqGreater(temp, test[i]))
        {
            return i;
        }
    }
    return -1;
}
void BigNumber::divide(string L, string R, string &out)
{
    vector<string> test(16);
    BigNumber R_big = BigNumber(R);
    //試商 測試每位的除法哪個結果符合
    //透過estimate function 測試哪個值為商
    for (int i = 1; i < 16; i++)
        test[i] = (BigNumber(dectohex(i)) * R_big).getString();
    int deg_1 = L.size();
    int deg_2 = R.size();
    int MAX_deg = deg_1 - deg_2 + 1;
    //example :L = 1234 R = 123
    string temp;
    BigNumber tmp_1, tmp_2, tmp_3;
    for (int i = 0; i < MAX_deg; i++)
    {
        if (L.size() < deg_2) //被除數會越除越小
            break;
        if (i == 0)
            temp.assign(L, 0, deg_2); //temp = 123
        else
            temp.push_back(L[temp.size()]); //each iteration add next digit of L
        if (isStringEqGreater(temp, R))
        {
            int quo = estimate(temp, test);
            out.push_back(dectohex(quo));
            //模擬直式除法，扣掉除數*商
            tmp_1.init(temp);
            tmp_2.init(test[quo]);
            L.erase(0, temp.size());
            tmp_3 = tmp_1 - tmp_2;
            L.insert(0, tmp_3.getString());
            temp = tmp_3.getString();
        }
        else
            out.push_back('0');
    }
    int out_size = out.size();
    for (int i = 0; i < MAX_deg - out_size; i++)
        out.push_back('0');
    while (out[0] == '0')
        out.erase(0, 1);
}
BigNumber BigNumber::add(BigNumber &addend)
{
    vector<long long int> vecL = this->vec1;  //left
    vector<long long int> vecR = addend.vec1; // right  // "Left" + "Right"
    if (vecL.size() < vecR.size())
        swap(vecL, vecR);
    int deg_1 = vecL.size();
    int deg_2 = vecR.size();

    vector<long long int> out;
    int carry = 0;
    for (int i = 0; i < deg_1; i++)
    {
        if (i < deg_2)
            out.push_back(vecL[i] + vecR[i] + carry);
        else
            out.push_back(vecL[i] + carry);
        if (out[i] >= MAX_VEC1)
        {
            out[i] -= MAX_VEC1;
            carry = 1;
        }
        else
            carry = 0;
    }
    if (carry)
        out.push_back(1);
    return BigNumber(out, 1, this->neg);
}
BigNumber BigNumber::minus(BigNumber &minuend)
{ //會進來這個funciton 一定是 postive - positive
    vector<long long int> vecL = this->vec1;
    vector<long long int> vecR = minuend.vec1;
    vector<long long int> out;
    bool isneg = false;
    if (minuend.isAbsGreater(*this))
    { //減數 > 被減數 => 值 < 0
        swap(vecL, vecR);
        isneg = true;
    }
    int deg_1 = vecL.size();
    int deg_2 = vecR.size();
    int carry = 0; //carry 為借位
    for (int i = 0; i < deg_1; i++)
    {
        if (i < deg_2)
            out.push_back(vecL[i] - vecR[i] - carry);
        else
            out.push_back(vecL[i] - carry);
        if (out[i] < 0)
        {
            out[i] += MAX_VEC1;
            carry = 1;
        }
        else
            carry = 0;
    }
    return BigNumber(out, 1, isneg);
}

//compare function
int BigNumber::strcmp_val(string a, string b)
{                                      //compare two string "hex" value 而不是字典序，並且不考慮正負
    while (a[0] == '0' || a[0] == '-') //移除數字前的符號與0
        a.erase(0, 1);
    while (b[0] == '0' || b[0] == '0')
        b.erase(0, 1);
    if (a.size() != b.size()) //a較長代表 a的值 大於 b的值
        return a.size() - b.size();
    else
        for (int i = 0; i < a.size(); i++) //若兩長度相同，從高位開始比較
            if (a[i] != b[i])
                return a[i] - b[i];
    return 0;
}
//based on strcmp_val 寫幾個常用的比較function 精簡主要的code以及提高程式可讀性
bool BigNumber::isStringEqGreater(string a, string b)
{
    int cmp = strcmp_val(a, b);
    return (cmp >= 0);
}
bool BigNumber ::isStringGreater(string a, string b)
{
    int cmp = strcmp_val(a, b);
    return (cmp > 0);
}
bool BigNumber::isAbsEqGreater(BigNumber &tmp)
{
    return isStringEqGreater(this->getString(), tmp.getString());
}
bool BigNumber::isAbsGreater(BigNumber &tmp)
{
    return isStringGreater(this->getString(), tmp.getString());
}
//constructor
BigNumber::BigNumber(vector<long long int> &vec, int type, bool neg)
{
    string signed_string;
    if (neg)
        signed_string.insert(0, "-");
    int Deg = vec.size();
    int start = Deg - 1;
    while (vec[start] == 0 && start > 0)
        start--;
    signed_string.append(numToString(vec[start--]));
    string append_zero;
    int way;
    if (type == 1)
    {
        append_zero = "0000000";
        way = 7;
    }
    else
    {
        append_zero = "0000";
        way = 4;
    }
    VectorToString(vec, signed_string, start, append_zero, way);
    this->init(signed_string);
}
//將運算時計算出的vector轉換成string, 並以string 初始化所求的Bignumber(呼叫init)
void VectorToString(vector<long long int> &vec, string &signed_string, int start, string append_zero, int way)
{
    for (int i = start; i >= 0; i--)
    {
        int tmp = vec[i];
        if (tmp == 0)
            signed_string.append(append_zero);
        else
        {
            char temp[15];
            sprintf(temp, "%0*llx", way, vec[i]);
            signed_string.append(string(temp));
        }
    }
}
void BigNumber::init(string s)
{
    if (s[0] == '-')
    {
        neg = true;
        s.erase(0, 1);
    }
    else
        neg = false;
    while (s[0] == '0') // 移除輸入多餘的0
    {
        s.erase(0, 1);
    }
    if (s == "") //移除光 補0
        s = string("0");
    this->unsigned_string = s;
    this->deg = s.size();
    this->init_vector(this->vec1, 7);
    this->init_vector(this->vec2, 4);
}
//初始化class中的vector 將input string 轉換成vector
void BigNumber::init_vector(vector<long long int> &vec, int Deg)
{
    vec.clear();
    string temp, unsigned_string = this->unsigned_string;
    int counter = this->deg;
    //convert input string to data vector
    //for example : input 123456789, init_vector(,4) => vec[0] = 6789, vec[1] = 2345, ec[2] = 1
    while (counter > 0)
    {
        counter -= Deg;
        if (counter > 0)
            temp.assign(unsigned_string, counter, Deg);
        else
            temp.assign(unsigned_string, 0, counter + Deg);
        vec.push_back(hextodec(temp));
    }
}
//IO
string BigNumber::getString()
{
    string signed_string;
    if (neg)
        signed_string.insert(0, "-");
    signed_string.append(unsigned_string);
    return signed_string;
}

//compare
bool BigNumber::operator==(BigNumber &comp)
{
    if (this->neg != comp.neg || this->deg != comp.deg)
    {
        return false;
    }
    else
    {
        return (this->unsigned_string == comp.unsigned_string);
    }
}
bool BigNumber::operator>(BigNumber &comp)
{
    if (!this->neg && comp.neg)
    { //+  > -
        return true;
    }
    else if (this->neg && !comp.neg)
    { // - < +
        return false;
    }
    else
    {
        if (this->neg)
        { //- -
            if (this->deg != comp.deg)
                return this->deg < comp.deg;
            else
                return !this->isAbsEqGreater(comp);
        }
        else
        { //+ +
            if (this->deg != comp.deg)
                return this->deg > comp.deg;
            else
                return this->isAbsGreater(comp);
        }
    }
}
