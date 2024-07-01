#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#ifdef _WIN32
char CLEAR[] ="cls";
#else
char CLEAR[] ="clear";
#endif

#define ENEMY "\e[31;40;1m"
#define RED "\033[214m"
#define FAL "\e[37;47;9m"
#define DEFAULT "\033[0m"
#define TALAJ "\x1b[30;40m"
#define FOHOS "\033[32;49m"
#define TARGY "\e[35;40m"
#define PENZ  "\e[33;40;1;5m"
#define BOLTOS "\e[37;40;1m"



typedef struct
{
    char nev[100];
    int seb;
    int ved;
    int megveve;
    int ar;
    int db;
} Aru;


typedef struct 
{
char nev[100];
int id;
int pos[2];
} Ajto;

Ajto *ajtok;
typedef struct 
{
char nev[100];
int id;
int pos[2];
} Kulcs;

Kulcs *kulcsok;
typedef struct
{
    char nev[999];
    int hp;
    int sebzes;
    int penz;
    int zsarolva;
    int harcban;
    int pos[2];
    int terkep;
    int aru_db;
    int kulcs_jelenleg;
    Kulcs *kulcsok;
    Aru *keszlet;
}Karakter;
Karakter *ellensegek;
Karakter *arusok;
typedef struct
{
    int sor;

    int oszlop;
    char **map;
    int ellenfelekSzama;
    int itemekSzama;
    int arusokSzama;
    int kulcs_db;
    int ajto_db;
    int kezdohely[2];
}Map;
typedef struct
{
    char nev[999];
    int def;
    int atk;
    int db;
    int pos[2];
}Itemek;
Itemek *targyak;
char next_map[100];
char itemLista[100];
char enemyLista[100];
char aruLista[100];
char ajtoLista[100];
char kulcsLista[100];
int vasarol = 0; //ha vásárlásban vagyunk akkor 1-es. ha nem akkor 0
Karakter foHos;
Map terkep;
int jatekVege = 0;
char **vak_map;
int Egyezik(char *a, char *b) //összemér 2 stringet
{
    int i,j;
    for(i = 0; a[i] != '\0'; i++) {}
    for(j = 0; b[j] != '\0'; j++) {}
        if(i != j){;return 0;}
    int egyezik = 1;
    for(int i = 0; a[i] != 0; i++)
    {
        if(a[i] != b[i]){return 0;}
    }
    return 1;
}


void mapBeolvas(char *map) //beolvassa a mapot
{
    FILE *f = fopen(map,"r");
    fscanf(f,"%d %d\n",&terkep.sor, &terkep.oszlop); //y és x.
    terkep.map = malloc(sizeof(char*)*terkep.sor);
    vak_map = malloc(sizeof(char*)*terkep.sor); //a térkép amit a játékos addig használ amíg nincs meg a teljes

    for(int i = 0; i < terkep.sor; i++)
    {
        terkep.map[i] = malloc(sizeof(char)*terkep.oszlop);
        vak_map[i] = malloc(sizeof(char)*terkep.oszlop);
    }
    for(int i = 0; i < terkep.sor; i++)
    {

        for(int j = 0; j < terkep.oszlop; j++)
        {
            fscanf(f," %c",&terkep.map[i][j]);
            if(terkep.map[i][j] == 'O') //A főhős helyzete
            {
                terkep.kezdohely[0] = i;
                terkep.kezdohely[1] = j;
                foHos.pos[0] = i; foHos.pos[1] = j;}
                vak_map[i][j] = ' '; //Feltölteni spacerekkel hogy ne csússzon szét
        }
    }
    fscanf(f,"%s\n%s\n%s\n%s\n%s\n%s",next_map,itemLista,enemyLista,aruLista,ajtoLista, kulcsLista); //A map végén felvannak sorolva a szükséges fileok nevei.
    fclose(f);

}
void ajto_kulcsaBeolvas(char *ajtoF,char *kulcsF) 
{


FILE *kulcs = fopen(kulcsF,"r");
FILE *ajto = fopen(ajtoF,"r");
int kulcsDB,ajtoDB = 0;
foHos.kulcs_jelenleg=0;
fscanf(kulcs,"%d\n",&kulcsDB);
fscanf(ajto,"%d\n",&ajtoDB);
kulcsok =malloc(sizeof(Kulcs)*kulcsDB);
ajtok =malloc(sizeof(Ajto)*ajtoDB);
terkep.kulcs_db = kulcsDB;
terkep.ajto_db = ajtoDB;
for(int i = 0; i < ajtoDB; i++) 
{
fscanf(ajto,"%[^\n]s\n",ajtok[i].nev);
fscanf(ajto,"%d %d %d\n",&ajtok[i].pos[0],&ajtok[i].pos[1],&ajtok[i].id);
terkep.map[ajtok[i].pos[0]][ajtok[i].pos[1]] = 'Z';
}    

for(int i = 0; i < kulcsDB; i++) 
{
fscanf(kulcs,"%[^\n]s\n",kulcsok[i].nev);
		char s[999];  
		sprintf(s,"\033[36;1m");
		strcat(s, kulcsok[i].nev);
		strcat(s,"\033[0m");
		strcpy(kulcsok[i].nev, s);
fscanf(kulcs,"%d %d %d\n",&kulcsok[i].pos[0],&kulcsok[i].pos[1],&kulcsok[i].id);
if(kulcsok[i].pos[0] == -1 && kulcsok[i].pos[1] == -1){}else 
{terkep.map[kulcsok[i].pos[0]][kulcsok[i].pos[1]] = 'k';}

}  
fclose(ajto);
fclose(kulcs);

foHos.kulcsok = malloc(sizeof(Kulcs)*kulcsDB);


}
void itemBeolvas(char *lista) //Beolvassa a mapból kapott item listát
{
    if(!Egyezik(lista,"nincs")){
    int db;
    FILE *f = fopen(lista,"r");
    Itemek  item;
    fscanf(f,"%d\n",&db); //hány darab item van
    targyak = malloc(sizeof(Itemek)*db);
    terkep.itemekSzama = db;
    for(int i = 0; i< db; i++)
    {
        
		//char NAME[99];
        fscanf(f,"%[^\n]s ",item.nev);
		char s[999];  
		sprintf(s,"\033[35;3;1m");
		strcat(s, item.nev);
		strcat(s,"\033[0m");
		strcpy(item.nev, s);
        fscanf(f,"%d %d %d %d\n",&item.atk,&item.def,&item.pos[0],&item.pos[1]);
        targyak[i] = item;
        terkep.map[item.pos[0]][item.pos[1]] = 't'; //A térképen megjelenített jelzése
    }
       fclose(f);
}
 
}
void EllenfelBeolvas(char *lista)
{
        if(!Egyezik(lista,"nincs")){
    int db;
    FILE *f = fopen(lista,"r");
    fscanf(f,"%d\n",&db);
    ellensegek = malloc(sizeof(Karakter)*db);
    terkep.ellenfelekSzama = db;

    for(int i = 0; i< db; i++)
    {
		char NAME[99];
        fscanf(f,"%[^\n]s\n",NAME);
		char s[999];  
		sprintf(s,"\033[35;49m");
		strcat(s, NAME);
		strcat(s,"\033[0m");
		strcpy(ellensegek[i].nev, s);
		
		
        char tmp[100];
        fscanf(f,"%d %d %d %d\n",&ellensegek[i].hp,&ellensegek[i].sebzes,&ellensegek[i].pos[0],&ellensegek[i].pos[1]);
    
        ellensegek[i].zsarolva = 0;
       terkep.map[ellensegek[i].pos[0]][ellensegek[i].pos[1]]='E';
     
    }

    fclose(f);
}
}
void arusBeolvas(char *lista)
{
        if(!Egyezik(lista,"nincs")){
    FILE* f = fopen(lista,"r");
    int db;
    fscanf(f,"%d\n",&db);
    terkep.arusokSzama = db;
    arusok = malloc(sizeof(Karakter)*db);
    for(int i = 0; i < db; i++)
    {
        Karakter npc;
        fscanf(f,"%[^\n]s\n",arusok[i].nev);
        fscanf(f,"%d %d %d\n",&arusok[i].pos[0],&arusok[i].pos[1],&arusok[i].aru_db); //Beolvassuk mennyi aruja van
        terkep.map[arusok[i].pos[0]][arusok[i].pos[1]] = 'a';
        if(arusok[i].aru_db > 0){
        arusok[i].keszlet = malloc(sizeof(Aru)*arusok[i].aru_db);
        for(int j = 0; j < arusok[i].aru_db; j++) //Aru darab alapján inditunk egy ciklust. az arus alatt felvannak sorolva a tételei
        {
			
            fscanf(f,"%[^\n]s\n",arusok[i].keszlet[j].nev);
			
		
            fscanf(f,"%d %d %d %d\n",&arusok[i].keszlet[j].seb,&arusok[i].keszlet[j].ved,&arusok[i].keszlet[j].ar,&arusok[i].keszlet[j].db);
            arusok[i].keszlet[j].megveve = 0;
			
			char s[999];  
		sprintf(s,arusok[i].keszlet[j].db==-1?"\033[36;1m":"\033[35;3;1m");
		strcat(s, arusok[i].keszlet[j].nev);
		strcat(s,"\033[0m");
		strcpy(arusok[i].keszlet[j].nev, s);
        }
}

    }
    fclose(f);
}
}
int EllenfelKereso(); //Előre létrehoztam mert más void is használja
int arusKereso() //Ha egy átuson állunk akkor a tömbbeli indexét visszaadja, ha nem akkor -1
{
    for(int i = 0; i < terkep.arusokSzama; i++)
    {
        if(arusok[i].pos[0] == foHos.pos[0] &&arusok[i].pos[1] == foHos.pos[1])
        {
            return i;
            vasarol = 0;
        }
    }
    return -1;
}
char lehetosegek(Karakter npc) //Kiiratjuk az alap promptokat a játékosnak
{  
    printf("Jelenlegi állapotod: ");
    printf(FOHOS);
    printf("%d hp\n",foHos.hp);
    printf(DEFAULT);


    printf("Sebzésed: ");
    printf("\033[31;49m");
    printf("%d dmg\n",foHos.sebzes);	
     printf(DEFAULT);

    printf("Pénzed: ");
    printf("\033[33;49;1m%d¢\033[0m\n",foHos.penz);
    printf(DEFAULT);

    if (EllenfelKereso() != -1)
    {
        if(foHos.harcban) //Ha harcban vagyunk csak akkor iratja ki
        {
            printf("1) Támadás\n");
        }
    }
    if(EllenfelKereso() != -1)
    {
        if(npc.zsarolva == 0 && foHos.harcban == 1) { //Ha harcban vagyunk
            printf("2) %s lefizetése (nálad: \033[33;49m%d¢\033[0m szükséges: \033[33;49;1m%d¢\033[0m)\n",npc.nev,foHos.penz,npc.hp*10);
        }

    }
    if(arusKereso() != -1 && vasarol == 0) //Ha egy aruson állunk és még nem vásárlunk
    {
        printf("2) Megnézed %s portékáját\n",npc.nev);
    }

    if(!foHos.harcban){ //Csak akkor irja ki ha nem vagyunk harcban, ugyanis nem lehet elmenekülni
        printf("Merre akarsz menni? f(el), l(e), b(alra), j(obbra) vagy wasd\n");
    }

    char a; //Visszaadjuk a választott opciot
    scanf(" %c",&a);
    system(CLEAR); //Töröljük a képernyőt
    return a;

}
void Tamad();
void Beszel();
void keszletKiirat();
void Bal();
void Jobb();
void Fel();
void Le();
void Exit();
void Mozgas(char irany);

//Előre létrehozott void-ok tömkelege
void terkepKiirat() //Kiiratjuk a teljes térképet
{

    for(int i = 0; i < terkep.sor; i++)
    {
        for(int j = 0; j < terkep.oszlop; j++)
        {
         
            if(terkep.map[i][j] == 'E') 
            {
                printf(ENEMY);
            }
             if(terkep.map[i][j] == '#') 
            {
             printf(FAL);
            
            }
            if(terkep.map[i][j] == '-') 
            {
                printf(TALAJ);
            }
            if(terkep.map[i][j] == 'O') 
            {
                printf(FOHOS);
            }
             if(terkep.map[i][j] == 't') 
            {
                printf(TARGY);
            }
            if(terkep.map[i][j] == 'p') 
            {
                printf(PENZ);
            }
              if(terkep.map[i][j] == 'a') 
            {
                printf(BOLTOS);
            }
            printf("%c ",terkep.map[i][j]);
            printf(DEFAULT);
     
        }
        printf("\n");
    }
};
void vakTerkepKiirat()
{
    for(int i = 0; i < terkep.sor; i++)
    {
        for(int j = 0; j < terkep.oszlop; j++)
        {
            if(vak_map[i][j] == 'E') 
            {
                printf(ENEMY);
            }
             if(vak_map[i][j] == '#') 
            {
             printf(FAL);
            
            }
            if(vak_map[i][j] == '-') 
            {
                printf(TALAJ);
            }
            if(vak_map[i][j] == 'O') 
            {
                printf(FOHOS);
            }
             if(vak_map[i][j] == 't') 
            {
                printf(TARGY);
            }
            if(vak_map[i][j] == 'p') 
            {
                printf(PENZ);
            }
              if(vak_map[i][j] == 'a') 
            {
                printf(BOLTOS);
            }
			if(vak_map[i][j] == 'k') 
            {
                printf("\033[36m");
            }
            printf("%c ",vak_map[i][j]);
            printf(DEFAULT);
        }
        printf("\n");
    }
}
void vakTerkepKitorlt() //Ahogy haladunk, körülöttünk bal,jobb,fel,le irányba feltérképezzük a mapot és úgy feltölti a vak térképet
{
    int a = foHos.pos[0]; //y
    int b = foHos.pos[1]; //x
    if(a > 0 && a < terkep.sor)
    {
        if( b > 0 && b < terkep.oszlop)
        {
              vak_map[a][b] = terkep.map[a][b];
        }
    }
  
    if(a+1 <terkep.sor+1) {vak_map[a+1][b] = terkep.map[a+1][b];}
    if(a-1 < terkep.sor){vak_map[a-1][b] = terkep.map[a-1][b];}
    if(b+1 < terkep.oszlop+1) {vak_map[a][b+1] = terkep.map[a][b+1];}
    if(b-1 < terkep.oszlop){vak_map[a][b-1] = terkep.map[a][b-1];}

}
char vasarlas() //A vásárlásnál hivom meg, nem volt kötelező külön function neki, visszaadja a választott tétel számát stringben, hogy tudjuk hibakezelni
{
    char b;
    scanf(" %c",&b);
    return b;
}
void Mozgas(char irany) //A 4 irányba mozgunk
{
    if (irany == 'l' || irany == 's') { Le();}
    else if (irany == 'f' || irany == 'w') { Fel(); }
    else if (irany == 'b' || irany == 'a') { Bal(); }
    else if (irany == 'j' || irany == 'd') { Jobb(); }


  //  vakTerkepKitorlt();
}

Ajto ajtoKeres(int a, int b)
{
    for(int i = 0; i < terkep.ajto_db; i++){
    if(ajtok[i].pos[0] == a && ajtok[i].pos[1] == b) 
        {
            return ajtok[i];}
        }
}

int zarva_van(int a, int b) 
{
    for(int i = 0; i < foHos.kulcs_jelenleg; i++) 
    {
        Ajto ajto = ajtoKeres(a,b);
        if(ajto.id == foHos.kulcsok[i].id)
            {
                printf("Felhasználva: %s\nKinyitottad az ajtót.",foHos.kulcsok[i].nev);
                return i;
            }
    }
    return -1;
}
void Valasztas(Karakter npc)
{
    if(!jatekVege && foHos.hp > 0) {
        printf("\nTérkép:\n");
        if (foHos.terkep) { terkepKiirat(); } else { vakTerkepKiirat(); } //Ha van térképünk kiratjuk azt, ha nincs, használjuk a vaktérképet.
        printf("\n");
        char valasz = lehetosegek(npc);
        if (!foHos.harcban) { //HA nem vagyunk harcban
            Mozgas(valasz);//Ha nem adtunk megsemmit, nem megyunk sehova.
           vakTerkepKitorlt(); //töltjük a vak térképet
            if (EllenfelKereso()!= -1) { //mozgás után, ha ellenségre érkezünk
                Valasztas(ellensegek[EllenfelKereso()]); //Az EllenfelKereso visszadaja az ellenfél indexét és az ellenségek tömbből kikeressük
            } else {
                if(valasz == '2') //Ha
                {
                    if(arusKereso() != -1) //Ha egy áruson álunk és a 2-est nyomjuk, nyissa meg a portékáját.
                    {
                        keszletKiirat(arusok[arusKereso()]);

                    }
                }
                else
                {
                    Valasztas(npc); //Ha rossz az input, ismételje ezt a fügvényt.
                }

            }
        }
        //Ekkor már harcban vagyunk
        if(valasz == '1')
        {
            Tamad(npc); //Harcolunk a kiválasztott ellenséggel
        }
        else if(valasz == '2' && arusKereso() == -1 && npc.zsarolva == 0 && EllenfelKereso() != -1)
        {
            Beszel(npc); //Megzsaroljuk
        }
        else { Valasztas(npc); //rossz input esetén ismételjük a függvényt

    }
    }

}
void Exit()
{//Ha elértük a játék végét jelőlő X-et
    
    if(foHos.hp <= 0) 
    { 
            printf("A játék véget ért. Vesztettél.\n");
            //Felszabadítjuk a memóriábol a tömböket.
            //Nincsen free, mert a OS magátol free-el. thank you tho.

    }

    if(terkep.map[foHos.pos[0]][foHos.pos[1]] == 'X')
    {



        if(Egyezik(next_map,"Vége")) //Ha nincs megadva a map alján egy másik térkép file
        {
            printf("Kijutottál!\nGratulálok, nyertél!\n");
            foHos.pos[0] = terkep.kezdohely[0];
            foHos.pos[1] = terkep.kezdohely[1];
            jatekVege = 1;
            //Felszabadítjuk a memóriábol a tömböket.

        }else
        {
            printf("Tovább haladtál a következő pályára.\n"); //továbbmegyünk a kövi pályára
            jatekVege = 0;
                        //Felszabadítjuk a memóriábol a tömböket.
               for(int i = 0; i < terkep.sor; i++)
            {
                free(terkep.map[i]);
                free(vak_map[i]);
            }
            free(vak_map);
            free(terkep.map); 
            free(ellensegek);
            free(targyak);
            free(arusok);
            free(kulcsok);
            free(ajtok);
            free(foHos.kulcsok);
//és mindent újra beolvasunk
            foHos.hp = 100;
            foHos.sebzes = 1;
            foHos.penz = 0;
            mapBeolvas(next_map);
            EllenfelBeolvas(enemyLista);
            itemBeolvas(itemLista);
            EllenfelKereso();
            arusBeolvas(aruLista);
              if(!Egyezik(ajtoLista,"nincs")){   
            ajto_kulcsaBeolvas(ajtoLista,kulcsLista);}
            vakTerkepKitorlt();
            Valasztas(foHos); 
        }


    }
    else {jatekVege= 0;}

}
void keszletKiirat(Karakter arus)
{
    printf("A távozáshoz nyomj egy betűt\n");
    printf("Nálad lévő pénz: \033[33;1m%d¢\033[0m\n",foHos.penz);
	 printf("Életerőd: \033[32;1;3m%d hp\033[0m\n",foHos.hp);
	 printf("Sebzésed: \033[31;1;3m%d dmg\033[0m\n",foHos.sebzes);
    foHos.harcban =0;
    vasarol = 1;
    for(int i = 0; i < arus.aru_db; i++) {
        if (arus.keszlet[i].megveve == 0) {
        
            if(arus.keszlet[i].db == -1) 
            {
            printf("\n%d) Kulcs egy üzenettel: \033[36;1m%s\033[0m\n", i + 1, arus.keszlet[i].nev);
            printf("Ára: \033[33;1m%d¢\033[0m\n", arus.keszlet[i].ar);
            }else {
            printf("\n%d) %s\n", i + 1, arus.keszlet[i].nev);
            if (arus.keszlet[i].seb > 0) {
                printf("+\033[31;1;3m%d Sebzés\033[0m ", arus.keszlet[i].seb);
            }
            if (arus.keszlet[i].seb < 0) {
                printf("\033[31;1;3m%d Sebzés\033[0m ", arus.keszlet[i].seb);
            }
            if(arus.keszlet[i].ved == 0)
            {
                printf("\n");
            }
            if (arus.keszlet[i].ved > 0) {
                printf("+\033[32;1;3m%d Életerő\033[0m\n", arus.keszlet[i].ved);
            }
            if (arus.keszlet[i].ved < 0) {
                printf("\033[32;1;3m%d Életerő\033[0m\n", arus.keszlet[i].ved);
            }
            if(arus.keszlet[i].db<=1)
            {
               printf("Ára: \033[33;1m%d¢\033[0m\n", arus.keszlet[i].ar);
             }else 
             {
             printf("Ára: \033[33;1m%d¢\033[0m\nkészleten: \033[37;1;3m%d db\033[0m\n", arus.keszlet[i].ar,arus.keszlet[i].db);
             }
        }
        }
        else
        {
            printf("\n%d) Megvéve\n", i + 1, arus.keszlet[i].nev);

        }
		printf("-----------------------------------\n");
    }
    char valasz = vasarlas();
    if(valasz < '1' || valasz > '9'){ //Ha nem szamot adunk meg,
        printf("Elköszöntél az árustól");
        vasarol = 0;
        system(CLEAR);

        Valasztas(arus);
    }else
    {

        int melyiket = atoi(&valasz);
        melyiket--;
        int hp = foHos.hp; //felvétel elötti hp
        int sebzes = foHos.sebzes; //felvétel elötti sebzés
        //A megvásárolt item csökkenti vagy növeli a statunkat
        if(arus.keszlet[melyiket].megveve == 1 || arus.keszlet[melyiket].db == 0)
        {
            system(CLEAR);
            printf("Ezt már megvetted.\n");
            keszletKiirat(arus);
        }else {
        if(foHos.penz>=arus.keszlet[melyiket].ar)
        {
            if(arus.keszlet[melyiket].db == -1)
            {
                foHos.kulcsok[foHos.kulcs_jelenleg].id =  arus.keszlet[melyiket].seb;
                sprintf(foHos.kulcsok[foHos.kulcs_jelenleg].nev,arus.keszlet[melyiket].nev);
                foHos.kulcs_jelenleg++;
                arus.keszlet[melyiket].megveve = 1;
                 foHos.penz -= arus.keszlet[melyiket].ar;
            }

            arus.keszlet[melyiket].db--;
            if(arus.keszlet[melyiket].megveve != 1 && arus.keszlet[melyiket].db != -1) {
                    foHos.penz -= arus.keszlet[melyiket].ar;
                    foHos.hp += arus.keszlet[melyiket].ved;
                    foHos.sebzes += arus.keszlet[melyiket].seb;
            if(arus.keszlet[melyiket].db <= 0){
                arus.keszlet[melyiket].megveve = 1;
            }
                system(CLEAR);
                if(foHos.hp > hp)
                {
                    printf("Megnőtt az életerőd \033[32m%d\033[0m->\033[32;1m%d\033[0m\n", hp,foHos.hp);
                }
                if(foHos.hp < hp)
                {
                    printf("Csökkent az életerőd \033[32;1m%d\033[0m->\033[32m%d\033[0m\n", hp,foHos.hp);
                }
                if(foHos.sebzes > sebzes)
                {
                    printf("Megnőtt a sebzésed \033[31m%d\033[0m->\033[31;1m%d\033[0m\n", sebzes,foHos.sebzes);
                }
                if(foHos.sebzes < sebzes)
                {
                    printf("Csökkent a sebzésed \033[31;1m%d\033[0m->\033[31m%d\033[0m\n", sebzes,foHos.sebzes);
                }
                keszletKiirat(arus);

            }
            else {
            system(CLEAR);
            keszletKiirat(arus);
            }
        }else {
            system(CLEAR);
            printf("Nincs rá elég pénzed.\n");
            keszletKiirat(arus);}
        }
    }

}
void Tamad(Karakter npc)
{
    //TODO chaos blade sebezzen, de csak érettségi után. 
    if(!jatekVege && foHos.hp > 0){
    time_t t1;
	int* cim = malloc(sizeof(int));
	
   int array[10]={1,101231,5126,24652456,265,7645,21234,764,2345,42};
    srand(time(NULL)+array[rand()%10]*((int)(&cim)));   // Random seed generálás
    free(cim);
    int r = rand()%10000;
	//printf("R:%d   SEED:%d\n",r,17);
	
   // printf("\033[35;49m%s\033[0m-(a)t megtamadtad.\n",npc.nev);
     printf("Megindultatok egymás felé. \n"); //Harc elötti hp
      printf("%s élete: \033[35;49;1m%d hp\033[0m\n",npc.nev, npc.hp);
    printf("Te életed:\033[32;49m %d hp\033[0m\n\n",foHos.hp);

    if(r <= 1000) //Kivéttük a támadást
        {
            printf("Kivédted \033[35;49m%s\033[0m támadását és megsebezted.\n",npc.nev); 
             npc.hp-= foHos.sebzes;
            
        }

    else if(r > 1000 && r <= 2000)
        {//Kivédték a támadásunkat
            printf("\033[35;49m%s\033[0m kivédte a támadásod és rádsúlytott\n",npc.nev);
              foHos.hp-= npc.sebzes;
        }

    else if(r > 2000 && r <= 2100)
        {
            printf("Kivédtétek egymás támadását\n");
        }
    else 
    {
        if(foHos.sebzes >npc.sebzes) 
        {
         //Mi ütünk először
        printf("Te voltál a gyorsabb és megütötted: \033[35;49m%s\033[0m\n",npc.nev);
        npc.hp-= foHos.sebzes;
        if(npc.hp > 0){
        printf("\033[35;49m%s\033[0m visszavágott.\n",npc.nev);
        //NPC visszaüt
        foHos.hp-= npc.sebzes;
        }
    }else 
    {
         printf("\033[35;49m%s\033[0m megtámadott.\n",npc.nev);
        foHos.hp-= npc.sebzes;
        if(foHos.hp > 0){
        printf("Visszavágtál.\n");
        //NPC visszaüt
         npc.hp-= foHos.sebzes;
        
        } 
    }

    }
     printf("\033[35;49m%s\033[0m élete: \033[35;49;1m%d hp\033[0m\n",npc.nev, npc.hp);
        printf("Te életed:\033[32;49m %d hp\033[0m\n",foHos.hp);
    if(npc.hp <= 0)
    {
        printf("Sikeresen megölted \033[35;49m%s\033[0m-t\n",npc.nev);
        terkep.map[foHos.pos[0]][foHos.pos[1]] = 'O'; //A térképen updatelődik a helyzetünk
        vakTerkepKitorlt();
        if(rand()%100>=20) //20% esély hogy dob pénzt
        {
            int penz = (rand()%50)+1; //eredeti 50
            foHos.penz+=penz;
            printf("\033[35;49m%s\033[0m elejtett: \033[33;49;1m%d¢\033[0m pénzt nálad: \033[33;49m%d¢\033[0m\n",npc.nev,penz,foHos.penz);
          
        }
        Valasztas(npc); // ha megöltük, folytatjuk a játékot
    }
    else if(foHos.hp<= 0)
    {
        printf("Sajnos \033[35;49m%s\033[0m jobb volt nálad és legyőzött.\n",npc.nev);
        //Ha meghaltunk vége a játéknak.
        Exit();
        
    }
    else
    {
        if(!jatekVege){
        Valasztas(npc);
    }
    }
}

}
void Beszel(Karakter npc)
{
    if(foHos.hp>0){
    printf("Megpróbálod lefizetni \033[35;49m%s\033[0m-(a)t (szükséges: %d nalad: \033[33;49m%d¢\033[0m)\n", npc.nev,npc.hp*4,foHos.penz);
    int i = EllenfelKereso();
    if(arusKereso() == -1) { //Ha nem árussal beszélünk.
        if (foHos.penz >= npc.hp*10) {
            printf("Elfogadta a pénzed és innentől békénhagy\n");
            foHos.harcban = 0; //Innentől nem fog megjelenni az ellenség a térképen
            terkep.map[foHos.pos[0]][foHos.pos[1]] = 'O';
            npc.zsarolva = 1;
             vakTerkepKitorlt();
            foHos.penz -= npc.hp*10;
            printf("Jelenlegi pénzmennyiséged: \033[33;49m%d¢\033[0m\n ", foHos.penz);
        } else {
            printf("A(z) \033[35;49m%s\033[0m az arcodba nevetett 'Ilyen nevetséges összeggel ne is próbálkozz' és arcon csapott.\n", npc.nev);
            foHos.hp-= 1;
             printf("\033[35;49m%s\033[0m élete: \033[35;49;1m%d hp\033[0m\n",npc.nev, npc.hp);
             printf("Te életed: \033[32;49m%d hp\033[0m\n",foHos.hp);
            ellensegek[i].zsarolva = 1; //Innentől nem lehet többször megzsarolni
            npc.zsarolva = 1;
        }
        Valasztas(ellensegek[i]);
    }
}
}
int EllenfelKereso()//visszaadja egy ellenfél helyzetét az ellenségek tömbben
{
    for(int i = 0; i < terkep.ellenfelekSzama; i++)
    {
        if(terkep.map[foHos.pos[0]][foHos.pos[1]] == 'E' || terkep.map[foHos.pos[0]][foHos.pos[1]] == 'H')
        {
            if((foHos.pos[0] == ellensegek[i].pos[0] && foHos.pos[1] == ellensegek[i].pos[1]))
            {
                foHos.harcban = 1;
                return i;
            }
        }

    }
    foHos.harcban = 0;
    return -1;
}

Kulcs kulcsKereso()
{
    for(int i = 0; i < terkep.kulcs_db; i++) 
    {
        if(kulcsok[i].pos[0] == foHos.pos[0] && kulcsok[i].pos[1] == foHos.pos[1]) 
        {
           
            foHos.kulcsok[foHos.kulcs_jelenleg] = kulcsok[i];
            foHos.kulcs_jelenleg++;
            return kulcsok[i];
        }
    }
}

void MelyikItem() //visszaadja hogy melyik itemre érkeztünk, illetve ha pénzre érkeztünk.
{
    int hp = foHos.hp;
    int sebzes = foHos.sebzes;
    if(terkep.map[foHos.pos[0]][foHos.pos[1]] =='t')
    {
        for(int i = 0; i < terkep.itemekSzama; i++)
        {
            if(targyak[i].pos[0] == foHos.pos[0] && targyak[i].pos[1] == foHos.pos[1])
            {
                printf("Itemet találtál: %s\n",targyak[i].nev);
                if(Egyezik(targyak[i].nev,"Terkep"))
                {
                    printf("Találtál egy térképet! Mostmár látod az egész pályát!\n");
                    foHos.terkep = 1;
                }
                    foHos.sebzes +=  targyak[i].atk;
                    foHos.hp +=  targyak[i].def;
                    if(foHos.sebzes > sebzes) {printf("Megnőtt a sebzésed: \033[31m%d\033[0m->\033[31;1m%d\033[0m\n",sebzes,foHos.sebzes);}
                    else 
                    {
                        if(sebzes!= foHos.sebzes){ printf("Csökkent a sebzésed: \033[31;1m%d\033[0m->\033[31m%d\033[0m\n",sebzes,foHos.sebzes);}}
                    if(foHos.hp > hp) {printf("Megnőtt az életed: \033[32m%d\033[0m->\033[32;1m%d\033[0m\n",hp,foHos.hp);}
                    else {
                            if(foHos.hp != hp){printf("Csökkent az életed: \033[32;1m%d\033[0m->\033[32m%d\033[0m\n",hp,foHos.hp);}
                        }
                        
            }
        }
    }
    if(terkep.map[foHos.pos[0]][foHos.pos[1]] =='p')
    {
        foHos.penz+=100;
                printf("Felvettél egy pénzeszsákot (\033[33m+100¢\033[0m) nálad: \033[33;1m%d\033[0m\n",foHos.penz);
    }
    if(terkep.map[foHos.pos[0]][foHos.pos[1]] =='k') 
    {
       Kulcs k = kulcsKereso();
        printf("Felvettél egy kulcsot, egy üzenet volt mellékelve hozzá:\n%s\n",k.nev); 

    }

}


void Bal() //A térkép haladunk
{
    printf("Balra fordultál\n");
    Exit();
    if(foHos.pos[1]-1 < 0 || terkep.map[foHos.pos[0]][foHos.pos[1]-1] == '#' || terkep.map[foHos.pos[0]][foHos.pos[1]-1] == 'Z') //Ha elérjük a pály szélét.
    {
        if(terkep.map[foHos.pos[0]][foHos.pos[1]-1] == 'Z') 
        {

                if(zarva_van(foHos.pos[0],foHos.pos[1]-1) != -1)
                    {
                    terkep.map[foHos.pos[0]][foHos.pos[1]-1] = '-';
                    }else
                    {printf("Zárt ajtó, nincs hozzá kulcsod.");}
        }else {
        printf("Falnak mentél.\n");
    }

    }else
    {
        if(EllenfelKereso()== -1) { //ha lelépett a karakter rola
            if(arusKereso() == -1) //és nem arus
            { terkep.map[foHos.pos[0]][foHos.pos[1]] = '-';}
            else
            {
                terkep.map[foHos.pos[0]][foHos.pos[1]] = 'a'; //Az arus megmarad a térképen
            }


        }
        else {
            //Failsafe ha valahogy ellenségen vagyunk
            if(EllenfelKereso()) {
                if(foHos.hp>0){
                printf("Csatába keveredtél: \033[35;49m%s\033[0m\n", ellensegek[EllenfelKereso()].nev);
                terkep.map[foHos.pos[0]][foHos.pos[1]] = 'H';}
            }

        }
        foHos.pos[1]-=1; //Mozgatjuk a karaktert
        Exit(); //Megnézzük hogy Exiten landoltunk-e
        if(EllenfelKereso()== -1){ //ha nem állunk ellenségen
            MelyikItem();  //Megnézzük állunk-e tárgyon
            if(arusKereso() != -1) { //Ha áruson állnuk
                printf("Belefutottál: %s",arusok[arusKereso()].nev);
                terkep.map[foHos.pos[0]][foHos.pos[1]] = 'A'; //Az arussal valo beszélés jele
                vakTerkepKitorlt();
                Valasztas(arusok[arusKereso()]);


            }
            else
            {
                terkep.map[foHos.pos[0]][foHos.pos[1]] = 'O'; //Ha semmin sem, akkor csak mozgunk
            }

        }
        else{
            if(EllenfelKereso()) {
                printf("Csatába keveredtél: \033[35;49m%s\033[0m\n", ellensegek[EllenfelKereso()].nev);
                terkep.map[foHos.pos[0]][foHos.pos[1]] = 'H'; //Harc jele
            }

        }
    }
};
void Jobb()
{
    printf("Jobbra fordultál\n");
    Exit();
    if(foHos.pos[1]+1 > terkep.oszlop-1 || terkep.map[foHos.pos[0]][foHos.pos[1]+1] == '#' || terkep.map[foHos.pos[0]][foHos.pos[1]+1] == 'Z')
    {
        if(terkep.map[foHos.pos[0]][foHos.pos[1]+1] == 'Z') 
        {

                if(zarva_van(foHos.pos[0],foHos.pos[1]+1) != -1)
                    {
                  
                    terkep.map[foHos.pos[0]][foHos.pos[1]+1] = '-';
                    }else
                    {printf("Zárt ajtó, nincs hozzá kulcsod.");}
        }else {
        printf("Falnak mentél.\n");
    }

    }else
    {
        if(EllenfelKereso()== -1) {

            if(arusKereso() == -1)
            {
                terkep.map[foHos.pos[0]][foHos.pos[1]] = '-';
                
            }
            else
            {
                terkep.map[foHos.pos[0]][foHos.pos[1]] = 'a';
            }

        }
        else{
            printf("Csatába keveredtél: \033[35;49m%s\033[0m\n",ellensegek[EllenfelKereso()].nev);
            terkep.map[foHos.pos[0]][foHos.pos[1]] = 'H';
        }
        foHos.pos[1]+=1;
        Exit();
        if(EllenfelKereso() == -1){
            MelyikItem();
            if(arusKereso() != -1) {
                printf("Belefutotál: %s\n", arusok[arusKereso()].nev);
                terkep.map[foHos.pos[0]][foHos.pos[1]] = 'A';
                vakTerkepKitorlt();
                Valasztas(arusok[arusKereso()]);
            }
            else
            {
                terkep.map[foHos.pos[0]][foHos.pos[1]] = 'O';            }
        }
        else
        {
            printf("Csatába keveredtél: \033[35;49m%s\033[0m\n",ellensegek[EllenfelKereso()].nev);
            terkep.map[foHos.pos[0]][foHos.pos[1]] = 'H';
        }
    }
};
void Fel()
{
    printf("Elindultal előre\n");
    if(foHos.pos[0]-1 < 0  || terkep.map[foHos.pos[0]-1][foHos.pos[1]] == '#'|| terkep.map[foHos.pos[0]-1][foHos.pos[1]] == 'Z')
    {

        if(terkep.map[foHos.pos[0]-1][foHos.pos[1]] == 'Z') 
        {

                if(zarva_van(foHos.pos[0]-1,foHos.pos[1]) != -1)
                    {
                    terkep.map[foHos.pos[0]-1][foHos.pos[1]] = '-';
                    }else
                    {printf("Zárt ajtó, nincs hozzá kulcsod.");}
        }else {
        printf("Falnak mentél.\n");
    }

    }else
    {

        if(EllenfelKereso()== -1) {

            if(arusKereso() == -1)
            {
                terkep.map[foHos.pos[0]][foHos.pos[1]] = '-';
            }
            else
            {
                terkep.map[foHos.pos[0]][foHos.pos[1]] = 'a';
            }

        }
        else
        {
            printf("Csatába keveredtél: \033[35;49m%s\033[0m\n",ellensegek[EllenfelKereso()].nev);
            terkep.map[foHos.pos[0]][foHos.pos[1]] = 'H';
        }
        foHos.pos[0]-=1;
         Exit();
         if(!jatekVege){
        if(EllenfelKereso()== -1) {//Ha nem ellenségre érkeztünk
            MelyikItem();
            if(arusKereso() != -1) {
                printf("Belefutttál: %s\n", arusok[arusKereso()].nev);
                terkep.map[foHos.pos[0]][foHos.pos[1]] = 'A';
                vakTerkepKitorlt();
                Valasztas(arusok[arusKereso()]);
            }
            else
            {
                terkep.map[foHos.pos[0]][foHos.pos[1]] = 'O';
            }

        }
        else{
            printf("Csatába keveredtél: \033[35;49m%s\033[0m\n",ellensegek[EllenfelKereso()].nev);
            terkep.map[foHos.pos[0]][foHos.pos[1]] = 'H';

        }


    }
}
}

void Le()
{
    printf("Elindultal visszafelé\n");
    if(foHos.pos[0]+1 > terkep.sor  || terkep.map[foHos.pos[0]+1][foHos.pos[1]] == '#' || terkep.map[foHos.pos[0]+1][foHos.pos[1]] == 'Z')
    {
         if(terkep.map[foHos.pos[0]+1][foHos.pos[1]] == 'Z') 
        {

                if(zarva_van(foHos.pos[0]+1,foHos.pos[1]) != -1)
                    {
                  
                    terkep.map[foHos.pos[0]+1][foHos.pos[1]] = '-';
                    }else 
                    {
printf("Zárt ajtó, nincs hozzá kulcsod.\n");
                       }
        }else {
        printf("Falnak mentél.\n");
    }
    }else
    {
        if(EllenfelKereso()== -1) { //Ha nem ellenségen vagyunk, 

            if(arusKereso() == -1) //Ha nem áruson
            {
                terkep.map[foHos.pos[0]][foHos.pos[1]] = '-';
            }
            else
            {
                terkep.map[foHos.pos[0]][foHos.pos[1]] = 'a';
            }

        }
        else
        {
            printf("Csataba keveredtél: \033[35;49m%s\033[0m\n",ellensegek[EllenfelKereso()].nev);
            terkep.map[foHos.pos[0]][foHos.pos[1]] = 'H';
        }
        foHos.pos[0]+=1;
        Exit();
        if(!jatekVege) {
            if (EllenfelKereso() == -1) { //Ellensegre estunk-e
                MelyikItem();
                if(arusKereso() != -1) {
                    printf("Belefutotál: %s\n", arusok[arusKereso()].nev);
                    terkep.map[foHos.pos[0]][foHos.pos[1]] = 'A';
                vakTerkepKitorlt();
                    Valasztas(arusok[arusKereso()]);
                }
                else
                {
                    terkep.map[foHos.pos[0]][foHos.pos[1]] = 'O';
        
                }

            } else {
                printf("Csatába keveredtél: \033[35;49m%s\033[0m\n", ellensegek[EllenfelKereso()].nev);
                terkep.map[foHos.pos[0]][foHos.pos[1]] = 'H';
            }
        }
    }
};





int main() {
setlocale(LC_ALL, "en_US.UTF-8");
   system(CLEAR);
    foHos.hp = 100;
    foHos.sebzes = 1;
    foHos.pos[0] = 0;
     foHos.pos[1] = 0;
     sprintf(foHos.nev,"IV. Ifjabb Jakabb Kalman Sandor Bernadett");
    foHos.penz = 20;
    foHos.terkep = 0;
    //Létrehozzuk főhősünket
    //Beolvassuk a megadott pályát
   mapBeolvas("map2.leiras");

if(!Egyezik(enemyLista,"nincs")){
   EllenfelBeolvas(enemyLista);}
if(!Egyezik(ajtoLista,"nincs"))
   {   
  ajto_kulcsaBeolvas(ajtoLista,kulcsLista);
}

if(!Egyezik(itemLista,"nincs")){   
  itemBeolvas(itemLista);}

  if(!Egyezik(aruLista,"nincs")){   
    arusBeolvas(aruLista);}

    EllenfelKereso();
    printf("Egyik nap békésen sétálgattál hazafelé. Az idő kellemes volt, a nap sütött és a szél is lágyan fújt.\n"
           "Ám egyszer csak megbotlottál a járdában, mert a mellette lévő fa gyökere felnyomta.\n"
           "Viszont ahelyett, hogy a járdának nekicsapódtál volna, elkezdtél zuhanni, másszóval, átklippeltél a valóságon.\n"
           "Egy sötét, nedves, büdös, hideg, és más hasonlo szinonímákkal ellátott helyen találtad magad.\n"
           "Furcsa módon úgy érezted, hogy ezen a helyen mindenféle fantázia szülötte lények fognak rád várna.\n"
           "Legyenek akár törpök, unikornisok, vagy egy hatalmas mágikus gonosz banán, Te akkor is kifogsz innen jutni!"
           "\n");
           vakTerkepKitorlt();

   Valasztas(foHos); //az npc parameter meg valtozhat
    return 0;
}
