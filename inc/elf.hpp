#ifndef _ELF_H
#define _ELF_H

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <map>

//#include <bits/stdcpp.h> ukljcuuje sve ali moze da zezne hahahha

using namespace std;
/*
#.symtab
╔═════╤═══════╤══════╤═══════╤══════╤═════╤════════╗
║ Num │ Value │ Size │ Type  │ Bind │ Ndx │ Name   ║
║─────┼───────┼──────┼───────┼──────┼─────┼────────║
║ 0   │ 0     │ 0    │ NOTYP │ LOC  │ UND │        ║
║ 1   │ 0     │ 0    │ SCTN  │ LOC  │ 1   │ .text  ║
║ 2   │ 0     │ 0    │ NOTYP │ LOC  │ 1   │ label1 ║
║ 3   │ 1     │ 0    │ NOTYP │ LOC  │ 1   │ label2 ║
║ 4   │ 3     │ 0    │ NOTYP │ LOC  │ 1   │ label3 ║
║ 5   │ 8     │ 0    │ NOTYP │ LOC  │ 1   │ label4 ║
║ 6   │ 11    │ 0    │ NOTYP │ LOC  │ 1   │ label5 ║
║ 7   │ 0     │ 0    │ NOTYP │ GLOB │ UND │ sym    ║
║ 8   │ 0     │ 0    │ NOTYP │ GLOB │ UND │ sym2   ║
╚═════╧═══════╧══════╧═══════╧══════╧═════╧════════╝*/
//value vrednost simbola
//size velicina uvek nula velicina sekcije
//tupe je tip simbola: simboli imaju razl tipove i razl se ponasaju
//ntype je obicna labela
//sctn je sekcija
//name je ime
//ndx je indeks sekcije kojoj taj simbol pripada

/*
#.rela.text
╔════════╤═════════╤══════════╤════════╗
║ Offset │ Type    │ Symbol   │ Addend ║
║────────┼─────────┼──────────┼────────║
║ 14     │ REL_ABS │ 7 (sym)  │ 0      ║
║ 19     │ REL_PC  │ 8 (sym2) │ -2     ║
╚════════╧═════════╧══════════╧════════╝ 
analogno
domaci!
tabela simbola je globalna, a ova relokaciona tabela i za sadrzaj su za svaku sekciju, po jedan vektor za svaku sekicju
imacemo i mapu u kojoj ce biti vektori kao vrednosti a kljuc ce da bude ime sekcije
symbol je indeks simbola u tabeli simbola(koji simbol treba da se stavi na to mesto) a ne sam simbol, bolje int

addent predstavlja vrednost koja se sabira sa simbolom 

to su indeksiu onoj gore tabeli 
offset predstavlja lokaciju unutar sekcije gde treba staviti taj relokacioni zapis
primer: neka je sadrzaj text sekcije:
00 1A 23 57 FF 39 00 00 15
to je sadzaj neke sekcije 
offset moze da predstavlja broj 6 i to znaci da od lokacije 6 treba da se zapise vrednost sumbola (00)
REL_ABS: kolika god je vrednost simbola, ti nju stavi na mesto ofseta 
sym je 120, ja cu uzeti 120 i staviti gde treba

REL_PC: pc relativno kod jumpova 
razlika od trenutnog pc-a do destinacije 
primer: 00 1A 23 57 FF 39 00 03 15 44 33 55
desio se neki relok zapis i upisala se trojka, znaci da od te lokacije pa 3 lokacije desno treba da se pomeri pc tj da se vrednost nalazi na tri lokacije desno 
on ce preskociti 15 i 44 i 33 krenuce od 55 zbog te trojke

00 1A 23 57 FF 39 00 17 15

╔════════╤═════════╤══════════╤════════╗
║ Offset │ Type    │ Symbol   │ Addend ║
║────────┼─────────┼──────────┼────────║
║ 6      │ REL_ABS │ 1 (lab1) │ 2      ║  
╚════════╧═════════╧══════════╧════════╝ 

╔═════╤═══════╤══════╤═══════╤══════╤═════╤════════╗
║ Num │ Value │ Size │ Type  │ Bind │ Ndx │ Name   ║
║─────┼───────┼──────┼───────┼──────┼─────┼────────║
║ 0   │ 0     │ 0    │ NOTYP │ LOC  │ UND │        ║
║ 1   │ 15    │ 0    │ OBJ   │ LOC  │ 1   │  lab1  ║
╚═════╧═══════╧══════╧═══════╧══════╧═════╧════════╝




*/

/*
#.text
╔════════╤═════════════════════════╗
║ Offset │ Values                  ║
║────────┼─────────────────────────║
║ 0000   │ 00 04 00 a0 00 03 00 04 ║
║ 0008   │ 6c 6f 6c 50 f5 03 00 00 ║
║ 0010   │ 53 f7 05 00 00          ║
╚════════╧═════════════════════════╝

binarni sadrzaj koji predstavlja sve instrukcije iret, halt.. 
i to prevodim u binarne instrukcije
niz charova
necemo uzeti string jer neki karakteri ne mogu da se isprintaju npr 00
niz ce biti vektor
td vektor karaktera
*/

enum tipSimbola{NOTYP, SCTN, SYM, EXTRN, MOD};
//sym je obicna labela
//sctn sekcija
enum bindSimbola{LOC, GLOB};
enum tipRelokacije{REL_ABS, REL_PC};


struct ulazTabeleRelokacije{
  //da li ovaj symbol da mi bude string ili int?
  int offset;
  tipRelokacije type;
  string symbol;
  int addend;
  ulazTabeleRelokacije()=default;
  ulazTabeleRelokacije(int offset, tipRelokacije type, string symbol, int addend):offset(offset), type(type), symbol(symbol), addend(addend){}
};


struct ulazTabelaSimbola
{
  int num;
  int value;
  int size;
  tipSimbola type;
  bindSimbola bind;
  int ndx;
  string name;
  ulazTabelaSimbola()=default;
  ulazTabelaSimbola(int num, int value, int size, tipSimbola type, bindSimbola bind, int ndx, string name) : num(num), value(value), size(size), type(type), bind(bind), ndx(ndx), name(name){}
  //prvi krug popunjavanja tabele simbola je ekstern
};

class Elf
{

public:


  vector<ulazTabelaSimbola> vektorTabeleSimbola;
  map<string, vector<ulazTabeleRelokacije>> mapaRelokacija;
  map<string, vector<char>> mapaSadrzaj;

  ulazTabelaSimbola losUlazTabSimb;

  void dodajUTabeluSimbola(int num, int value, int size,tipSimbola type, bindSimbola bind,int ndx, string name);

  ulazTabelaSimbola dohvatiIzTabeleSimbola(string name);

//id sekcije (string) 
  void dodajUTabeluRelok(string id, int offset, tipRelokacije type, string symbol, int addend);

  bool postojiTakva(string name);
  bool postojiTakvaRelok(string name);

  string kojojSekcijiPripada(string nazivSimbola);
  int komFajluPripada(string nazivSimbola);

  //za svaku sekicju poseban
  void dodajUSadrzaj(string name, char c); 
  void dodajUSadrzajDva(string name, char c1, char c2); 
  void dodajUSadrzajTri(string name, char c1, char c2, char  c3); 
  void dodajUSadrzajCetiri(string name, char c1, char c2, char c3, char c4); 
  void dodajUSadrzajPet(string name, char c1, char c2, char c3, char c4, char c5);

  int dohvNextNumSimb() const;
  int dohvUlazTrenSekc(string trenSekc)const;

  void izmeniTabeluSimbola(ulazTabelaSimbola uts, string trenutnaSekcija);

  void azurirajSizeSekcija();

  void ispisiTekst(string imeFajla);

  string typeUString(tipSimbola type);

  string tipUStringR(int type);

  string bindUString(int bind);

  string ndxUString(int ndx);
    
};

#endif