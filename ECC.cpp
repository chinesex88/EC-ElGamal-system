#include "BigNumber.h"
#include <bits/stdc++.h>
typedef class BigNumber B_int;
//由於我的大數只能讀入小寫英文字幕，因此在IO上另外處理
int mlen = 20;      //ECC 160
B_int base = "100"; //pow(2,8) = 256 in dec is 0x100 in hex
B_int p = "ffffffffffffffffffffffffffffffff7fffffff";
B_int a = "ffffffffffffffffffffffffffffffff7ffffffc";
B_int b = "1c97befc54bd7a8b65acf89f81d4d4adc565fa45";
B_int Gx = "4a96b5688ef573284664698968c38bb913cbfc82";
B_int Gy = "23a628553168947d59dcc912042351377ac5fb32";
B_int n = "0100000000000000000001f4c8f927aed3ca752257";
B_int three = "3";
B_int two = "2";
B_int zero = "0";
B_int one = "1";
B_int ten = "1b";
B_int four = "4";
//f_p(sqrt(a^2-n))
struct fp2
{
    B_int x, y;
};
struct point
{
    B_int x;
    B_int y;
};
struct Ciphertext
{
    B_int Pk;
    B_int Pb;
};
point G;
void BezoutIdentity(B_int A, B_int B, B_int &rx, B_int &ry)
{
    B_int x = 0;
    B_int y = 1;
    B_int last_x = 1;
    B_int last_y = 0;
    B_int tmp1, tmp2, tmp3;
    while (B != zero)
    {
        B_int q = A / B;
        tmp1 = A % B;
        A = B;
        B = tmp1;
        tmp2 = x;
        x = last_x - q * x;
        last_x = tmp2;
        tmp3 = y;
        y = last_y - q * y;
        last_y = tmp3;
    }
    rx = last_x;
    ry = last_y;
}
//multi inverse in finite field
B_int multiinverse(B_int s)
{
    B_int x, y;
    //px+ay = 1
    //y is multi inverse of a mod p
    BezoutIdentity(p, s, x, y);
    y = y % p;
    return y;
}
//let (0,0) be infinity
point doubling(point P);
point inverse(point s)
{
    point return_val;
    return_val.x = s.x;
    return_val.y = -s.y + p;
    return return_val;
}
point pluspt(point s, point t)
{
    B_int x1 = s.x;
    B_int x2 = t.x;
    B_int y1 = s.y;
    B_int y2 = t.y;
    point res;
    //if s == t use doubling
    if (x1 == x2 && y1 == y2)
        return doubling(s);
    //if s or t == infinity return directly.
    if (x1 == zero && y1 == zero)
        return t;
    if (x2 == zero && y2 == zero)
        return s;
    B_int tmp1, tmp2;
    tmp1 = -y2 % p;
    //if s = -t
    if (y1 == tmp1)
    {
        res.x = "0";
        res.y = "0";
        return res;
    }
    tmp1 = (y2 - y1) % p;
    tmp2 = (x2 - x1) % p;
    if (tmp2 == zero)
    {
        res.x = "0";
        res.y = "0";
        return res;
    }
    tmp2 = multiinverse(tmp2);
    B_int lambda = (tmp1 * tmp2) % p;
    B_int x3 = (lambda * lambda - x1 - x2) % p;
    B_int y3 = (-y1 + lambda * (x1 - x3)) % p;
    res.x = x3;
    res.y = y3;
    return res;
}
point doubling(point P)
{
    point return_val;
    B_int x1, y1;
    x1 = P.x;
    y1 = P.y;
    if (P.x == zero && P.y == zero)
        return P;
    B_int tmp1 = (three * x1 * x1 + a);
    tmp1 = tmp1 % p;
    B_int tmp2 = (two * y1) % p;
    tmp2 = multiinverse(tmp2);
    B_int lambda = (tmp1 * tmp2) % p;
    B_int x3 = (lambda * lambda - two * x1) % p;
    B_int y3 = (-y1 + lambda * (x1 - x3));
    y3 = y3 % p;
    return_val.x = x3;
    return_val.y = y3;
    return return_val;
}
point scalarmulti(B_int r, point P)
{
    point res;
    res.x = "0";
    res.y = "0";
    while (r > zero)
    {
        if (r % two == one)
        {
            res = pluspt(res, P);
        }
        P = doubling(P);
        r = r / two;
    }
    return res;
}
B_int power(B_int in, B_int y, B_int mod)
{
    string temp;
    B_int res = one;
    int len;
    while (y > zero)
    {
        if (y % two == one)
            res = res * in;
        res = res % mod;
        y = y / two;
        in = in * in;
        in = in % mod;
    }
    return res;
}
fp2 fp2multi(fp2 in1, fp2 in2, B_int p, B_int w2)
{
    fp2 res;
    B_int tmp1, tmp2;
    tmp1 = in1.x * in2.x % p;
    tmp2 = in1.y * in2.y % p;
    tmp2 = tmp2 * w2 % p;
    res.x = (tmp1 + tmp2) % p;
    tmp1 = in1.x * in2.y % p;
    tmp2 = in1.y * in2.x % p;
    res.y = (tmp1 + tmp2) % p;
    return res;
}
//return in^y under f_p(w2)
fp2 powerfp2(fp2 in, B_int y, B_int p, B_int w2)
{
    fp2 res;
    res.x = one;
    res.y = zero;
    while (y > zero)
    {
        if (y % two == one)
            res = fp2multi(res, in, p, w2);
        y = y / two;
        in = fp2multi(in, in, p, w2);
    }
    return res;
}
int LegendreSymbol(B_int in)
{
    int res;
    B_int tmp = power(in, (p - one) / two, p);
    if ((p - one) == tmp)
        return -1;
    string s = tmp.getString();
    stringstream ss;
    ss << s;
    ss >> std::hex >> res;
    ss.str("");
    ss.clear();
    return res;
}
bool checkoncurve(B_int in)
{
    B_int tmp = in * in * in + a * in + b;
    tmp = tmp % p;
    //now check tmp is quadratic residue mod p by euler's criterion
    if (LegendreSymbol(tmp) == 1)
        return true;
    return false;
}
B_int big_num_generator(int bit)
{
    srand(time(0));
    stringstream ss;
    string s, tmp;
    unsigned out;
    int k = bit / 32;
    for (int i = 0; i < k; i++)
    {
        out = (rand() % (4294967295 - 2147483649)) + 2147483649;
        ss << std::hex << out;
        ss >> tmp;
        s = s + tmp;
        // cout << "out " << std::hex << out << " s " << s << endl;
        tmp.erase();
        ss.str("");
        ss.clear();
    }
    return BigNumber(s);
}
//to find y=x^2
B_int cipolla(B_int in, B_int yp)
{
    B_int random = big_num_generator(128);
    B_int w2 = (random * random - in) % p;
    fp2 answer;
    //step1 pick an elemnt in prime field s.t. a^2-in is not square
    while (LegendreSymbol(w2) == 1)
    {
        random = big_num_generator(128);
        w2 = (random * random - in) % p;
    }
    w2 = random * random - in; //don't mod p now
    answer.x = random;
    answer.y = one;
    answer = powerfp2(answer, (p + one) / two, p, w2);
    answer.x = answer.x % p;
    //now answer.x is desire square root of in we want odd one
    if (answer.x % two == yp)
        return answer.x;
    else
    {
        return p - answer.x;
    }
}
point integertopoint(B_int in)
{
    point res;
    string tmp = in.getString();
    if (tmp.length() <= 40)
        tmp = tmp.append(2, '0');
    in = tmp;
    while (!checkoncurve(in))
        in = in + one;
    //now in is our x coor-dinate
    res.x = in;
    B_int temp = in * in * in + a * in + b;
    temp = temp % p;
    res.y = cipolla(temp, one);
    return res;
}
B_int pointtoineger(point in)
{
    return in.x;
}
string Capsconvert(string s)
{
    for (int i = 0; i < s.length(); i++)
    {
        //大寫
        if (s[i] <= 90 && s[i] >= 65)
            s[i] = s[i] - 65 + 97; //轉小寫
        else if (s[i] >= 97 && s[i] <= 122)
            s[i] = s[i] - 97 + 65; //轉大寫
    }
    return s;
}
void printpoint(point in)
{
    string Mx, My;
    printf("Mx = ");
    Mx = Capsconvert(in.x.getString());
    cout << Mx << endl;
    printf("My = ");
    My = Capsconvert(in.y.getString());
    cout << My << endl;
}
Ciphertext encryption(point Pm, point Pa, B_int nk)
{
    Ciphertext res;
    res.Pk = pointtoineger(scalarmulti(nk, G));
    res.Pb = pointtoineger(pluspt(Pm, scalarmulti(nk, Pa)));
    return res;
}
void printcipher(Ciphertext in)
{
    string tmp;
    string appendzero = "";
    int len;
    printf("Cm = {Pk, Pb} = { ");
    tmp = Capsconvert(in.Pk.getString());
    len = 40 - tmp.length();
    appendzero = appendzero.append(len, '0');
    cout << appendzero << tmp << ", ";
    appendzero = "";
    tmp = Capsconvert(in.Pb.getString());
    len = 40 - tmp.length();
    appendzero = appendzero.append(len, '0');
    cout << appendzero << tmp << " }" << endl;
}
point OctetToPoint(string s)
{
    point res;
    string tmp = s.substr(0, 2);
    s = s.substr(2, s.length() - 1);
    /*
    //octet string to integer
    string temp;
    B_int twop, M_i;
    B_int x = "0";
    for (int i = 0; i < mlen; i++)
    {
        if (i == mlen - 1)
            temp = s;
        else
            temp = s.substr(s.length() - 2, s.length() - 1);
        s = s.substr(0, s.length() - 2);
        M_i = temp;
        twop = i * 8;
        x = (x + power(two, twop, p * ten) * M_i);
    }*/
    B_int x = s;
    B_int yp;
    if (tmp == "02")
        yp = "0";
    else if (tmp == "03")
        yp = "1";
    else
    {
        printf("error");
    }
    B_int alpha;
    alpha = (x * x * x + a * x + b) % p;
    B_int beta;
    beta = cipolla(alpha, yp);
    res.x = x;
    res.y = beta;
    return res;
}
string decryption(point Pk, point Pb, B_int na)
{
    string res;
    point tmp = scalarmulti(na, Pk);
    point Pm = pluspt(Pb, inverse(tmp));
    res = Capsconvert(Pm.x.getString().substr(0, Pm.x.getString().length() - 2));
    return res;
}
int main()
{
    G.x = Gx;
    G.y = Gy;
    string s;
    printf("<EC-ElGamal encryption>\n");
    //scan plaintext
    printf("Plaintext M = ");
    cin >> s;
    s = Capsconvert(s);
    B_int M = s;
    point Pm = integertopoint(M);

    printf("Pa = ");
    cin >> s;
    s = Capsconvert(s);
    //take first two elemnt to last
    point Pa = OctetToPoint(s);
    printf("nk = ");
    cin >> s;
    s = Capsconvert(s);
    B_int nk = s;
    printpoint(Pm);
    Ciphertext out;
    out = encryption(Pm, Pa, nk);
    printcipher(out);
    printf("\n");
    //decryption
    printf("<EC-ElGamal decryption>\n");
    printf("Pk = ");
    cin >> s;
    s = Capsconvert(s);
    point Pk = OctetToPoint(s);
    B_int tmp;
    printf("Pb = ");
    cin >> s;
    s = Capsconvert(s);
    point Pb = OctetToPoint(s);
    printf("na = ");
    cin >> s;
    s = Capsconvert(s);
    B_int na = s;
    cout << "Plaintext = " << decryption(Pk, Pb, na) << endl;
    return 0;
}