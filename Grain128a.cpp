#include <iostream>
#include <vector>
using namespace std;

class Grain{
    public:
        void initialization();
        void keystreamgen(vector<bool> &ks);
        Grain(bool key[128], bool IV[96], int ks_len);
    private:
        bool NFSR[128];
        bool LFSR[128];
        bool key[128];
        bool IV[96];
        bool NFSRbit();
        bool LFSRbit();
        bool output();
        int ks_len;
};

Grain::Grain(bool key_[128], bool IV_[96], int ks_len_){
    for (int i = 0; i < 128;++i){
        key[i] = key_[i];
    }
    for (int i = 0; i < 96;++i){
        IV[i] = IV_[i];
    }
    ks_len = ks_len_;
}

bool Grain::LFSRbit(){
    bool LFSR_128 = LFSR[0] ^ LFSR[7] ^ LFSR[38] ^ LFSR[70] ^ LFSR[81] ^ LFSR[96];
    return LFSR_128;
}

bool Grain::NFSRbit(){
    bool NFSR_128 = LFSR[0] ^ NFSR[0] ^ NFSR[26] ^ NFSR[56] ^ NFSR[91] ^ NFSR[96] ^ (NFSR[3] & NFSR[67]) ^ (NFSR[11] & NFSR[13]) ^ (NFSR[17] & NFSR[18]) ^ (NFSR[27] & NFSR[59]) ^ (NFSR[40] & NFSR[48]) ^ (NFSR[61] & NFSR[65]) ^ (NFSR[68] & NFSR[84]) ^ (NFSR[88] & NFSR[92] & NFSR[93] & NFSR[95]) ^ (NFSR[22] & NFSR[24] & NFSR[25]) ^ (NFSR[70] & NFSR[78] & NFSR[82]);
    return NFSR_128;
}

bool Grain::output(){
    bool hx = (NFSR[12] & LFSR[8]) ^ (LFSR[13] & LFSR[20]) ^ (NFSR[95] & LFSR[42]) ^ (LFSR[60] & LFSR[79]) ^ (NFSR[12] & NFSR[95] & LFSR[94]);
    bool z = NFSR[2] ^ NFSR[15] ^ NFSR[36] ^ NFSR[45] ^ NFSR[64] ^ NFSR[73] ^ NFSR[89] ^ hx ^ LFSR[93];
    return z;
}

void Grain::initialization(){
    for (int i = 0; i < 128;++i){
        NFSR[i] = key[i];
    }
    for (int i = 0; i < 96;++i){
        LFSR[i] = IV[i];
    }
    for (int i = 96; i < 127;++i){
        LFSR[i] = 1;
    }
    LFSR[127] = 0;

    bool n, l, z;
    for (int i = 0; i < 256; ++i){
        n = NFSRbit();
        l = LFSRbit();
        z = output();
        for (int j = 0; j < 127;++j){
            NFSR[j] = NFSR[j + 1];
            LFSR[j] = LFSR[j + 1];
        }
        NFSR[127] = n ^ z;
        LFSR[127] = l ^ z;
        
        /*
        //输出每轮状态
        for (int i = 0; i < 128;++i){
            cout << NFSR[i];
        }
            cout << endl;
        for (int i = 0; i < 128;++i){
            cout << LFSR[i];
        }
            cout << endl;
        */ 
    }   
}

void Grain::keystreamgen(vector<bool> &ks){
    bool n, l;
    for (int i = 0; i < ks_len;++i){
        ks.push_back(output());
        n = NFSRbit();
        l = LFSRbit();       
        for (int j = 0; j < 127;++j){
            NFSR[j] = NFSR[j + 1];
            LFSR[j] = LFSR[j + 1];
        }
        NFSR[127] = n;
        LFSR[127] = l;
    }
}

int main(){   
    bool key[128];
    bool IV[96];
    int ks_len = 256;//输入想要输出的密钥流比特长度
    vector<bool> ks;
    for (int i = 0; i < 128;++i){
        key[i] = 0;
    }
    for (int i = 0; i < 96;++i){
        IV[i] = 0;
    }
    Grain keystream = Grain(key, IV, ks_len);
    keystream.initialization(); 
    keystream.keystreamgen(ks);
    vector<bool>::iterator i;
    for (i = ks.begin(); i != ks.end();++i){
        cout << *i;
    }
}
