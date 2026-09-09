#include <nds.h>
#include <stdio.h>
#include <stdbool.h>

#define W 256
#define H 192
#define TILE 16
#define MW 16
#define MH 11
#define INV_MAX 12

enum { GRASS, WATER, SOIL, TREE, ROCK, PATH, HOUSE, SHOP };
enum { EMPTY, SEED, GROWING, READY };

static u16 *fb;
static int px=7, py=8, energy=100, day=1, money=80;
static int seeds=8, selected=0, area=0, menu=0, message_timer=0;
static u8 crop[MH][MW], wet[MH][MW];

static const u8 farm[MH][MW]={
{WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER},
{WATER,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,WATER},
{WATER,GRASS,TREE,GRASS,GRASS,GRASS,GRASS,SOIL,SOIL,GRASS,GRASS,ROCK,GRASS,GRASS,GRASS,WATER},
{WATER,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,SOIL,SOIL,GRASS,GRASS,GRASS,GRASS,TREE,GRASS,WATER},
{WATER,GRASS,GRASS,ROCK,GRASS,GRASS,GRASS,SOIL,SOIL,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,WATER},
{WATER,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,ROCK,GRASS,GRASS,WATER},
{WATER,GRASS,GRASS,GRASS,TREE,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,WATER},
{WATER,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,TREE,GRASS,WATER},
{WATER,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,WATER},
{WATER,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,WATER},
{WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER}
};

static const u8 village[MH][MW]={
{WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER},
{WATER,GRASS,GRASS,GRASS,GRASS,PATH,PATH,PATH,PATH,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,WATER},
{WATER,GRASS,HOUSE,HOUSE,GRASS,PATH,GRASS,PATH,GRASS,GRASS,SHOP,SHOP,GRASS,GRASS,GRASS,WATER},
{WATER,GRASS,HOUSE,HOUSE,GRASS,PATH,GRASS,PATH,GRASS,GRASS,SHOP,SHOP,GRASS,GRASS,GRASS,WATER},
{WATER,GRASS,GRASS,GRASS,GRASS,PATH,GRASS,PATH,GRASS,GRASS,GRASS,GRASS,GRASS,TREE,GRASS,WATER},
{WATER,GRASS,GRASS,GRASS,GRASS,PATH,PATH,PATH,PATH,GRASS,GRASS,ROCK,GRASS,GRASS,GRASS,WATER},
{WATER,GRASS,GRASS,GRASS,TREE,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,WATER},
{WATER,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,TREE,GRASS,WATER},
{WATER,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,WATER},
{WATER,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,GRASS,WATER},
{WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER,WATER}
};

static void rect(int x,int y,int w,int h,u16 c){
    for(int yy=y;yy<y+h;yy++) for(int xx=x;xx<x+w;xx++)
        if((unsigned)xx<W&&(unsigned)yy<H) fb[yy*W+xx]=c;
}

static void tile(int x,int y,int t){
    int X=x*TILE,Y=y*TILE; u16 c=RGB15(30,150,55);
    if(t==WATER)c=RGB15(20,100,190);
    if(t==SOIL)c=RGB15(145,90,45);
    if(t==PATH)c=RGB15(185,155,95);
    if(t==HOUSE||t==SHOP)c=RGB15(170,75,55);
    if(t==TREE)c=RGB15(30,150,55);
    if(t==ROCK)c=RGB15(90,90,95);
    rect(X,Y,TILE,TILE,c);
    if(t==WATER){rect(X+2,Y+5,8,2,RGB15(35,145,220));rect(X+7,Y+11,7,2,RGB15(35,145,220));}
    if(t==TREE){rect(X+6,Y+9,5,7,RGB15(120,75,35));rect(X+2,Y+2,12,9,RGB15(15,105,45));rect(X+5,Y+1,7,7,RGB15(30,180,60));}
    if(t==ROCK){rect(X+3,Y+5,10,8,RGB15(120,120,125));rect(X+6,Y+3,5,4,RGB15(155,155,160));}
    if(t==HOUSE||t==SHOP){rect(X+5,Y+8,6,8,RGB15(90,55,35));rect(X+2,Y+2,12,5,RGB15(210,50,45));}
    if(t==PATH){rect(X+3,Y+3,2,2,RGB15(210,180,115));rect(X+11,Y+10,2,2,RGB15(210,180,115));}
}

static void draw_crop(int x,int y){
    if(!crop[y][x])return; int X=x*TILE,Y=y*TILE;
    if(crop[y][x]==SEED)rect(X+6,Y+9,4,3,RGB15(70,45,20));
    else if(crop[y][x]==GROWING){
        rect(X+7,Y+6,2,8,RGB15(20,120,40));rect(X+4,Y+7,4,3,RGB15(35,165,50));rect(X+9,Y+5,4,3,RGB15(35,165,50));
    } else {
        rect(X+7,Y+3,3,11,RGB15(20,120,40));rect(X+3,Y+5,5,5,RGB15(35,175,55));rect(X+10,Y+4,4,5,RGB15(35,175,55));rect(X+5,Y+1,7,5,RGB15(235,190,35));
    }
    if(wet[y][x])rect(X+1,Y+13,3,2,RGB15(35,140,220));
}

static void player(void){
    int X=px*TILE,Y=py*TILE;
    rect(X+5,Y+5,7,8,RGB15(245,190,145));rect(X+4,Y+3,9,4,RGB15(90,50,25));
    rect(X+4,Y+12,3,3,RGB15(40,60,130));rect(X+10,Y+12,3,3,RGB15(40,60,130));
}

static int blocked(int x,int y){
    if(x<1||x>=MW-1||y<1||y>=MH-1)return 1;
    int t=(area==0?farm[y][x]:village[y][x]);
    return t==WATER||t==TREE||t==ROCK||t==HOUSE||t==SHOP;
}

static void hud(void){
    consoleClear();
    iprintf("\x1b[1;1HDS FARM V6");
    iprintf("\x1b[2;1HArea: %s  Dia: %d",area?"Vila":"Fazenda",day);
    iprintf("\x1b[3;1H$%d  Sementes: %d  Energia: %d",money,seeds,energy);
    iprintf("\x1b[5;1H[A] Acao  [B] Colher");
    iprintf("\x1b[6;1H[Y] Ferramenta  [X] Inventario");
    iprintf("\x1b[7;1H[L] Vila/Fazenda  [R] NPC");
    iprintf("\x1b[8;1H[START] Dormir/Salvar");
    iprintf("\x1b[10;1HFerramenta: %s",selected?"Regador":"Enxada");
    if(menu==1){
        iprintf("\x1b[12;1HINVENTARIO");
        iprintf("\x1b[13;1HSementes: %d",seeds);
        iprintf("\x1b[14;1HColheitas guardadas: %d",money/25);
        iprintf("\x1b[15;1HDinheiro: $%d",money);
        iprintf("\x1b[17;1HB fecha");
    }
    if(menu==2){
        iprintf("\x1b[12;1HNOVO NPC");
        iprintf("\x1b[13;1H" "Oi! Sou o morador da vila.");
        iprintf("\x1b[14;1H" "A fazenda parece prosperar!");
        iprintf("\x1b[16;1H" "B fecha");
    }
}

static void save_game(void){
    FILE *f=fopen("dsfarm.sav","wb");
    if(!f)return;
    fwrite(&day,sizeof(day),1,f); fwrite(&money,sizeof(money),1,f);
    fwrite(&seeds,sizeof(seeds),1,f); fwrite(crop,sizeof(crop),1,f); fwrite(wet,sizeof(wet),1,f);
    fclose(f);
}

static void load_game(void){
    FILE *f=fopen("dsfarm.sav","rb");
    if(!f)return;
    fread(&day,sizeof(day),1,f); fread(&money,sizeof(money),1,f);
    fread(&seeds,sizeof(seeds),1,f); fread(crop,sizeof(crop),1,f); fread(wet,sizeof(wet),1,f);
    fclose(f);
}

static void new_day(void){
    day++; energy=100;
    for(int y=0;y<MH;y++)for(int x=0;x<MW;x++){
        wet[y][x]=0;
        if(crop[y][x]==SEED)crop[y][x]=GROWING;
        else if(crop[y][x]==GROWING)crop[y][x]=READY;
    }
    save_game();
}

static void action(void){
    if(area!=0||menu)return;
    if(energy<=0)return;
    int t=farm[py][px];
    if(t==SOIL){
        if(selected==0 && crop[py][px]==EMPTY && seeds>0){crop[py][px]=SEED;seeds--;energy-=3;}
        else if(selected==1 && crop[py][px]!=EMPTY){wet[py][px]=1;energy-=2;}
    }
}

static void harvest(void){
    if(area!=0||menu)return;
    if(crop[py][px]==READY && energy>0){
        crop[py][px]=EMPTY;wet[py][px]=0;money+=25;energy-=2;
    }
}

static void update(void){
    scanKeys();u32 k=keysDown();
    if(menu){
        if(k&KEY_B)menu=0;
        return;
    }
    if(k&KEY_LEFT){if(!blocked(px-1,py))px--;}
    if(k&KEY_RIGHT){if(!blocked(px+1,py))px++;}
    if(k&KEY_UP){if(!blocked(px,py-1))py--;}
    if(k&KEY_DOWN){if(!blocked(px,py+1))py++;}
    if(k&KEY_A)action();
    if(k&KEY_B)harvest();
    if(k&KEY_Y)selected=!selected;
    if(k&KEY_X)menu=1;
    if(k&KEY_R)menu=2;
    if(k&KEY_L){area=!area;px=7;py=8;}
    if(k&KEY_START)new_day();
}

int main(void){
    videoSetMode(MODE_FB0);vramSetBankA(VRAM_A_LCD);fb=(u16*)VRAM_A;
    videoSetModeSub(MODE_0_2D);vramSetBankC(VRAM_C_SUB_BG);consoleDemoInit();
    load_game();
    while(1){
        update();
        for(int y=0;y<MH;y++)for(int x=0;x<MW;x++)tile(x,y,area? village[y][x]:farm[y][x]);
        if(!area)for(int y=0;y<MH;y++)for(int x=0;x<MW;x++)draw_crop(x,y);
        player();
        rect(4,178,100,8,RGB15(40,40,40));rect(6,180,energy,4,RGB15(30,190,70));
        hud();
        swiWaitForVBlank();
    }
    return 0;
}
