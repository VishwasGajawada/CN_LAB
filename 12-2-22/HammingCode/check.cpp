#include <iostream>
#include <stdlib.h>

using namespace std;


string checkAndCorrect(string hc) {
    int mutated_bit = 0;
    for(int i = 0; i < hc.length(); i++) {
        if(((i+1)&i) == 0) {
            int one_count = 0;
            for(int j = 0; j < hc.length(); j++) {
                if(((j+1)&(i+1)) && hc[j] == '1') one_count++;
            }
            if(one_count%2 == 1) {
                mutated_bit += (i+1);
            }
        }
    }

    if(mutated_bit == 0) {
        printf("The hamming code does not have any errors\n");
        return hc;
    }else {
        printf("Error detected at bit %d\n", mutated_bit);
        int idx = mutated_bit - 1;
        hc[idx] = (hc[idx] == '1') ? '0' : '1';
        return hc;
    }
}

int main() {
    string hammingCode;
    printf("Enter the hamming Code to check: ");
    cin >> hammingCode;

    printf("Enter any bit(1 - %d) to flip, or 0 to skip: ", hammingCode.length());
    int bit;
    cin >> bit;
    if(bit != 0) {
        hammingCode[bit-1] = (hammingCode[bit-1] == '1') ? '0' : '1';
    }

    // randomly mutate a bit in the hamming code
    string corrected = checkAndCorrect(hammingCode);
    printf("The corrected hamming code is: %s\n", corrected.c_str());
    return 0;
}
// 1001 - 0011001