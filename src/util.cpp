#include <iostream>
#include <fstream>
#include <cstring>
#include "util.hpp"
using namespace std;

map<string, char> mnemonik = {
  {"halt", 0x00}, {"iret", 0x20}, {"ret", 0x40},
  {"int", 0x10}, {"not", 0x80}, {"add", 0x70}, {"sub", 0x71}, {"mul", 0x72}, {"div", 0x73}, {"cmp", 0x74}, {"xchg", 0x60}, {"and", 0x81}, {"or", 0x82}, {"xor", 0x83}, {"test", 0x84}, {"shl", 0x90}, {"shr", 0x91}, {"call", 0x30}, {"jmp", 0x50}, {"jeq", 0x51}, {"jne", 0x52}, {"jgt", 0x53}, {"ldr", 0xA0}, {"str", 0xB0}, {"pop", 0xA0}, {"push", 0xB0}
};

map<string, int> brojRegistara = {
  {"halt", 0}, {"iret", 0}, {"ret", 0},
  {"int", 1}, {"not", 1}, {"add", 2}, {"sub", 2}, {"mul", 2}, {"div", 2}, {"cmp", 2}, {"xchg", 2}, {"and", 2}, {"or", 2}, {"xor", 2}, {"test", 2},{"shl", 2}, {"shr", 2}, {"call", -1}, {"jmp", -1}, {"jeq", -1}, {"jne", -1}, {"jgt", -1}, {"ldr", -2}, {"str", -2}, {"pop", 1}, {"push", 1}
};

string letters = "qwertyuiopasdfghjklzxcvbnm";

string dohvIzlazniFajl(int argc, char *argv[]){
  for(int i = 0;i < argc; i++)
  {
    if(strcmp(argv[i],"-o") == 0){
      //vracam naredni element jer je naredni el ime
      if(i+1<argc){
        return argv[i+1];
      } else {
        return "";
      }
    }
  }
  return "";
}
int prebroj1(unsigned int a) {
  int r = 0;
  while (a) {
    r ++;
    a -= a&(-a);
  }
  return r;
}


string izbaciKomentare(string linija){
  for(int i = 0; i < linija.size(); i++){
    int pom = 0;

    if(linija[i] == '#'){
      pom = i;
      linija.erase(pom);
    }
  }
  return linija;
}

string izbaciBlankoPre(string linija){
  while(linija.size() && linija[0] ==' ' && linija[0] != '\t'){
    linija.erase(0, 1);
  }
  return linija;
}

string izbaciBlankoPosle(string linija){
  while(linija.size() && linija[linija.size()-1] == ' ' && linija[linija.size()-1] != '\t'){
    int pom = linija.size()-1;
    linija.erase(pom, pom + 1);
  }
  return linija;
}

string izbaciUglasteZagrade(string linija){
  //[reg]
  linija = izbaciBlankoPre(linija);
  if(linija.size() && linija[0] == '['){
    linija.erase(0,1);
  }
  linija = izbaciBlankoPre(linija);
  linija = izbaciBlankoPosle(linija);
  if(linija.size() && linija[linija.size()-1] == ']'){
    int pom = linija.size()-1;
    linija.erase(pom, pom + 1);
  }
  linija = izbaciBlankoPosle(linija);

  return linija;
}

int dohvatiPrviReg(string linija, string instr){
  instr = instr + " r";
  int sizeInst = instr.size();
  string x = linija.substr(sizeInst, 1);
  int prviReg=stoi(x);
  return prviReg;
}

int dohvatiDrugiReg(string linija, string instr, int prvi){
  string prviString = to_string(prvi);
  instr = instr + " r" + prviString + ", r";
  int sizeInst = instr.size();
  string s = linija.substr(sizeInst);
  if(s.size()>1 && !isdigit(stoi(s))){
    cout<<"nedefinisan simbol!! ovakav registar ne postoji"<<endl;
  }
  string x = linija.substr(sizeInst, 1);
  int drugiReg=stoi(x);
  return drugiReg;
}

string decUHex(string x){
  int n = stoi(x);
  string hex = "";
  char hexChars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  if(n!=0){
    while (n > 0) {
      hex = hexChars[n % 16] + hex;
      n /= 16;
    }
  } else{
    hex = '0';
  }
  return hex;
}

string decToHexaStrInt(int n)
{
    char hexaDeciNum[100];
    int i = 0;
    while(n != 0)
    {   
         
        int temp  = 0;
        temp = n % 16;
        if (temp < 10)
        {
            hexaDeciNum[i] = temp + 48;
            i++;
        }
        else
        {
            hexaDeciNum[i] = temp + 87;
            i++;
        }
        n = n / 16;
    }
     
    string ans = "";
       
    for(int j = i - 1; j >= 0; j--)
    {
        ans += hexaDeciNum[j];
    }
    return ans;
}

string binaryToHex(string binary) {
    map<string, char> binToHex = {
        {"0000", '0'}, {"0001", '1'}, {"0010", '2'}, {"0011", '3'},
        {"0100", '4'}, {"0101", '5'}, {"0110", '6'}, {"0111", '7'},
        {"1000", '8'}, {"1001", '9'}, {"1010", 'A'}, {"1011", 'B'},
        {"1100", 'C'}, {"1101", 'D'}, {"1110", 'E'}, {"1111", 'F'}
    };

    int n = binary.length();
    string hex = "";
    for (int i = 0; i < n; i += 4) {
        string chunk = binary.substr(i, 4);
        hex += binToHex[chunk];
    }
    return hex;
}


int octalToDecimal(int n)
{
    int num = n;
    int dec_value = 0;
    int base = 1;
   
    int temp = num;
    while (temp)
    {
         
        int last_digit = temp % 10;
        temp = temp / 10;
        
        dec_value += last_digit * base;
   
        base = base * 8;
    }
    return dec_value;
}



void dodajURela(string simbol, string trenutnaSekcija, string type, Elf &elf){
  
  ulazTabelaSimbola uts1 = elf.dohvatiIzTabeleSimbola(simbol);
  if(uts1.name != elf.losUlazTabSimb.name){
    
      if(type == "abs"){
        elf.dodajUTabeluRelok(trenutnaSekcija, elf.mapaSadrzaj[trenutnaSekcija].size()-2, REL_ABS, uts1.name, 0);
      } else if(type == "pc"){
        elf.dodajUTabeluRelok(trenutnaSekcija, elf.mapaSadrzaj[trenutnaSekcija].size()-2, REL_PC, uts1.name, 0);
      } else{
        //cout << "los tip?" <<endl;
      }
  } else{
      if(type == "abs"){
        int indeksTrenutneSekcije = elf.dohvUlazTrenSekc(trenutnaSekcija);
        int sizePom = elf.mapaSadrzaj[trenutnaSekcija].size();
        if(indeksTrenutneSekcije == -1){
          elf.dodajUTabeluSimbola(0, sizePom, 0, MOD, LOC, -2, simbol);
          //ovde sam nula stavila umesto undefined
        }  else{

          elf.dodajUTabeluSimbola(0, sizePom, 0, MOD, LOC, indeksTrenutneSekcije, simbol);
        }
        ulazTabelaSimbola uts2 = elf.dohvatiIzTabeleSimbola(simbol);
        elf.dodajUTabeluRelok(trenutnaSekcija, elf.mapaSadrzaj[trenutnaSekcija].size()-2, REL_ABS, uts2.name, 0);

      } else if(type == "pc"){
        int indeksTrenutneSekcije = elf.dohvUlazTrenSekc(trenutnaSekcija);
        int sizePom = elf.mapaSadrzaj[trenutnaSekcija].size();
        if(indeksTrenutneSekcije == -1){
          elf.dodajUTabeluSimbola(0, sizePom, 0, MOD, LOC, -2, simbol);
        }  else{
          elf.dodajUTabeluSimbola(0, sizePom, 0, MOD, LOC, indeksTrenutneSekcije, simbol);
        }
        ulazTabelaSimbola uts2 = elf.dohvatiIzTabeleSimbola(simbol);
        elf.dodajUTabeluRelok(trenutnaSekcija, elf.mapaSadrzaj[trenutnaSekcija].size()-2, REL_PC, uts2.name, 0);
        
      } else{
        //cout << "los tip?" <<endl;
      }

  }

}

bool jeLiteral(){
  return false;
}

bool jeRegistar(){
  return false;
}

bool jeSimbol(){
  return false;
}

void proveriPodatak(string trenutnaSekcija, string linija, string instr, int r, Elf &elf){

  int sizeInst = instr.size() + 1;
  if ( r/10 == 0){
    sizeInst += 4;
  } else{
    sizeInst += 5;
  }

  string x = linija.substr(sizeInst);
  if(x.find("$")==0){
    if(isdigit(x[1])){
      int vr;
      string lit = x.substr(1);
      if(lit.size()>1 && lit[1]=='x'){
        string hex = lit.substr(2);
        vr = stoi(hex, nullptr, 16);
      }else{
        string pom = decUHex(lit);
        vr = stoi(pom, nullptr, 16);
      }
      int ds = (r<<4) | 0; 
      int niziBiti = vr & 0xff; 
      int visiBiti = (vr >> 8) & 0xff; 
      int s = 0; //0 00 
      elf.dodajUSadrzajPet(trenutnaSekcija, mnemonik[instr], (char)ds, (char)s, (char)niziBiti, (char)visiBiti);
    } else if(x[1]=='-'){
      string litt = x.substr(2);
      int vr = stoi(litt, nullptr, 16);
      vr = 0 - vr;
      int ds = (r<<4) | 0; 
      int niziBiti = vr & 0xff; 
      int visiBiti = (vr >> 8) & 0xff; 
      int s = 0; 
      elf.dodajUSadrzajPet(trenutnaSekcija, mnemonik[instr], (char)ds, (char)s, (char)niziBiti, (char)visiBiti);
    } else if(x[1]=='@' | x[1]=='!' | x[1]=='#' | x[1]=='$' | x[1]=='%' | x[1]=='^' | x[1]=='&' | x[1]=='*' | x[1]=='(' | x[1]==')' | x[1]=='_' | x[1]=='+' | x[1]=='~'){
      cout<<"takav simbol ne moze biti definisan."<<endl;
    } else {
      string simb = x.substr(1); //ovde ce biti sam simbol bez $
    
      int ds = (r<<4) | 0; 
      int nula = (0<<7)& 0xFF;
      elf.dodajUSadrzajPet(trenutnaSekcija, mnemonik[instr], (char)ds, (char)nula, (char)nula, (char)nula);
      dodajURela(simb, trenutnaSekcija, "abs", elf);
      
    }
  } else if(x.find("%")==0){
    //%simbol cekam simbole
    string simb = x.substr(1);
    int ds = (r<<4) | 7; 
    int s = (0<<7) | 3;
    int nula = (0<<7)& 0xFF;
    elf.dodajUSadrzajPet(trenutnaSekcija, mnemonik[instr], (char)ds, (char)s, (char)nula, (char)nula);
    dodajURela(simb, trenutnaSekcija, "pc", elf);
  }
  else if(isdigit(x[0])){
    //cout<< "podatak literal" <<endl;
    //literal
    int vr;
    string lit = x;
    if(lit[1]=='x'){
      string hex = lit.substr(2);
      vr = stoi(hex, nullptr, 16);
    } else{
      string pom = decUHex(lit);
      vr = stoi(pom, nullptr, 16);
    }
    int ds = (r<<4) | 0; 
    int niziBiti = vr & 0xff; 
    int visiBiti = (vr >> 8) & 0xff; 
    int s = (0<<7) | 4;
    elf.dodajUSadrzajPet(trenutnaSekcija, mnemonik[instr], (char)ds, (char)s, (char)niziBiti, (char)visiBiti);

  } else if(x[0] == 'r' && isdigit(x[1])){
    //cout<< "podatak reg" <<endl;
    int s = dohvatiDrugiReg(linija, instr, r);
    int ds = (r<<4) | s; 
    int pom = (0<<7) | 1;
    elf.dodajUSadrzajTri(trenutnaSekcija, mnemonik[instr], (char)ds, (char)pom);
  } else if(x.find("[")!=0){
    //cout<< "podatak simb" <<endl;
    int ds = (r<<4) | 0; 
    int s = (0<<7) | 4;
    int nula = (0<<7)& 0xFF;
    elf.dodajUSadrzajPet(trenutnaSekcija, mnemonik[instr], (char)ds, (char)s, (char)nula, (char)nula);
    dodajURela(x, trenutnaSekcija, "abs", elf);
  } else if(x.find("[")==0 && x[1] == 'r' &&  x.find("+") == -1){
    //cout<< "podatak [reg]" <<endl;
    //[reg]
    string linijica = izbaciUglasteZagrade(x);
    string pom = linijica.substr(1); //ovde izbacujem r da bih dobila koji je registar u pitanju 
    int pompom = stoi(pom );
    int s = (0<<7) | 2;
    int ds = (r<<4) | pompom; 
    elf.dodajUSadrzajTri(trenutnaSekcija, mnemonik[instr], (char)ds, (char)s);
  } else if(x.find("[")==0 && x[1] == 'r' &&  x.find("+") != -1){
    int indexPlusa = x.find("+");
    if(isdigit(x[indexPlusa + 2])){
      //cout<< "podatak [reg+literal]" <<endl;
      //[reg + literal]
      string linijica = izbaciUglasteZagrade(x);
      //cout<<linijica<<endl;
      string pom = linijica.substr(1);
      //cout<<pom<<endl;
      string brojReg = "";
      while(isdigit(pom[0])){
        brojReg += pom[0];
        pom = pom.substr(1);
      }
      int s = stoi(brojReg);
      int d = (0<<7) | 3;    
      linijica = x.substr(indexPlusa+1);
      string lit = izbaciUglasteZagrade(linijica);
      int vr;
      if(lit[1]=='x'){
        string hex = lit.substr(2);
        vr = stoi(hex, nullptr, 16);
      } else{
        string pom11 = decUHex(lit);
        vr = stoi(pom11, nullptr, 16);
      }
      int ds = (r<<4) | s; 
      int niziBiti = vr & 0xff; 
      int visiBiti = (vr >> 8) & 0xff; 
      elf.dodajUSadrzajPet(trenutnaSekcija, mnemonik[instr], (char)ds, (char)d, (char)niziBiti, (char)visiBiti);
    }
    else {
      //cout<< "podatak [reg+simbol]" <<endl;
      //[reg + simbol]
      string linijica = izbaciUglasteZagrade(x);
      string pom = linijica.substr(1); //brisem r
      string brojReg = "";
      while(isdigit(pom[0])){
        brojReg += pom[0];
        pom = pom.substr(1);
      }
      int s = stoi(brojReg);
      int ds = (r<<4) | s; 
      int d = (0<<7) | 3;    
      linijica = x.substr(indexPlusa+1);
      string simb = izbaciUglasteZagrade(linijica);
      int nula = (0<<7)& 0xFF;
      elf.dodajUSadrzajPet(trenutnaSekcija, mnemonik[instr], (char)ds, (char)d, (char)nula, (char)nula);
      dodajURela(simb, trenutnaSekcija, "abs", elf);
    }
  }
}

void proveriSkok(string trenutnaSekcija, string linija, string instr, Elf &elf){

  int sizeInst = instr.size() + 1;

  string x = linija.substr(sizeInst);
  if(x.find("%")==0){
    //%simbol
    string simb = x.substr(1);
    int ds = (15<<4) | 7; 
    int s = (0<<7) | 5;
    int nula = (0<<7)& 0xFF;
    elf.dodajUSadrzajPet(trenutnaSekcija, mnemonik[instr], (char)ds, (char)s, (char)nula, (char)nula);
    dodajURela(simb, trenutnaSekcija, "pc", elf);
  } else if(x.find("*")==0){
    x = x.substr(1); //skidam zvezdu
    if(isdigit(x[0])){
      //cout<< "skok * literal"<<endl;
      int vr;
      string lit = x;
      if(lit[1]=='x'){
        string hex = lit.substr(2);
        vr = stoi(hex, nullptr, 16); 
      } else{
        string pom = decUHex(lit); 
        vr = stoi(pom, nullptr, 16); 
        //  STEVU PITAJ JE L OVAKO MOZE
      }
      int ds = 0xf0; 
      int niziBiti = vr & 0xff; 
      int visiBiti = (vr >> 8) & 0xff; 
      int s = 4;
      elf.dodajUSadrzajPet(trenutnaSekcija, mnemonik[instr], (char)ds, (char)s, (char)niziBiti, (char)visiBiti);
    } else if(x[0] == 'r' && isdigit(x[1])){
      //cout<< "skok * reg"<<endl;
      //*registar
      string pom = x.substr(1); //skinula r
      string brojReg = "";
      while(isdigit(pom[0])){
        brojReg += pom[0];
        pom = pom.substr(1);
      }
      int s = stoi(brojReg);
      int ds = (15<<4) | s; 
      int d = (0<<7) | 1;
      elf.dodajUSadrzajTri(trenutnaSekcija, mnemonik[instr], (char)ds, (char)d);
    } else if(x.find("[")!=0){
      //cout<< "skok * simbol"<<endl;
      //*simbol
      string simb = x;
      int ds = (15<<4) | 0; 
      int s = (0<<7) | 4;
      int nula = (0<<7)& 0xFF;
      elf.dodajUSadrzajPet(trenutnaSekcija, mnemonik[instr], (char)ds, (char)s, (char)nula, (char)nula);
      dodajURela(simb, trenutnaSekcija, "abs", elf);
    } else if(x.find("[")==0 && x[1] == 'r' &&  x.find("+") == -1){
      //cout<< "skok * [reg]"<<endl;
      //*[reg]
      string linijica = izbaciUglasteZagrade(x);
      string pom = linijica.substr(1); //ovde izbacujem r da bih dobila koji je registar u pitanju 
      int pompom = stoi(pom);
      int s = (0<<7) | 2;
      int ds = (15<<4) | pompom; 
      elf.dodajUSadrzajTri(trenutnaSekcija, mnemonik[instr], (char)ds, (char)s);
    } else if(x.find("[")==0 && x[1] == 'r' &&  x.find("+") != -1){
      int indexPlusa = x.find("+");
      if(isdigit(x[indexPlusa + 2])){
        //cout<< "skok * [reg+literal]"<<endl;
        //*[reg + literal]
        string linijica = izbaciUglasteZagrade(x);
        string pom = linijica.substr(1);
        string brojReg = "";
        while(isdigit(pom[0])){
          brojReg += pom[0];
          pom = pom.substr(1);
        }
        int s = stoi(brojReg);
        int ds = (15<<4) | s; 
        int d = (0<<7) | 3;    
        linijica = x.substr(indexPlusa+1);
        string lit = izbaciUglasteZagrade(linijica);
        int vr;
        if(lit[1]=='x'){
          string hex = lit.substr(2);
          vr = stoi(hex, nullptr, 16);
        } else{
        string pom = decUHex(lit);
        vr = stoi(pom, nullptr, 16);
        }
        int niziBiti = vr & 0xff; 
        int visiBiti = (vr >> 8) & 0xff; 
        elf.dodajUSadrzajPet(trenutnaSekcija, mnemonik[instr], (char)ds, (char)d, (char)niziBiti, (char)visiBiti);
      }
      else {
        //*[reg + simbol]
        //cout<< "skok * [reg+simbol]"<<endl;
        string linijica = izbaciUglasteZagrade(x);
        string pom = linijica.substr(1); //brisem r
        string brojReg = "";
        while(isdigit(pom[0])){
          brojReg += pom[0];
          pom = pom.substr(1);
        }
        int s = stoi(brojReg);
        int ds = (15<<4) | s; 
        int d = (0<<7) | 3;    
        linijica = x.substr(indexPlusa+1);
        string simb = izbaciUglasteZagrade(linijica);
        int nula = (0<<7)& 0xFF;
        elf.dodajUSadrzajPet(trenutnaSekcija, mnemonik[instr], (char)ds, (char)d, (char)nula, (char)nula);
        dodajURela(simb, trenutnaSekcija, "abs", elf);
      }
    }

  } else if(isdigit(x[0])){
    //cout<< "skok literal"<<endl;
    //literal
    int vr;
    string lit = x;
    if(lit[1]=='x'){
      string hex = lit.substr(2);
      vr = stoi(hex, nullptr, 16);
    } else{
        string pom = decUHex(lit);
        vr = stoi(pom, nullptr, 16);
    }
    int ds = 0xF0; 
    int niziBiti = vr & 0xff; 
    int visiBiti = (vr >> 8) & 0xff; 
    int s = (0<<7) & 0xFF;
    elf.dodajUSadrzajPet(trenutnaSekcija, mnemonik[instr], (char)ds, (char)s, (char)niziBiti, (char)visiBiti);
  } else{
    string simb = x;
    int ds = 0xF0;
    int s = 0x00;
    int nula = 0x00;
    elf.dodajUSadrzajPet(trenutnaSekcija, mnemonik[instr], ds, (char)s, (char)nula, (char)nula);
    dodajURela(simb, trenutnaSekcija, "abs", elf);
  }
}

