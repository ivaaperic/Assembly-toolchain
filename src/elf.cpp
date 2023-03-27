#include "elf.hpp"

ulazTabelaSimbola losUlazTabSimb = {-1, 0, 0, NOTYP, GLOB, 0, "/"};
static int nextNumSimb = 0;

void Elf::azurirajSizeSekcija(){
  for(int i = 0; i<vektorTabeleSimbola.size(); i++){
    if (vektorTabeleSimbola[i].type == SCTN){
      int pomSize = mapaSadrzaj[vektorTabeleSimbola[i].name].size();
      vektorTabeleSimbola[i].size = pomSize;
    }
  }
}

int Elf::dohvNextNumSimb() const{
  return nextNumSimb;
}

int Elf::dohvUlazTrenSekc(string trenSekc) const{

  for(int i = 0; i< vektorTabeleSimbola.size(); i++){
    if(vektorTabeleSimbola[i].name == trenSekc){
      return vektorTabeleSimbola[i].num;
    }
  }
  return 0;
}

void Elf::dodajUTabeluSimbola(int num, int value, int size,tipSimbola type, bindSimbola bind,int ndx, string name){
  if(vektorTabeleSimbola.size()==0){
    ulazTabelaSimbola uts1 = {0, 0, 0, NOTYP, LOC, 0, ""};
    vektorTabeleSimbola.push_back(uts1);
    nextNumSimb = 1;
  }
  if(type == MOD){
    num = nextNumSimb--;
    ulazTabelaSimbola uts = {num, value, size, type, bind, ndx, name};
    vektorTabeleSimbola.push_back(uts);
  }else{
    num = nextNumSimb;
    ulazTabelaSimbola uts = {num, value, size, type, bind, ndx, name};
    vektorTabeleSimbola.push_back(uts);
  }
  nextNumSimb++;
}

ulazTabelaSimbola Elf::dohvatiIzTabeleSimbola(string name){
  ulazTabelaSimbola uts;
  int pom = -1;
  for(int i = 0; i < vektorTabeleSimbola.size(); i++){

    if(vektorTabeleSimbola[i].name == name){
      pom = i;
    }
  }
  if(pom!= -1){
    uts.num = vektorTabeleSimbola[pom].num;
    uts.value = vektorTabeleSimbola[pom].value;
    uts.size = vektorTabeleSimbola[pom].size;
    uts.type = vektorTabeleSimbola[pom].type;
    uts.bind = vektorTabeleSimbola[pom].bind;
    uts.ndx = vektorTabeleSimbola[pom].ndx;
    uts.name = vektorTabeleSimbola[pom].name;
    return uts;
  } else {
    return losUlazTabSimb;
  } 
}

bool Elf::postojiTakva(string name){
  ulazTabelaSimbola uts1 = dohvatiIzTabeleSimbola(name);

  if(uts1.name == losUlazTabSimb.name){
    return false;
  } else {
    return true;
  }
}

bool Elf::postojiTakvaRelok(string name){
  bool flag = false;

  for (auto it = mapaRelokacija.begin(); it != mapaRelokacija.end(); ++it) {
    if(it->first != name) continue;  
    else {
      flag = true;
    }
  }
 
  return flag;
}


string Elf::kojojSekcijiPripada(string nazivSimbola){
  string sekcija = "/";
  for(int i = 0; i<vektorTabeleSimbola.size(); i++){
    if(vektorTabeleSimbola[i].name!= nazivSimbola){
      if(vektorTabeleSimbola[i].type == SCTN){
        sekcija = vektorTabeleSimbola[i].name;
      }
    } else{
      i = vektorTabeleSimbola.size();
    }
    
  }
  return sekcija;
}





void Elf::izmeniTabeluSimbola(ulazTabelaSimbola uts, string trenutnaSekcija){
  int indeksTrenutneSekcije = dohvUlazTrenSekc(trenutnaSekcija);
  if(indeksTrenutneSekcije == uts.ndx){
    vektorTabeleSimbola[uts.num].value = 0;
    vektorTabeleSimbola[uts.num].type = EXTRN;
    vektorTabeleSimbola[uts.num].bind = GLOB;
  } else{
    vektorTabeleSimbola[uts.num].ndx = -2;
    vektorTabeleSimbola[uts.num].value = 0;
    vektorTabeleSimbola[uts.num].type = EXTRN;
    vektorTabeleSimbola[uts.num].bind = GLOB;
  }
  
}

void Elf::dodajUTabeluRelok(string id, int offset, tipRelokacije type, string symbol, int addend){
  ulazTabeleRelokacije utr = {offset, type, symbol, addend};
  if (mapaRelokacija.find(id) == mapaRelokacija.end()){
    mapaRelokacija[id] = vector<ulazTabeleRelokacije>();
  }
  mapaRelokacija[id].push_back(utr);
}

void Elf::dodajUSadrzaj(string name, char c){
  mapaSadrzaj[name].push_back(c);
}


void Elf::dodajUSadrzajDva(string name, char c1, char c2){
  mapaSadrzaj[name].push_back(c1);
  mapaSadrzaj[name].push_back(c2);
}

void Elf::dodajUSadrzajTri(string name, char c1, char c2, char c3){
  mapaSadrzaj[name].push_back(c1);
  mapaSadrzaj[name].push_back(c2);
  mapaSadrzaj[name].push_back(c3);
}

void Elf::dodajUSadrzajCetiri(string name, char c1, char c2, char c3, char c4){
  mapaSadrzaj[name].push_back(c1);
  mapaSadrzaj[name].push_back(c2);
  mapaSadrzaj[name].push_back(c3);
  mapaSadrzaj[name].push_back(c4);
}

void Elf::dodajUSadrzajPet(string name, char c1, char c2, char c3, char c4, char c5){
  mapaSadrzaj[name].push_back(c1);
  mapaSadrzaj[name].push_back(c2);
  mapaSadrzaj[name].push_back(c3);
  mapaSadrzaj[name].push_back(c4);
  mapaSadrzaj[name].push_back(c5);
}

string Elf::typeUString(tipSimbola type){
  //enum tipSimbola{NOTYP, SCTN, SYM, EXTRN, MOD};
  if(type == NOTYP){
    return "NOTYP";
  } else if(type == SCTN){
    return "SCTN";
  } else if(type == SYM){
    return "SYM";
  } else if(type == EXTRN){
    return "EXTRN";
  } else if(type == MOD){
    return "MOD";
  }
  return "greska";
}

string Elf::tipUStringR(int type){
  if(type == REL_PC){
    return "REL_PC";
  } else if(type == REL_ABS){
    return "REL_ABS";
  } else {
    return "greska R";
  }
}

string Elf::bindUString(int bind){
  //enum bindSimbola{LOC, GLOB};
  if(bind == LOC){
    return "LOC";
  } else if(bind == GLOB){
    return "GLOB";
  } else {
    return "greska";
  }
    
}

string Elf::ndxUString(int ndx){
  if(ndx == -2){
    return "UND";
  } else if(ndx== -1){
    return "greska";
  } else {
    return to_string(ndx);
  }
}



void Elf::ispisiTekst(string imeFajla){
  FILE *fp;
  fp = fopen(imeFajla.c_str() , "w");

  fprintf(fp, "╔═════╤═══════╤══════╤═══════╤══════╤═════╤══════════════╗\n");
  fprintf(fp, "║ Num │ Value │ Size │ Type  │ Bind │ Ndx │ Name         ║\n");
  fprintf(fp, "║─────┼───────┼──────┼───────┼──────┼─────┼──────────────║\n");

  for(int i = 0 ; i<vektorTabeleSimbola.size(); i++){

    fprintf(fp, "║ %3d │ %5d │ %4d │ %5s │ %4s │ %3s │ %12s ║\n" , vektorTabeleSimbola[i].num, vektorTabeleSimbola[i].value, vektorTabeleSimbola[i].size,  typeUString(vektorTabeleSimbola[i].type).c_str(), bindUString(vektorTabeleSimbola[i].bind).c_str() , ndxUString(vektorTabeleSimbola[i].ndx).c_str(),  vektorTabeleSimbola[i].name.c_str());
  }

  fprintf(fp, "╚═════╧═══════╧══════╧═══════╧══════╧═════╧══════════════╝\n");
  

  for(int k = 0; k< vektorTabeleSimbola.size(); k++){
    if(vektorTabeleSimbola[k].type == SCTN){
      fprintf(fp, "╔════════════════════════╗\n");
      fprintf(fp, "║Values : %14s ║\n", vektorTabeleSimbola[k].name.c_str());
      fprintf(fp, "╚════════════════════════╝\n");
      for (auto it = mapaSadrzaj.begin(); it != mapaSadrzaj.end(); it++) {
        if(it->first != vektorTabeleSimbola[k].name) continue;
        for (int m =0; m < it->second.size(); m++) {
          fprintf(fp,"0x%02X ", (unsigned char)it->second[m]);
        }
      }
      fprintf(fp, "\n\n------------------------------\n");
      vector<ulazTabeleRelokacije> &vecPom = mapaRelokacija[vektorTabeleSimbola[k].name];
      // for (auto it = mapaRelokacija.begin(); it != mapaRelokacija.end(); ++it) {
      //   if(it->first != vektorTabeleSimbola[k].name) continue;
        fprintf(fp, ".rela.%s \n", vektorTabeleSimbola[k].name.c_str());

        fprintf(fp, "╔════════╤═════════╤════════════════╤════════╗\n");
        fprintf(fp, "║ Offset │ Type    │    Symbol      │ Addend ║\n");
        fprintf(fp, "║────────┼─────────┼────────────────┼────────║\n");

        for(int s = 0 ; s< vecPom.size(); s++){
          int indeksic = 0;
          for(int i = 0; i<vektorTabeleSimbola.size(); i++){
            if(vektorTabeleSimbola[i].name == vecPom[s].symbol){
              indeksic = i;
            }
          }
          fprintf(fp,"║ %6d | %7s | %14s | %6d ║\n", vecPom[s].offset, tipUStringR(vecPom[s].type).c_str(), vektorTabeleSimbola[indeksic].name.c_str(), vecPom[s].addend );
      
        }

        fprintf(fp, "╚════════╧═════════╧════════════════╧════════╝\n");

        fprintf(fp, "\n\n═════════════════════════════════════\n");
      // }

    }

  }
}
