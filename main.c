#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct songs{
    char name[1024];
    struct songs* next;
    struct songs* prev;
}song;

char buf[2048] = "ls -1 /home/soi/Music/song/*.mp3 > /tmp/songs.txt";
song* head = NULL;
song* now = NULL;
int isstarting = 0;
int isplaying = 0;

//初始化播放器
void init(){
    system(buf);
    song* p;
    FILE* list = fopen("/tmp/songs.txt","r");
    while(1){
        if(fgets(buf,sizeof buf,list) == NULL)
            break;
        p = (song*)malloc(sizeof(song));
        strcpy(p->name,buf);
        p->name[strlen(p->name)-1] = 0x20;     //去除歌名中的回车
        p->next = NULL;
        p->prev = now;
        now = p;
        if(head == NULL){
            head = p;
        }
        else {
            p->prev->next = p;
        }
    }
    p = NULL;
    head->prev = now;
    now->next = head;
    now = head;
    fclose(list);
}

//启停播放
void suspend(){
    if(isplaying == 1 && isstarting == 1){
        system("killall -STOP madplay");
        isplaying = 0;
    }
    else if(isplaying == 0 && isstarting == 1){
        system("killall -CONT madplay");
        isplaying = 1;
    }
}

//开始播放
void start(song* music){
    if(isstarting == 0){
        song* nowbak = music->prev;
        FILE* musiclist = fopen("/tmp/songlist.txt","w+");
        do{
            musiclist = fopen("/tmp/songlist.txt","a+");
            fputs(nowbak->name,musiclist);
            fclose(musiclist);
            nowbak = nowbak->next;
        }while(nowbak != music->prev);
        nowbak = NULL;
        musiclist = fopen("/tmp/songlist.txt","r");
        fgets(buf,sizeof buf,musiclist);
        sprintf(buf,"madplay %s < /dev/null > /dev/null 2>1 -r &",buf);    //拼接命令，将输入输出重定向到回收站，否则会出错
        printf("上一曲 %s\n",music->prev->name);
        printf("正在播放 %s\n",music->name);
        printf("下一曲 %s\n",music->next->name);
        system(buf);
        isstarting = 1;
        isplaying = 1;
    }
    else
        suspend();
}

//停止播放
void stop(){
    if(isstarting == 1){
        system("killall -9 madplay");
        isstarting = 0;
    }
}

//上一曲
void prev(){
    stop();
    now = now->prev;
    start(now);
}

//下一曲
void next(){
    stop();
    now = now->next;
    start(now);
}

//退出程序
void exitmusic(){
    if(isstarting == 1){
        system("killall -9 madplay");        
    }
    do{
        free(head);
        head = head->next;
    }while(head = NULL);
    exit(0);
}

int main()
{
    int cmd;
    init();
    while(1){
        if(isstarting == 0){
            printf("请输入指令： 1 开始播放   2 上一曲   3 下一曲   0 退出\n");
        }
        else if(isplaying == 1){
            printf("请输入指令： 1 暂停播放   2 上一曲   3 下一曲   4 停止播放   0 退出\n");
        }
        else{
            printf("请输入指令： 1 继续播放   2 上一曲   3 下一曲   4 停止播放   0 退出\n");
        }
        scanf("%d",&cmd);
        switch (cmd) {
        case 0:
            exitmusic();
            break;
        case 1:
            start(now);
            break;
        case 2:
            prev();
            break;
        case 3:
            next();
            break;
        case 4:
            stop();
            break;
        default:
            break;
        }
    }
    return 0;
}
