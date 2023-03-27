#include "assembler.hpp"

extern map<string, char> mnemonik;
extern map<string, int> brojRegistara; 


void asembliraj(string ulazniFajl, string izlazniFajl) {
    ifstream fajl(ulazniFajl);
    int brojLinije = 1;
    string linija; 
    string orgLinija; 

    vector<string> globalniSimboli;
    vector<string> eksterniSimboli;
    string trenutnaSekcija = "";
    int indeksTrenutneSekcije = 0;

    Elf elf;

    vector<string> labele;

    for (; getline(fajl, orgLinija); brojLinije++) {
      linija = orgLinija;
      linija = izbaciKomentare(linija);
      linija = izbaciBlankoPre(linija);
      linija = izbaciBlankoPosle(linija);
      for(int i = 0; i < linija.size(); i++){
        int pom = 0;

        if(linija[i] == ':'){

          int pos = linija.find(":");
          if(pos){
            string sub = linija.substr(0 , pos);
            labele.push_back(sub);
            int indeksTrenutneSekcije = elf.dohvUlazTrenSekc(trenutnaSekcija);
            int sizePom = elf.mapaSadrzaj[trenutnaSekcija].size();
            if(indeksTrenutneSekcije == -1){
              elf.dodajUTabeluSimbola(0, sizePom, 0, SYM, GLOB, -2, sub);
              //ovde sam -2 stavila umesto undefined 
            } else{
              elf.dodajUTabeluSimbola(0, sizePom, 0, SYM, GLOB, indeksTrenutneSekcije, sub);
            }
          } 
        }
      }

      if(linija[0]=='.'){
        string direktiva = linija.substr(1);
        if(direktiva.find("global") == 0){
          string dirPom = direktiva.substr(6);
          stringstream ss(dirPom);
          string word;
          while(ss>>word){
            if(word[word.size()-1] ==','){
              int wordSize = word.size()-1;
              word = word.erase(wordSize);
            }
            globalniSimboli.push_back(word);
          }
        } else if(direktiva.find("extern")==0){
          string dirPom = direktiva.substr(6);
          stringstream ss(dirPom);
          string word;
          while(ss>>word){
            if(word[word.size()-1] ==','){
              int wordSize = word.size()-1;
              word = word.erase(wordSize);
            }
            eksterniSimboli.push_back(word);
            int indeksTrenutneSekcije = elf.dohvUlazTrenSekc(trenutnaSekcija);
            if(indeksTrenutneSekcije == -1){
              elf.dodajUTabeluSimbola(0, 0, 0, EXTRN, GLOB, -2, word);
            } else {
              elf.dodajUTabeluSimbola(0, 0, 0, EXTRN, GLOB, indeksTrenutneSekcije, word);
            }

          }
          
          
        } else if(direktiva.find("section")==0){
          string dirPom = direktiva.substr(7);
          dirPom = izbaciBlankoPosle(izbaciBlankoPre(dirPom));
          trenutnaSekcija = dirPom;
          
          if(!elf.postojiTakva(dirPom)){
            int numSekcije = elf.dohvNextNumSimb();
            if(elf.vektorTabeleSimbola.size()==0){
              numSekcije++;
            }
            elf.dodajUTabeluSimbola(0, 0, 0, SCTN, LOC, numSekcije, trenutnaSekcija);
            indeksTrenutneSekcije = numSekcije;
          } else{
            ulazTabelaSimbola utsPom = elf.dohvatiIzTabeleSimbola(dirPom);
            indeksTrenutneSekcije = utsPom.num;
          }
         
        } else if(direktiva.find("word")==0){
          vector<string> vectorPom;
          string dirPom = direktiva.substr(4);
          stringstream ss(dirPom);
          string word;
          while(ss>>word){
            if(word[word.size()-1] ==','){
              int wordSize = word.size()-1;
              word = word.erase(wordSize);
            }
            vectorPom.push_back(word);
          }
          for(int i = 0; i<vectorPom.size(); i++){
            int cnt = 0;
            string vectorPomPom = vectorPom[i];
            string vr;
            if(isdigit(vectorPomPom[0])){
              if(vectorPomPom[0]=='0'){
                if(vectorPomPom.size()>1){
                  if(vectorPomPom[1]=='x'){
                    vr = vectorPomPom.substr(2);
                    //cout<<"vr "<<vr<<endl;
                  } else if(vectorPomPom[1]=='b'){
                    vr = vectorPomPom.substr(2);
                    vr = binaryToHex(vr);

                  } else{
                    vr = vectorPomPom.substr(1);
                    vr = decToHexaStrInt(octalToDecimal(stoi(vr)));
                  }
                } else{
                  vr = decUHex(vectorPomPom);
                }

              } else{
                vr = decUHex(vectorPomPom);
              }
              if(vr.size()/2 == 0){
                int vrint = stoi(vr, nullptr, 16);
                elf.dodajUSadrzajDva(trenutnaSekcija, (char)vrint, 0);
              }
              else if(vr.size()/2 == 1){
                if(vr.size()== 2){
                  int vrrr = stoi(vr, nullptr, 16);
                  //cout<<"vrrr "<<vrrr<<endl;
                  elf.dodajUSadrzajDva(trenutnaSekcija, (char)vrrr, 0);
                } else{
                  string prvi = vr.substr(0, 1);
                  string drugi = vr.substr(1);
                  int vrint1 = stoi(prvi, nullptr, 16);
                  int vrint2 = stoi(drugi, nullptr, 16);
                  elf.dodajUSadrzajDva(trenutnaSekcija, (char)vrint2, (char)vrint1);
                }
              } else if(vr.size()/2 == 2){
                if(vr.size()==4){
                  string prvi = vr.substr(0, 2);
                  string drugi = vr.substr(2);
                  int vrint1 = stoi(prvi, nullptr, 16);
                  int vrint2 = stoi(drugi, nullptr, 16);
                  elf.dodajUSadrzajDva(trenutnaSekcija, (char)vrint2, (char)vrint1);
                } else {
                  //cout<<"preveliki heksa broj u word"<<endl;
                }

              }
              

            } else if(vectorPomPom[0]=='.'){
              int numSekcije = elf.dohvNextNumSimb();
              elf.dodajUTabeluSimbola(0, 0, 0, SCTN, LOC, numSekcije, vectorPomPom);

            }else{

              elf.dodajUSadrzajDva(trenutnaSekcija, 0, 0);
              ulazTabelaSimbola uts = elf.dohvatiIzTabeleSimbola(vectorPomPom);
              if(uts.name != elf.losUlazTabSimb.name && uts.bind == GLOB){
                elf.dodajUTabeluRelok(trenutnaSekcija, elf.mapaSadrzaj[trenutnaSekcija].size()-2, REL_ABS, uts.name, 0);
              } else {
                int indeksTrenutneSekcije = elf.dohvUlazTrenSekc(trenutnaSekcija);
                int sizePom = elf.mapaSadrzaj[trenutnaSekcija].size();
                if(indeksTrenutneSekcije == -1){
                  elf.dodajUTabeluSimbola(0, sizePom, 0, MOD, GLOB, -2, vectorPomPom);
                  //ovde sam nula stavila umesto undefined
                } else {
                  elf.dodajUTabeluSimbola(0, sizePom, 0, MOD, GLOB, indeksTrenutneSekcije, vectorPomPom);
                }
                ulazTabelaSimbola uts1 = elf.dohvatiIzTabeleSimbola(vectorPomPom);
                elf.dodajUTabeluRelok(trenutnaSekcija, elf.mapaSadrzaj[trenutnaSekcija].size()-2, REL_ABS, uts1.name, 0);
              }
            }
          }
          linija = linija.substr(5);

        }else if(direktiva.find("skip")==0){
          int vr;
          string dirPom = direktiva.substr(4);
          dirPom = izbaciBlankoPosle(izbaciBlankoPre(dirPom));
          if(isdigit(dirPom[0])){
            if(dirPom[1]== 'x'){
              string hex = dirPom.substr(2);
              vr = stoi(hex, nullptr, 16);
            } else{
              vr = stoi(dirPom);
            }
          } else {
            vr = -1;
          }
          if(vr >= 1){
            for (int i = 0; i < vr; ++i)
              elf.dodajUSadrzaj(trenutnaSekcija, 0);
          }
        }else if(direktiva.find("end")==0){
          
          break;
        } else{
          cout<<"nepostojeca direktiva"<<endl;
        }
      }

      if(linija == "halt"){
        elf.dodajUSadrzaj(trenutnaSekcija, mnemonik[linija]);
      }
      if(linija == "iret"){
        elf.dodajUSadrzaj(trenutnaSekcija, mnemonik[linija]);
      }
      if(linija == "ret"){
        elf.dodajUSadrzaj(trenutnaSekcija, mnemonik[linija]);
      }
      if(linija.find("int ")== 0){
        string x;
        string instr = "int ";
        int sizeInst = instr.size();
        string sled = linija.substr(sizeInst, 1);
        if (sled == "0"){
          sizeInst += 2;
          x = linija.substr(sizeInst, 1);
          int d=stoi(x);
          int s = 15;
          int ds = (d<<4) | s;
          elf.dodajUSadrzajDva(trenutnaSekcija, mnemonik["int"], (char)ds);
        } else if(sled== "r"){
          int d, s;
          d = dohvatiPrviReg(linija, "int");
          s = 15;
          int ds = (d<<4) | s; 
          elf.dodajUSadrzajDva(trenutnaSekcija, mnemonik["int"], (char)ds);
        } else {
          x = linija.substr(sizeInst, 1);
          int d=stoi(x);
          int s = 15;
          int ds = (d<<4) | s;
          elf.dodajUSadrzajDva(trenutnaSekcija, mnemonik["int"], (char)ds);
        }
      }
      if(linija.find("add ")== 0){
        int d, s;
        d = dohvatiPrviReg(linija, "add");
        s = dohvatiDrugiReg(linija, "add", d);
        int ds = (d<<4) | s; 
        elf.dodajUSadrzajDva(trenutnaSekcija, mnemonik["add"], (char)ds);
      }
      if(linija.find("sub ")== 0){
        int d, s;
        d = dohvatiPrviReg(linija, "sub");
        s = dohvatiDrugiReg(linija, "sub", d);
        int ds = (d<<4) | s; 
        elf.dodajUSadrzajDva(trenutnaSekcija, mnemonik["sub"], (char)ds);
      }
      if(linija.find("mul ")== 0){
        int d, s;
        d = dohvatiPrviReg(linija, "mul");
        s = dohvatiDrugiReg(linija, "mul", d);
        int ds = (d<<4) | s;
        elf.dodajUSadrzajDva(trenutnaSekcija, mnemonik["mul"], (char)ds);
      }
      if(linija.find("div ")== 0){
        int d, s;
        d = dohvatiPrviReg(linija, "div");
        s = dohvatiDrugiReg(linija, "div", d);
        int ds = (d<<4) | s; 
        elf.dodajUSadrzajDva(trenutnaSekcija, mnemonik["div"], (char)ds);
      }
      if(linija.find("cmp ")== 0){
        int d, s;
        d = dohvatiPrviReg(linija, "cmp");
        s = dohvatiDrugiReg(linija, "cmp", d);
        int ds = (d<<4) | s; 
        elf.dodajUSadrzajDva(trenutnaSekcija, mnemonik["cmp"], (char)ds);
      }
      if(linija.find("and ")== 0){
        int d, s;
        d = dohvatiPrviReg(linija, "and");
        s = dohvatiDrugiReg(linija, "and", d);
        int ds = (d<<4) | s; 
        elf.dodajUSadrzajDva(trenutnaSekcija, mnemonik["and"], (char)ds);
      }
      if(linija.find("or ")== 0){
        int d, s;
        d = dohvatiPrviReg(linija, "or");
        s = dohvatiDrugiReg(linija, "or", d);
        int ds = (d<<4) | s; 
        elf.dodajUSadrzajDva(trenutnaSekcija, mnemonik["or"], (char)ds);
      }
      if(linija.find("xor ")== 0){
        int d, s;
        d = dohvatiPrviReg(linija, "xor");
        s = dohvatiDrugiReg(linija, "xor", d);
        int ds = (d<<4) | s; 
        elf.dodajUSadrzajDva(trenutnaSekcija, mnemonik["xor"], (char)ds);
      }
      if(linija.find("test ")== 0){
        int d, s;
        d = dohvatiPrviReg(linija, "test");
        s = dohvatiDrugiReg(linija, "test", d);
        int ds = (d<<4) | s; 
        elf.dodajUSadrzajDva(trenutnaSekcija, mnemonik["test"], (char)ds);
      }
      if(linija.find("xchg ")== 0){
        int d, s;
        d = dohvatiPrviReg(linija, "xchg");
        s = dohvatiDrugiReg(linija, "xchg", d);
        int ds = (d<<4) | s; 
        elf.dodajUSadrzajDva(trenutnaSekcija, mnemonik["xchg"], (char)ds);
      }
      if(linija.find("shl ")== 0){
        int d, s;
        d = dohvatiPrviReg(linija, "shl");
        s = dohvatiDrugiReg(linija, "shl", d);
        int ds = (d<<4) | s; 
        elf.dodajUSadrzajDva(trenutnaSekcija, mnemonik["shl"], (char)ds);
      }
      if(linija.find("shr ")== 0){
        int d, s;
        d = dohvatiPrviReg(linija, "shr");
        s = dohvatiDrugiReg(linija, "shr", d);
        int ds = (d<<4) | s; 
        elf.dodajUSadrzajDva(trenutnaSekcija, mnemonik["shr"], (char)ds);
      }
      if(linija.find("not ")== 0){
        int d, s;
        d = dohvatiPrviReg(linija, "not");
        s = 0;
        int ds = (d<<4) | s; 
        elf.dodajUSadrzajDva(trenutnaSekcija, mnemonik["not"], (char)ds);
      }
      if(linija.find("pop ")== 0){
        int d, s;
        d = dohvatiPrviReg(linija, "pop");
        s = 6;
        int ds = (d<<4) | s; 
        int addrmode = 0x42;
        elf.dodajUSadrzajTri(trenutnaSekcija, mnemonik["pop"], (char)ds, (char)addrmode);
      }
      if(linija.find("push ")== 0){
        int d, s;
        s = dohvatiPrviReg(linija, "push");
        d = 6;
        int ds = (d<<4) | s; 
        int addrmode = 0x12;
        elf.dodajUSadrzajTri(trenutnaSekcija, mnemonik["push"], (char)ds, (char)addrmode);
      }
      if(linija.find("call ")== 0){
        proveriSkok(trenutnaSekcija, linija, "call", elf);
      }
      if(linija.find("jmp ")== 0){
        proveriSkok(trenutnaSekcija, linija, "jmp", elf);        
      }      
      if(linija.find("jeq ")== 0){
        proveriSkok(trenutnaSekcija, linija, "jeq", elf);        
      }
      if(linija.find("jne ")== 0){
        proveriSkok(trenutnaSekcija, linija, "jne", elf);        
      }
      if(linija.find("jgt ")== 0){
        proveriSkok(trenutnaSekcija, linija, "jgt", elf);        
      }
      if(linija.find("ldr ")== 0){
        int r = dohvatiPrviReg(linija, "ldr");
        proveriPodatak(trenutnaSekcija, linija, "ldr", r, elf);        
      }
      if(linija.find("str ")== 0){
        int r = dohvatiPrviReg(linija, "str");        
        proveriPodatak(trenutnaSekcija, linija, "str", r, elf);        
      }

    }
    for(int i = 0; i < eksterniSimboli.size(); i++){
      
      ulazTabelaSimbola uts1 = elf.dohvatiIzTabeleSimbola(eksterniSimboli[i]);
      if(uts1.name != elf.losUlazTabSimb.name){
        if(uts1.type == MOD){
          elf.vektorTabeleSimbola[uts1.num].type = EXTRN;
          elf.vektorTabeleSimbola[uts1.num].bind = GLOB;
        }
      } 
    }
    int mrs = 0;
    while(mrs < elf.vektorTabeleSimbola.size()){
      if(elf.vektorTabeleSimbola[mrs].type == MOD){
        elf.vektorTabeleSimbola.erase(elf.vektorTabeleSimbola.begin()+mrs);
        mrs--;
      }
      mrs++;
    }

    elf.azurirajSizeSekcija();
    for(int i = 0; i< labele.size(); i++){
    }

  elf.ispisiTekst(izlazniFajl);
}


int main(int argc, char *argv[]) {
  string izlazniFajl =  dohvIzlazniFajl(argc, argv);
  string ulazniFajl = argv[argc-1];
  if(izlazniFajl == ""){
    printf("Nekorektan ulazni fajl.\n");
  } else if(!ifstream(argv[argc-1]).good()){
  } else {
  }
  
  if(string(argv[2])== "-o"){
    ulazniFajl = argv[1];
  } else if(string(argv[1])== "-o"){
    ulazniFajl = argv[3];
  }
  asembliraj(ulazniFajl, izlazniFajl);

}