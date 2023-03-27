#include "emulator.hpp"

#define PC 7
#define SP 6
#define PSWZ 1
#define PSWO 2
#define PSWC 4
#define PSWN 8


void emuliraj(string ulazniFajl){
  unsigned short regs[8];
  unsigned short psw;
  unsigned char mem[65536];

  string linija;
  ifstream fajl(ulazniFajl);
  int pomAdresa = 0;
  for (; getline(fajl, linija); ) {
    for (int i = 8 ; i<linija.size(); ){
      string d = linija.substr(i, 2);
      int decimal = stoi(d, NULL, 16);
      int ds = decimal | 0x00;
      i = i + 5;
      mem[pomAdresa++] = ds;
      
    }
  }
  for (int i = 0; i< 8; i++){
    regs[i] = 0;
  }

  bool bioPush = false;
  int pushCnt = 0;

  regs[PC] = (mem[1]<<8) | mem[0];

  while(1){
    // cout<<"PC: "<<endl;
    // printf("0x%04x\n", regs[PC]);
    // printf("PSW : 0x%04x\n", psw);
    
    // printf("0x%04x\n", mem[regs[PC]]);
    unsigned char prviBajt = mem[regs[PC]++];
    // printf("PRVI BAJT : 0x%02x\n", prviBajt);
    
    
    if(prviBajt == 0x00){
      //HALT
      break;
    }else if(prviBajt == 0x20){
      //IRET
      unsigned short adr = (unsigned short)regs[SP];
      unsigned short x = 0;
      x |= mem[adr+1]; 
      x <<= 8;
      x |= mem[adr];
      regs[SP] += 2; 
      psw = x; 
      adr = (unsigned short)regs[SP];
      x = 0;
      x |= mem[adr+1];
      x <<= 8;
      x |= mem[adr];
      regs[SP] += 2;
      regs[PC] = x; 
      // cout<<"PSW nakon iret"<<psw<<endl;
    }else if(prviBajt == 0x40){
      //RET
      // cout<<"ret"<<endl;
      unsigned short adr = (unsigned short)regs[SP];
      unsigned short x = 0;
      x |= mem[adr+1];
      x <<= 8;
      x |= mem[adr];
      regs[SP] += 2;
      regs[PC] = (short)x; 
    } else{
      //drugi bajt
      unsigned char drugiBajt = mem[regs[PC]++];
      
      int d = drugiBajt>>4;
      int s = drugiBajt&0x0F;
      // cout<<"d "<<d<<endl;
      // cout<<"s "<<s<<endl;
      if(prviBajt == 0x10){
        //INT
        // cout<<"int"<<endl;
        regs[SP] -= 1;
        mem[(unsigned short)regs[SP]] = regs[PC]>>8;
        regs[SP] -= 1;
        mem[(unsigned short)regs[SP]] = regs[PC]&255;
        regs[SP] -= 1;
        mem[(unsigned short)regs[SP]] = psw>>8;
        regs[SP] -= 1;
        mem[(unsigned short)regs[SP]] = psw&255;
        unsigned short adr = (unsigned short)((((unsigned short)regs[d])%8)*2), x=0;
        x |= mem[adr+1];
        x <<= 8;
        x |= mem[adr];
        regs[PC] = x; 
      }else if(prviBajt == 0x80){
        // cout<<"not"<<endl;
        regs[d] = ~regs[d];
      } else if(prviBajt == 0x60){
        // cout<<"xchg"<<endl;
        unsigned short pom = regs[d];
        regs[d] = regs[s];
        regs[s] = pom;
      }else if(prviBajt == 0x70){
        // cout<<"add"<<endl;
        regs[d] = regs[d]+regs[s];
      }else if(prviBajt == 0x71){
        // cout<<"sub"<<endl;
        regs[d] = regs[d]-regs[s];
      }else if(prviBajt == 0x72){
        // cout<<"mul"<<endl;
        regs[d] = regs[d]*regs[s];
      }else if(prviBajt == 0x73){
        // cout<<"div"<<endl;
        if (regs[s] == 0) cout<<"ne moze se deliti nulom"<<endl;
        else{
          regs[d] = regs[d]/regs[s];
        }
      }else if(prviBajt == 0x74){
        // cout<<"cmp"<<endl;
        int vrednostC = ((short)regs[d] < (short)regs[s]);
        int vrednostO = ((short)(regs[d]-regs[s]) != ((int)regs[d]-(int)regs[s]));
        psw &= 0xFFFF^PSWC;
        psw |= (vrednostC?PSWC:0); //ili setuj ili nula
        psw &= 0xFFFF^PSWO;
        psw |= (vrednostO?PSWO:0);
        short pom = regs[d]-regs[s]; 
        int vrednostN = (((unsigned short)pom & 0x8000)!=0);
        psw &= 0xFFFF^PSWN;
        psw |= (vrednostN?PSWN:0);
        int vrednostZ = (pom == 0);
        psw &= 0xFFFF^PSWZ;
        psw |= (vrednostZ?PSWZ:0);
        // cout<<"PSW nakon cmp"<<psw<<endl;
      }else if(prviBajt == 0x81){
        // cout<<"and"<<endl;
        regs[d] = regs[d]&regs[s];
      }else if(prviBajt == 0x82){
        // cout<<"or"<<endl;
        regs[d] = regs[d]|regs[s];
      }else if(prviBajt == 0x83){
        // cout<<"xor"<<endl;
        regs[d] = regs[d]^regs[s];
      }else if(prviBajt == 0x84){
        // cout<<"test"<<endl;
        short pom = regs[d]&regs[s]; 

        int vrednostN = (((unsigned short)pom & 0x8000)!=0);
        psw &= 0xFFFF^PSWN;
        psw |= (vrednostN?PSWN:0);
        int vrednostZ = (pom == 0);
        psw &= 0xFFFF^PSWZ;
        psw |= (vrednostZ?PSWZ:0);
        // cout<<"PSW nakon test"<<psw<<endl;
      }else if(prviBajt == 0x90){
        // cout<<"shl"<<endl;
        int pom = (prebroj1(regs[d])!=prebroj1(regs[d]<<regs[s]));
        psw &= 0xFFFF^PSWC;
        psw |= (pom?PSWC:0);
        regs[d] = regs[d] << regs[s]; 
        int vrednostN = (((unsigned short)regs[d] & 0x8000)!=0);
        psw &= 0xFFFF^PSWN;
        psw |= (vrednostN?PSWN:0);
        int vrednostZ = (regs[d] == 0);
        psw &= 0xFFFF^PSWZ;
        psw |= (vrednostZ?PSWZ:0);
        // cout<<"PSW nakon shl"<<psw<<endl;
      }else if(prviBajt == 0x91){
        // cout<<"shr"<<endl;
        int pom = (prebroj1(regs[d])!=prebroj1(regs[d]>>regs[s]));
        psw &= 0xFFFF^PSWC;
        psw |= (pom?PSWC:0);
        regs[d] = regs[d] >> regs[s]; 
        int vrednostN = (((unsigned short)regs[d] & 0x8000)!=0);
        psw &= 0xFFFF^PSWN;
        psw |= (vrednostN?PSWN:0);
        int vrednostZ = (regs[d] == 0);
        psw &= 0xFFFF^PSWZ;
        psw |= (vrednostZ?PSWZ:0);
        // cout<<"PSW nakon shr"<<psw<<endl;
      }else{
        // cout<<"vise bajtova"<<endl;
        unsigned char treciBajt = mem[regs[PC]++];
        int t2 = treciBajt&0xF;
        int t1 = treciBajt>>4;
        // cout<<"t1: "<<t1<<endl;
        // cout<<"t2: "<<t2<<endl;

        if(prviBajt == 0x30 && d == 0xF){
          // cout<<"call"<<endl;
           //na stek sam stavila pc da ga lepo uzme ret
          if(t2 == 1){//reg dir
            regs[SP] -= 1;
            mem[(unsigned short)regs[SP]] = regs[PC]>>8;
            regs[SP] -= 1;
            mem[(unsigned short)regs[SP]] = regs[PC]&255;
            regs[PC] = (short)regs[s];
          } else if(t2 == 2){//reg ind 
            if (t1==1) regs[s] -= 2;
            else if (t1==2) regs[s] += 2;
            unsigned short tmp = regs[s];
            if (t1==3) regs[s] -= 2;
            else if (t1==4) regs[s] += 2;
            unsigned short adr = (unsigned short)((int)tmp), x=0;
            x |= mem[adr+1];
            x <<= 8;
            x |= mem[adr];
            regs[SP] -= 1;
            mem[(unsigned short)regs[SP]] = regs[PC]>>8;
            regs[SP] -= 1;
            mem[(unsigned short)regs[SP]] = regs[PC]&255;
            regs[PC] = (short)x;
          } else if(t2 == 0){//neposr 
            unsigned char cetvrtiBajt = mem[regs[PC]++];
            unsigned char petiBajt = mem[regs[PC]++];
            regs[SP] -= 1;
            mem[(unsigned short)regs[SP]] = regs[PC]>>8;
            regs[SP] -= 1;
            mem[(unsigned short)regs[SP]] = regs[PC]&255;
            regs[PC] = (short)((petiBajt<<8) | cetvrtiBajt);
            
          } else if(t2 == 5){//pc rel
            unsigned char cetvrtiBajt = mem[regs[PC]++];
            unsigned char petiBajt = mem[regs[PC]++];
            regs[SP] -= 1;
            mem[(unsigned short)regs[SP]] = regs[PC]>>8;
            regs[SP] -= 1;
            mem[(unsigned short)regs[SP]] = regs[PC]&255;
            regs[PC] = (short)(regs[s]+ (short)((petiBajt<<8) | cetvrtiBajt));
          } else if(t2 == 4){//mem dir
            unsigned char cetvrtiBajt = mem[regs[PC]++];
            unsigned char petiBajt = mem[regs[PC]++];
            unsigned short adr = (unsigned short)((int)((unsigned short)((petiBajt<<8) | cetvrtiBajt))), x=0;
            x |= mem[adr+1];
            x <<= 8;
            x |= mem[adr];
            regs[SP] -= 1;
            mem[(unsigned short)regs[SP]] = regs[PC]>>8;
            regs[SP] -= 1;
            mem[(unsigned short)regs[SP]] = regs[PC]&255;
            regs[PC] = (short)x;
          } else if(t2 == 3){//regindpom
            unsigned char cetvrtiBajt = mem[regs[PC]++];
            unsigned char petiBajt = mem[regs[PC]++];
            if (t1==1) regs[s] -= 2;
            else if (t1==2) regs[s] += 2;
            unsigned short tmp = regs[s];
            if (t1==3) regs[s] -= 2;
            else if (t1==4) regs[s] += 2;
            unsigned short adr = (unsigned short)((int)(tmp+(unsigned short)((petiBajt<<8) | cetvrtiBajt))), x=0;
            x |= mem[adr+1];
            x <<= 8;
            x |= mem[adr];
            regs[SP] -= 1;
            mem[(unsigned short)regs[SP]] = regs[PC]>>8;
            regs[SP] -= 1;
            mem[(unsigned short)regs[SP]] = regs[PC]&255;
            regs[PC] = (short)x;
          } else{
            break;
            cout<<"pogresan nacin adresiranja za call"<<endl;
          }

        }else if(prviBajt == 0x50 && d == 0xF){
          if(t2 == 1){//reg dirx
            regs[PC] = regs[s];
          } else if(t2 == 2){//reg ind
            if (t1==1) regs[s] -= 2;
            else if (t1==2) regs[s] += 2;
            unsigned short tmp = regs[s];
            if (t1==3) regs[s] -= 2;
            else if (t1==4) regs[s] += 2;
            unsigned short adr = tmp, x=0;
            x |= mem[ adr+1 ];
            x <<= 8;
            x |= mem[ adr ];
            regs[PC] = x;
          } else if(t2 == 0){//neposr
            // cout<<regs[PC]<<endl;
            unsigned char cetvrtiBajt = mem[regs[PC]++];
            // cout<<regs[PC]<<endl;
            unsigned char petiBajt = mem[regs[PC]++];
            regs[PC] = (petiBajt<<8) | cetvrtiBajt;
          } else if(t2 == 5){//pc rel
            unsigned char cetvrtiBajt = mem[regs[PC]++];
            unsigned char petiBajt = mem[regs[PC]++];
            regs[PC] = regs[s]+ ((petiBajt<<8) | cetvrtiBajt);
          } else if(t2 == 4){//mem dir
            unsigned char cetvrtiBajt = mem[regs[PC]++];
            unsigned char petiBajt = mem[regs[PC]++];
            unsigned short adr = (unsigned short)((petiBajt<<8) | cetvrtiBajt), x=0;
            x |= mem[adr+1];
            x <<= 8;
            x |= mem[adr];
            regs[PC] = x;
          } else if(t2 == 3){//regindpom
            unsigned char cetvrtiBajt = mem[regs[PC]++];
            unsigned char petiBajt = mem[regs[PC]++];
            if (t1==1) regs[s] -= 2;
            else if (t1==2) regs[s] += 2;
            unsigned short tmp = regs[s];
            if (t1==3) regs[s] -= 2;
            else if (t1==4) regs[s] += 2;
            unsigned short adr = (unsigned short)(tmp + (unsigned short)((petiBajt<<8) | cetvrtiBajt)); 
            unsigned short x=0;
            x |= mem[adr+1];
            x <<= 8;
            x |= mem[adr];
            regs[PC] = x;
          } 
          else{
            cout<<"pogresan nacin adresiranja za jmp"<<endl;
          }
        }else if(prviBajt == 0x51 && d == 0xF){
          if (psw&PSWZ){
            if(t2 == 1){//reg dir
              regs[PC] = regs[s];
            } else if(t2 == 2){//reg ind
              if (t1==1) regs[s] -= 2;
              else if (t1==2) regs[s] += 2;
              unsigned short tmp = regs[s];
              if (t1==3) regs[s] -= 2;
              else if (t1==4) regs[s] += 2;
              unsigned short adr = tmp, x=0;
              x |= mem[ adr+1 ];
              x <<= 8;
              x |= mem[ adr ];
              regs[PC] = x;
            } else if(t2 == 0){//neposr
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              regs[PC] = (petiBajt<<8) | cetvrtiBajt;
            } else if(t2 == 5){//pc rel
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              regs[PC] = regs[s]+ ((petiBajt<<8) | cetvrtiBajt);
            } else if(t2 == 4){//mem dir
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              unsigned short adr = (unsigned short)((petiBajt<<8) | cetvrtiBajt), x=0;
              x |= mem[adr+1];
              x <<= 8;
              x |= mem[adr];
              regs[PC] = x;
            } else if(t2 == 3){//regindpom
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              if (t1==1) regs[s] -= 2;
              else if (t1==2) regs[s] += 2;
              unsigned short tmp = regs[s];
              if (t1==3) regs[s] -= 2;
              else if (t1==4) regs[s] += 2;
              unsigned short adr = (unsigned short)(tmp + (unsigned short)((petiBajt<<8) | cetvrtiBajt)); 
              unsigned short x=0;
              x |= mem[adr+1];
              x <<= 8;
              x |= mem[adr];
              regs[PC] = x;
            } 
            else{
              cout<<"pogresan nacin adresiranja za jeq"<<endl;
            }

          }
        }else if(prviBajt == 0x52 && d == 0xF ){
          if (!(psw&PSWZ)){
            if(t2 == 1){//reg dir
              regs[PC] = regs[s];
            } else if(t2 == 2){//reg ind
              if (t1==1) regs[s] -= 2;
              else if (t1==2) regs[s] += 2;
              unsigned short tmp = regs[s];
              if (t1==3) regs[s] -= 2;
              else if (t1==4) regs[s] += 2;
              unsigned short adr = tmp, x=0;
              x |= mem[ adr+1 ];
              x <<= 8;
              x |= mem[ adr ];
              regs[PC] = x;
            } else if(t2 == 0){//neposr
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              regs[PC] = (petiBajt<<8) | cetvrtiBajt;
            } else if(t2 == 5){//pc rel
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              regs[PC] = regs[s]+ (petiBajt<<8) | cetvrtiBajt;
            } else if(t2 == 4){//mem dir
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              unsigned short adr = (unsigned short)((petiBajt<<8) | cetvrtiBajt), x=0;
              x |= mem[adr+1];
              x <<= 8;
              x |= mem[adr];
              regs[PC] = x;
            } else if(t2 == 3){//regindpom
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              if (t1==1) regs[s] -= 2;
              else if (t1==2) regs[s] += 2;
              unsigned short tmp = regs[s];
              if (t1==3) regs[s] -= 2;
              else if (t1==4) regs[s] += 2;
              unsigned short adr = (unsigned short)(tmp + (unsigned short)((petiBajt<<8) | cetvrtiBajt)); 
              unsigned short x=0;
              x |= mem[adr+1];
              x <<= 8;
              x |= mem[adr];
              regs[PC] = x;
            } 
            else{
              cout<<"pogresan nacin adresiranja za jne"<<endl;
            //   break;
            }
          } 
        }else if(prviBajt == 0x53 && d == 0xF){
          if (!(psw&PSWC) && !(psw&PSWZ)){
            
            if(t2 == 1){//reg dir
              regs[PC] = regs[s];
            } else if(t2 == 2){//reg ind
              if (t1==1) regs[s] -= 2;
              else if (t1==2) regs[s] += 2;
              unsigned short tmp = regs[s];
              if (t1==3) regs[s] -= 2;
              else if (t1==4) regs[s] += 2;
              unsigned short adr = tmp, x=0;
              x |= mem[ adr+1 ];
              x <<= 8;
              x |= mem[ adr ];
              regs[PC] = x;
            } else if(t2 == 0){//neposr
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              regs[PC] = (petiBajt<<8) | cetvrtiBajt;
            } else if(t2 == 5){//pc rel
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              regs[PC] = regs[s]+ (petiBajt<<8) | cetvrtiBajt;
            } else if(t2 == 4){//mem dir
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              unsigned short adr = (unsigned short)((petiBajt<<8) | cetvrtiBajt), x=0;
              x |= mem[adr+1];
              x <<= 8;
              x |= mem[adr];
              regs[PC] = x;
            } else if(t2 == 3){//regindpom
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              if (t1==1) regs[s] -= 2;
              else if (t1==2) regs[s] += 2;
              unsigned short tmp = regs[s];
              if (t1==3) regs[s] -= 2;
              else if (t1==4) regs[s] += 2;
              unsigned short adr = (unsigned short)(tmp + (unsigned short)((petiBajt<<8) | cetvrtiBajt)); 
              unsigned short x=0;
              x |= mem[adr+1];
              x <<= 8;
              x |= mem[adr];
              regs[PC] = x;
            } 
            else{
              cout<<"pogresan nacin adresiranja za jgt"<<endl;
            }
          }
        }else if(prviBajt == 0xB0){
          if(d == 0x6 && treciBajt == 0x12){
            regs[SP] -= 1;
            mem[ (unsigned short)regs[SP] ] = regs[s]>>8;
            regs[SP] -= 1;
            mem[ (unsigned short)regs[SP] ] = regs[s]&255;
            bioPush = true;
            pushCnt++;
          } else{
            //STR
            if(t2 == 1){//reg dir
              regs[s] = regs[d];
            } else if(t2 == 2){//reg ind
              if (t1==1) regs[s] -= 2;
              else if (t1==2) regs[s] += 2;
              unsigned short tmp = regs[s];
              if (t1==3) regs[s] -= 2;
              else if (t1==4) regs[s] += 2;
              mem[tmp+1] = regs[d]>>8;
              mem[tmp] = regs[d]&0xFF;
            } else if(t2 == 0 && s ==0){//neposr
                cout<<"nevalidna instrukcija, ne moze str neposredno"<<endl;
            } else if(t2 == 4 && s ==0){//mem dir
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              mem[((petiBajt<<8) | cetvrtiBajt)+1] = regs[d]>>8;
              mem[((petiBajt<<8) | cetvrtiBajt)] = regs[d]&0xFF;
            } else if(t2 == 3 && s == 7){//pc rel
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              regs[s] = regs[d]+ ((petiBajt<<8) | cetvrtiBajt);
            } else if(t2 == 3){//regindpom
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              if (t1==1) regs[s] -= 2;
              else if (t1==2) regs[s] += 2;
              unsigned short tmp = regs[s];
              if (t1==3) regs[s] -= 2;
              else if (t1==4) regs[s] += 2;
              unsigned short pom = (unsigned short)(tmp + (unsigned short)((petiBajt<<8) | cetvrtiBajt)); 
              mem[tmp+1] = regs[d]>>8;
              mem[tmp] = regs[d]&0xFF;
            } 
            else{
              cout<<"pogresan nacin adresiranja za str"<<endl;
            }

          }
          
        }else if(prviBajt == 0xA0){
          if(s == 0x6 && treciBajt == 0x42 && bioPush && t1 != 0){
            unsigned short adr = (unsigned short)regs[SP], x=0;
            x |= mem[ adr+1 ];
            x <<= 8;
            x |= mem[ adr ];
            regs[SP] += 2;
            regs[d] = (unsigned short)x;
          } else if(t1==0){
            //LDR
            if(t2 == 1){//reg dir
              regs[d] = regs[s];
            } else if(t2 == 2){//reg ind
              if (t1==1) regs[s] -= 2;
              else if (t1==2) regs[s] += 2;
              unsigned short tmp = regs[s];
              if (t1==3) regs[s] -= 2;
              else if (t1==4) regs[s] += 2;
              unsigned short adr = tmp, x=0;
              x |= mem[ adr+1 ];
              x <<= 8;
              x |= mem[ adr ];
              regs[d] = x;
            } else if(t2 == 0 && s ==0){//neposr
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              regs[d] = (petiBajt<<8) | cetvrtiBajt;
            } else if(t2 == 4 && s ==0){//mem dir
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              unsigned short adr = (unsigned short)((petiBajt<<8) | cetvrtiBajt), x=0;
              x |= mem[adr+1];
              x <<= 8;
              x |= mem[adr];
              regs[d] = x;
            } else if(t2 == 3 && s == 7){//pc rel
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              regs[d] = regs[s]+ ((petiBajt<<8) | cetvrtiBajt);
            } else if(t2 == 3){//regindpom
              unsigned char cetvrtiBajt = mem[regs[PC]++];
              unsigned char petiBajt = mem[regs[PC]++];
              if (t1==1) regs[s] -= 2;
              else if (t1==2) regs[s] += 2;
              unsigned short tmp = regs[s];
              if (t1==3) regs[s] -= 2;
              else if (t1==4) regs[s] += 2;
              unsigned short adr = (unsigned short)(tmp + (unsigned short)((petiBajt<<8) | cetvrtiBajt)); 
              unsigned short x=0;
              x |= mem[adr+1];
              x <<= 8;
              x |= mem[adr];
              regs[d] = x;
            } 
            else{
              cout<<"pogresan nacin adresiranja za ldr"<<endl;
            }
          }
          
        } 
        else{

          cout<<"nepostojeca instrukcija, emulator ne prepoznaje!"<<endl;
          break;
        }

      }
    }

  }
  //ispis:
  printf("------------------------------------------------\n");
  printf("Emulated processor executed halt instruction\n");
  printf("Emulated processor state: psw=0x%04x \n", psw);    
  printf("r0=0x%04x r1=0x%04x r2=0x%04x r3=0x%04x\n", regs[0], regs[1], regs[2], regs[3]);
  printf("r4=0x%04x r5=0x%04x r6=0x%04x r7=0x%04x\n", regs[4], regs[5], regs[6], regs[7]);

}

int main(int argc, char *argv[]) {
  if(argc!=2){
    cout<<"Potrebno je navesti samo jedan argument!"<<endl;
  } 
  else if(!ifstream(argv[1]).good()){
   cout<<"Ulazni fajl ne postoji u sistemu!"<<endl;
  }
  else{
    emuliraj(argv[1]);
  }

  return 0;
}