#include <iostream>
#include <math.h>

using namespace std;

int hammingLength(int l) {
    int r = 0; // number of redundant bits
    // r = ceil(log2(l));
    // if((l&(l-1)) == 0) r++;
    // int nl = l+r;
    // r = ceil(log2(nl));
    // if((nl&(nl-1))==0) r++;
    while(pow(2, r) < l + r + 1) r++;
    return (l + r);
}

string hammingCodeGen(string b) {
    int l = b.length();
    int hl = hammingLength(l);
    string hc(hl, '-');

    // fill positions which are  powers of 2 with b
    int idx = 0;
    for(int i = 0; i < hl; i++) {
        if(((i+1)&(i)) != 0) {
            hc[i] = b[idx++];
        }
    }
    printf("Intermediate hamming code: %s\n", hc.c_str());
    for(int i = 0; i < hl-l; i++) {
        int check = pow(2, i);
        int one_count = 0;
        for(int j = 0; j < hl; j++) {
            if(((j+1)&check) && hc[j] == '1') one_count++;
        }
        if(one_count%2 == 1) hc[check - 1] = '1';
        else hc[check - 1] = '0';
    }
    return hc;
}

int main() {
    string b;
    printf("Enter a binary string: ");
    cin >> b;

    string hammingCode = hammingCodeGen(b);
    cout << "Hamming Code: " << hammingCode << endl;
}