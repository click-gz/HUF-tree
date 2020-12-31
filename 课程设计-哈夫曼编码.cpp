
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<malloc.h>
#include<fstream>
#include<string>
#include<string.h>
#include<boost/dynamic_bitset.hpp>
#include <typeinfo>
using namespace std;
const unsigned char single = 1;
typedef struct HTNode {
    char ch;
    unsigned int weight;
    unsigned int parent, lchild, rchild;
    char* HC;
    int tot_num;
}HTNode, * HufTree;
void ProTraverse(HufTree Hf, int num, int n) {
    if (num <= n) {
        cout << Hf[num].weight << " ";
    }
    if (Hf[num].lchild != 0) {
        ProTraverse(Hf, Hf[num].lchild, n);
    }
    if (Hf[num].rchild != 0) {
        ProTraverse(Hf, Hf[num].rchild, n);
    }
}
void Init(HufTree& Hf,int &tot) {
    fstream  f;
    f.open("source.txt", ios::in);
    string  line;
    int ch[256] = { 0 };
    int tot_num = 0;
    while (getline(f, line))
    {
        for (int i = 0; i < line.size(); i++) {
            ch[int(line[i])]++;
            tot_num++;
        }
        ch[int('\n')]++;
        tot_num++;
    }
    fstream F;
    F.open("Huffman.txt", ios::out);
    for (int i = 0; i < 256; i++) {
        if (ch[i] != 0) {
            cout << i << " "<<char(i)<<" ";
            cout << ch[i] <<endl;
            F << char(i) << " " << ch[i] << endl;
            tot++;
        }
    }
    F.close();
    f.close();
    Hf = (HTNode*)malloc(2 * tot * sizeof(HTNode));
    Hf[0].weight = 10000000;
    Hf->tot_num = tot_num;
    int j = 1;
    for (int i = 0; i < 256; i++) {
        if (ch[i] != 0) {
            Hf[j].ch = char(i);
            Hf[j].weight=ch[i];
            Hf[j].lchild = 0;
            Hf[j].rchild = 0;
            Hf[j].parent = 0;
            j++;
        }
        
    }
    for (int i = tot + 1; i < 2 * tot; i++) {
        int s1 = 0, s2 = 0;
        for (int j = 1; j < i; j++) {
            if (s1 == s2) {
                if (Hf[j].parent == 0 && Hf[j].weight < Hf[s1].weight) {
                    s1 = j;
                }
            }
            else {
                if (Hf[j].parent == 0 && Hf[j].weight < Hf[s2].weight) {
                    if (Hf[j].weight < Hf[s1].weight) {
                        s2 = s1;
                        s1 = j;
                    }
                    else {
                        s2 = j;
                    }
                }
            }
        }
        Hf[s1].parent = i;
        Hf[s2].parent = i;
        Hf[i].lchild = s1;
        Hf[i].rchild = s2;
        Hf[i].parent = 0;
        Hf[i].weight = Hf[s1].weight + Hf[s2].weight;
    }
    cout << "哈夫曼树为：" << endl;
    for (int i = 1;; i++) {
        if (Hf[i].parent == 0) {
            ProTraverse(Hf, i, tot);
            break;
        }
    }
    
    for (int i = 1; i <= tot; i++) {
        Hf[i].HC = (char*)malloc(sizeof(char) * tot);
        int start = 0;
        for (int c = i, f = Hf[i].parent; f != 0; c = f, f = Hf[f].parent) {
            if (Hf[f].lchild == c) {
                Hf[i].HC[start++] = '0';
            }
            else
                Hf[i].HC[start++] = '1';
        }
        Hf[i].HC[start++] = '\0';
        for (int k = 0; k < (start-1) / 2; k++) {
            int t = Hf[i].HC[k];
            Hf[i].HC[k] = Hf[i].HC[start - k -2];
            Hf[i].HC[start - k -2] = t;
        }
    }
   
}
void get_char(string s,unsigned char &ch) {
    for (int i = 0; i < 8; i++) {
        if (s[i] == '0') {
            ch = ch << 1;
        }
        else {
            ch = ch << 1;
            ch = ch | single;
        }
    }
    
}
void Compress(HufTree& Hf, int tot, int &count_time,int &lose) {
    fstream  f;
    f.open("source.txt", ios::in);
    char word;
    int ch[256] = { 0 };
    fstream file;
    file.open("code.dat", ios::binary | ios::app);
    string copy = "";
    string s = "";
    int temp_num = 0;
    f.get(word);
    while (!f.eof())
    {
       
       for (int i = 1; i <= tot; i++) {
            if (word == Hf[i].ch) {
                cout << word;
                for (int j = 0; j < strlen(Hf[i].HC); j++) {
                    s += Hf[i].HC[j];
                    if (s.length() % 8 == 0) {
                        unsigned char ch;
                        get_char(s, ch);
                        count_time++;
                        file << ch;
                        s = "";
                    }
                }
                break;
            }
       }
       f.get(word);
    }
    lose=s.length();
    while (s.length() % 8 != 0 && s.length()!=0) {
        s += '0';
    }
    if (s.length() % 8 == 0 && s.length() != 0) {
        unsigned char ch;
        get_char(s, ch);
        file << ch;
        s = "";
    }
    f.close();
    file.close();
}
void get_bit(unsigned char *ch, string &buffer) {
    for (int i = 0; i < 8; i++) {
        buffer += (ch[0] % 2);
        ch[0] /= 2;
    }
}
void Decompress(HufTree Hf,int tot,int count_time,int lose) {
    unsigned char s = 0;
    fstream f;
    f.open("code.dat", ios::binary | ios::in);
    if (!f) {
        cout << "open error";
        exit(0);
    }
    string ss = "";
    string copy="";
    int x;
    int te = 0;
    f.read((char*)&s, sizeof(s));
    int buffer[8];
    int time = 0;
    int count = 1;
    while (true) {
        time++;
        for (int j = 1; j <= 8; j++) {
            buffer[8 - j] = int(s % 2);
            s /= 2;
        }
        for(int i=0;i<8;i++)
            copy += (buffer[i] + '0');
        ss = copy;
        //cout << copy << endl;
        string temp = "";
        int len = 0;
        if (time == count_time) {
            len = copy.length() - lose +1 ;
        }
        else
            len = copy.length();
        for (int j = 0; j < len; j++) {
            temp += copy[j];
            for (int i = 1; i <= tot; i++) {
                if (temp == Hf[i].HC) {
                    cout << Hf[i].ch;
                    if (Hf[i].ch == 'm' || Hf[i].ch == 'e' || Hf[i].ch == '.' || Hf[i].ch == '"') {
                        if (count == 1&& Hf[i].ch == 'm') {
                            count++;
                        }
                        else if (count > 1) {
                            count++;
                        }
                    }
                    else {
                        count = 1;
                    }

                    ss = "";
                    for (int t = j + 1; t < len; t++) {
                        ss += copy[t];
                    }
                    copy = ss;
                    ss = "";
                    break;
                }
            }
            if (ss == ""){
                break;
            }
        }
        if (count == 5) {
            break;
        }
        f.read((char*)&s, sizeof(s));
        
    }
    f.close();
}
int main()
{
    HufTree Hf;
    int tot = 0;
    Init(Hf,tot);
    for (int i = 0; i < tot; i++) {
        cout << Hf[i+1].ch << " " << Hf[i+1].HC << endl;
    }
    int lose = 0, count_time = 0;
    Compress(Hf, tot, count_time,lose);
    Decompress(Hf,tot,count_time,lose);
}
