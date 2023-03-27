#ifndef _UTIL_H
#define _UTIL_H

// ovde ide sve sto mi nije vezano za asembler, linker i emulator
#include <iostream>
#include <fstream>
#include <cstring>
#include "elf.hpp"
using namespace std;

string dohvIzlazniFajl(int argc, char *argv[]);
int prebroj1(unsigned int a);

string izbaciKomentare(string linija);
string izbaciBlankoPre(string linija);
string izbaciBlankoPosle(string linija);
string izbaciUglasteZagrade(string linija);

int dohvatiPrviReg(string linija, string instr);

int dohvatiDrugiReg(string linija, string instr, int prvi);

void proveriSkok(string trenutnaSekcija, string linija, string instr, Elf &elf);

void proveriPodatak(string trenutnaSekcija, string linija, string instr, int r, Elf &elf);

string decUHex(string x);
string decToHexaStrInt(int n);
int octalToDecimal(int n);
string binaryToHex(string binary);


void dodajURela(string simbol, string trenutnaSekcija, string type, Elf &elf);

bool jeLiteral();

bool jeRegistar();

bool jeSimbol();

#endif /*_UTIL_H */