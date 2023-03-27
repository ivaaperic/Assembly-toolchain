#include "linker.hpp"

struct strukturaVelicinaFajl{
  int velicina;
  int fajl;
  strukturaVelicinaFajl()=default;
  strukturaVelicinaFajl(int velicina, int fajl) : velicina(velicina), fajl(fajl){}
};

void ispisiIzlazLinkera(string izlazniFajl, vector<unsigned char> izlaz){
  FILE *fp;
  fp = fopen(izlazniFajl.c_str() , "w");

  int adresa = 0;
  for (int i =0; i < izlaz.size()/8 + ((izlaz.size()%8)!=0); i++) {
    fprintf(fp,"%04X: ", adresa);
    for (int i2 = 0; i2 < 8; ++i2) {
      int inx = i*8 + i2;
      if (inx >= izlaz.size()) break;
      
      fprintf(fp,"0x%02X ", izlaz[inx]);
      
    }
    adresa += 8;
    fprintf(fp,"\n");
  }
  fprintf(fp,"\n");

}

int dohvatiAdresu(vector<string>sveSekcije, map<string, vector<strukturaVelicinaFajl>> mapaSekVelFajl, string sekcija, int fajl){
  vector<string> sekcijePreMene;
  int zbir = 0;
  for(int i = 0; i<sveSekcije.size(); i++){
    if(sveSekcije[i]!=sekcija){
      sekcijePreMene.push_back(sveSekcije[i]);
    } else{
      i = sveSekcije.size();
    }
  }

  for(int j = 0; j< sekcijePreMene.size();j++){
    for (auto it = mapaSekVelFajl.begin(); it != mapaSekVelFajl.end(); ++it) {
      if(it->first == sekcijePreMene[j]){
        for(int s = 0 ; s< it->second.size(); s++){
          zbir += it->second[s].velicina;
        }
      }
    }
  }
  for (auto it = mapaSekVelFajl.begin(); it != mapaSekVelFajl.end(); ++it) {
    if(it->first == sekcija){
      for(int s = 0 ; s< it->second.size(); s++){
        if(it->second[s].fajl<fajl){
          zbir += it->second[s].velicina;
        }
        
      }
    }
  }
  return zbir;
}

void linkuj(vector<string> ulazniFajlovi, string izlazniFajl){
  vector<Elf> elfovi;

  for(int k = 0; k < ulazniFajlovi.size(); k++){
    Elf elf;
    ifstream fajl(ulazniFajlovi[k]);
    string linija;

    for (; getline(fajl, linija); ) {

      if(linija == "║ Num │ Value │ Size │ Type  │ Bind │ Ndx │ Name         ║"){
        int num;
        int value;
        int size;
        tipSimbola type;
        bindSimbola bind;
        int ndx;
        string name;
        getline(fajl, linija);
        getline(fajl, linija);
        getline(fajl, linija);
        while(linija!="╚═════╧═══════╧══════╧═══════╧══════╧═════╧══════════════╝"){
          
          //NUM:
          if( isdigit(linija[5]) && isdigit(linija[6])){
            string linija56 = linija.substr(5,2);
            num = stoi(linija56);
          }else if(!isdigit(linija[5]) && isdigit(linija[6])){
            string linija6 = linija.substr(6,1);
            num = stoi(linija6);
          }

          //VALUE:
          if(isdigit(linija[14]) && isdigit(linija[15]) && isdigit(linija[16])){
            //trocifren
            string linija14 = linija.substr(14,3);
            value = stoi(linija14);
          }
          else if(!isdigit(linija[14])&& isdigit(linija[15]) && isdigit(linija[16])){
            string linija15 = linija.substr(15,2);
            value = stoi(linija15);
          }else if(!isdigit(linija[14]) && !isdigit(linija[15]) && isdigit(linija[16])){
            string linija16 = linija.substr(16,1);
            value = stoi(linija16);
          }

          //SIZE:
          if(isdigit(linija[22])&& isdigit(linija[23]) && isdigit(linija[24]) && isdigit(linija[25])){
            string linija22 = linija.substr(22,4);
            size = stoi(linija22);
          }else if(!isdigit(linija[22])&& isdigit(linija[23]) && isdigit(linija[24]) && isdigit(linija[25])){
            //trocifren
            string linija23 = linija.substr(23,3);
            size = stoi(linija23);
          }
          else if(!isdigit(linija[22])&&!isdigit(linija[23])&& isdigit(linija[24]) && isdigit(linija[25])){
            string linija24 = linija.substr(24,2);
            size = stoi(linija24);
          }else if(!isdigit(linija[22])&&!isdigit(linija[23]) && !isdigit(linija[24]) && isdigit(linija[25])){
            string linija25 = linija.substr(25,1);
            size = stoi(linija25);
          }

          //enum tipSimbola{NOTYP, SCTN, SYM, EXTRN, MOD};
          //TIP:
          if(linija[31] == 'N' && linija[32]== 'O' && linija[33]=='T'&& linija[34]=='Y' && linija[35]=='P'){
            type = tipSimbola::NOTYP;
          }
          if(linija[32]== 'S' && linija[33]=='C'&& linija[34]=='T' && linija[35]=='N'){
            type = tipSimbola::SCTN;
          }
          if(linija[33]=='S'&& linija[34]=='Y' && linija[35]=='M'){
            type = tipSimbola::SYM;
          }
          if(linija[31] == 'E' && linija[32]== 'X' && linija[33]=='T'&& linija[34]=='R' && linija[35]=='N'){
            type = tipSimbola::EXTRN;
          }
          if(linija[33]=='M'&& linija[34]=='O' && linija[35]=='D'){
            type = tipSimbola::MOD;
          }
          // cout<<elf.typeUString(type)<<endl;

          //enum bindSimbola{LOC, GLOB};
          //BIND:
          if(linija[41] == 'G' && linija[42]== 'L' && linija[43]=='O'&& linija[44]=='B'){
            bind = bindSimbola::GLOB;
          }
          if(linija[42]=='L'&& linija[43]=='O' && linija[44]=='C'){
            bind = bindSimbola::LOC;
          }
          // cout<<elf.bindUString(bind)<<endl;

          //NDX:
          if( isdigit(linija[51]) && isdigit(linija[52])){
            string linija5152 = linija.substr(51,2);
            ndx = stoi(linija5152);
          }else if(!isdigit(linija[51]) && isdigit(linija[52])){
            string linija52 = linija.substr(52,1);
            ndx = stoi(linija52);
          }
          // cout<<ndx<<endl;

          string linijapom = linija.substr(58, 12); //labela sa razmacima
          name = izbaciUglasteZagrade(linijapom);
          // cout<<name<<endl;

          elf.dodajUTabeluSimbola(num, value, size, type, bind, ndx, name);
          
          getline(fajl, linija);
        }
      }

      if(linija == "╔════════════════════════╗"){
        getline(fajl, linija);
        //║Values :        my_code ║ ekstraktuj my_code tj sekciju i stavi je u neki string
        string sekcija;
        if(linija.find("Values :")){
          string linijapom = linija.substr(12, 14); //labela sa razmacima
          sekcija = izbaciUglasteZagrade(linijapom);
        }
        getline(fajl, linija);
        getline(fajl, linija);
        for (int i = 2 ; i<linija.size(); ){
          // cout<<linija[i]<<linija[i+1]<<endl;
          string d = linija.substr(i, 2);
          // cout<<d<<endl;
          int decimal = stoi(d, NULL, 16);
          int ds = decimal | 0x00;
          elf.dodajUSadrzaj(sekcija, (char)ds);

          i = i + 5;
        }


        getline(fajl, linija);
        getline(fajl, linija);
        getline(fajl, linija);
        getline(fajl, linija);
        getline(fajl, linija);
        getline(fajl, linija);
        getline(fajl, linija);
        while(linija!= "╚════════╧═════════╧════════════════╧════════╝"){

          //enum tipRelokacije{REL_ABS, REL_PC};
          int offset;
          tipRelokacije tip;
          string symbol;
          int addend=0;

          //OFFSET:
          if(isdigit(linija[5]) && isdigit(linija[6])&& isdigit(linija[7]) && isdigit(linija[8]) && isdigit(linija[9])){
            string linija56789 = linija.substr(5,5);
            offset = stoi(linija56789);
          }else if(!isdigit(linija[5]) && isdigit(linija[6])&& isdigit(linija[7]) && isdigit(linija[8]) && isdigit(linija[9])){
            string linija6789 = linija.substr(6,4);
            offset = stoi(linija6789);
          }else if(!isdigit(linija[5]) && !isdigit(linija[6])&& isdigit(linija[7]) && isdigit(linija[8]) && isdigit(linija[9])){
            //trocifren
            string linija789 = linija.substr(7,3);
            offset = stoi(linija789);
          }
          else if(!isdigit(linija[5]) && !isdigit(linija[6])&& !isdigit(linija[7]) && isdigit(linija[8]) && isdigit(linija[9])){
            string linija89 = linija.substr(8,2);
            offset = stoi(linija89);
          }else if(!isdigit(linija[5]) && !isdigit(linija[6])&& !isdigit(linija[7]) && !isdigit(linija[8]) && isdigit(linija[9])){
            string linija9 = linija.substr(9,1);
            offset = stoi(linija9);
          }
          // cout<<offset<<endl;

          //TIP RELOKACIJE:
          if(linija[13] == 'R' && linija[14]== 'E' && linija[15]=='L'&& linija[16]=='_'&& linija[17]=='A'&& linija[18]=='B'&& linija[19]=='S'){
            tip = tipRelokacije::REL_ABS;
          }
          if(linija[14] == 'R' && linija[15]== 'E' && linija[16]=='L'&& linija[17]=='_'&& linija[18]=='P'&& linija[19]=='C'){
            tip = tipRelokacije::REL_PC;
          }

          //SIMBOL:
          string linijapom = linija.substr(23, 14); //labela sa razmacima
          symbol = izbaciUglasteZagrade(linijapom);

          elf.dodajUTabeluRelok(sekcija, offset, tip, symbol, addend);

          getline(fajl, linija);
        }

      }
    }
    elf.ispisiTekst("provera.txt");
    elfovi.push_back(elf);
  }

  vector<unsigned char> izlaz;
  vector<string>sekcije; //naziv 
  map<string, vector<strukturaVelicinaFajl>> mapaSekVelFajl;
  for(int k = 0; k<elfovi.size();k++){
    /*
    ABS kolika je vrednost simbola, to napisem na mesto
    PC je vrednost pc - vrednost labele , a vrednost pc-a je offset od pocetka izlaznog fajla
    */
    for(int m = 0; m<elfovi[k].vektorTabeleSimbola.size(); m++){
      if(elfovi[k].vektorTabeleSimbola[m].type==SCTN){
        if (count(sekcije.begin(), sekcije.end(), elfovi[k].vektorTabeleSimbola[m].name)) {
          //cout << "Postoji vec takva sekcija, nadovezacu je na postojecu";
          strukturaVelicinaFajl str = {elfovi[k].vektorTabeleSimbola[m].size, k};
          mapaSekVelFajl[elfovi[k].vektorTabeleSimbola[m].name].push_back(str);
        } else {
          //cout << "Ne postoji ta sekcija, pravim novu";
          sekcije.push_back(elfovi[k].vektorTabeleSimbola[m].name);
          strukturaVelicinaFajl str = {elfovi[k].vektorTabeleSimbola[m].size, k};
          mapaSekVelFajl[elfovi[k].vektorTabeleSimbola[m].name].push_back(str);
        }
      }
    }
  }
  for(int j = 0; j<sekcije.size(); j++){
    for(int k = 0; k<elfovi.size();k++){
      for (int indeks = 0 ; indeks<elfovi[k].mapaSadrzaj[sekcije[j]].size(); indeks++){
        izlaz.push_back((unsigned char)elfovi[k].mapaSadrzaj[sekcije[j]][indeks]);
      }
    }
  }

  for(int k = 0; k<elfovi.size();k++){
    for(int s = 0; s < sekcije.size(); s++){
      if(elfovi[k].postojiTakvaRelok(sekcije[s])){
        for (auto it = elfovi[k].mapaRelokacija[sekcije[s]].begin(); it != elfovi[k].mapaRelokacija[sekcije[s]].end(); ++it) {
          int adresa = it->offset; 
          string simbol = it->symbol;
          tipRelokacije tip = it->type;
          string sekcijaKojojPripada = "";
          int njegovaVrednost = -1;
          int vrednost = 0;

          for(int elff = 0; elff<elfovi.size(); elff++){
            for(int tabsim = 0; tabsim<elfovi[elff].vektorTabeleSimbola.size(); tabsim++){
              if(elfovi[elff].vektorTabeleSimbola[tabsim].name == simbol && elfovi[elff].vektorTabeleSimbola[tabsim].type == tipSimbola::SYM){
                sekcijaKojojPripada = elfovi[elff].kojojSekcijiPripada(simbol);
                njegovaVrednost = elfovi[elff].vektorTabeleSimbola[tabsim].value;
                vrednost += njegovaVrednost; 
                if(sekcijaKojojPripada!= "/"){
                  adresa += dohvatiAdresu(sekcije, mapaSekVelFajl, sekcije[s], k);
                  vrednost += dohvatiAdresu(sekcije, mapaSekVelFajl, sekcijaKojojPripada, elff);

                  if(tip == REL_PC){
                    vrednost = vrednost-adresa-2;
                  }
                  int niziBiti = vrednost & 0xff; 
                  int visiBiti = (vrednost >> 8) & 0xff; 
                  izlaz[adresa] = (unsigned char)niziBiti;
                  izlaz[adresa+1] = (unsigned char)visiBiti;

                } else{
                  // cout<<"symbol "<<simbol<<" ne pripada nijednoj sekciji"<<endl;
                }
              } else{
                //  cout<<"greska, ne postoji fajl u kojem se nalazi simbol "<< simbol<<" gde je on tipa SYM u tabeli simbola"<<endl;
              }
            }
          }
        }
      }
    }
  }
  ispisiIzlazLinkera(izlazniFajl, izlaz);
}

int main(int argc, char *argv[]) {
  vector<string> ulazniFajlovi;
  string izlazniFajl;
  if(argc>4){
    if(string(argv[0]) == "./linker"){
      if(string(argv[1])=="-hex"){
        if(string(argv[2])== "-o"){
          if(string(argv[3]).find(".hex")){
            izlazniFajl = string(argv[3]);
            for(int i = 4; i<argc;i++){
              if(string(argv[i]).find(".o")){
                ulazniFajlovi.push_back(argv[i]);
              } else{
                cout<<"pogresni fajlovi"<<endl;
                return -1;
              }
            }
            linkuj(ulazniFajlovi, izlazniFajl);
          } else{
            cout<<"posle -o mora .hex fajl"<<endl;
          }
          
        } else{ //./linker -hex file1.o fajl.0 fajl.0 fajl.0 -o program.hex
          if(string(argv[argc-1]).find(".hex") && string(argv[argc-2])== "-o"){
            izlazniFajl = string(argv[argc-1]);
            for(int i = 2; i<argc-2;i++){
              if(string(argv[i]).find(".o")){
                ulazniFajlovi.push_back(argv[i]);
              } else{
                cout<<"pogresni fajlovi"<<endl;
                return -1;
              }
            }
            linkuj(ulazniFajlovi, izlazniFajl);
          }
          cout<<"greska: posle -hex mora -o"<< endl;
        }
      }else if(string(argv[1])=="-o"){
        if(string(argv[2]).find(".hex")){
          izlazniFajl = string(argv[2]);
          if(string(argv[3])== "-hex"){
            for(int i = 4; i < argc; i++){
              if(string(argv[i]).find(".o")){
                ulazniFajlovi.push_back(argv[i]);
              } else{
                cout<<"pogresni fajlovi"<<endl;
                return -1;
              }
            }
            linkuj(ulazniFajlovi, izlazniFajl);
          }else{
            cout<<"greska: los format!"<<endl;
          }
        } else{
          cout<<"posle -o mora da ide izlazni fajl"<<endl;
        }
      } else{
        cout<<"greska: los raspored"<<endl;
      }
    } else{
      cout<<"greska: los format"<<endl;
    }
  }else{
    cout<<"nedovoljno argumenata"<<endl;
  }
  
  //linker -hex -o izlaznifajl.hex ulazni1.o.txt ulazni2.o.txt
  //linker -o izlaznifajl.hex -hex ulazni1.o.txt ulazni2.o.txt
  //linker -hex ulazni1.o.txt ulazni2.o.txt
  //linker ulazni1.o.txt ulazni2.o.txt
}