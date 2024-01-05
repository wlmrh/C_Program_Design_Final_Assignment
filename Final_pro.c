#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>
//#define PRINT_BLUE(fmt,args...)   ({printf("\033[0;32;34m info: %s(%d) %s: ",__FILE__, __LINE__, __func__);printf(fmt"\r\n" ,##args);})
char folderPath[50];
char doc[11][20];//文件名列表,从1开始记录
void solve();
void doc_list_fill();
void service();
Node *tree();
char *fgetsentence(char *rst, int *para, int *sent, int no, int num);
void extend(char *word, Node *srt, int no, int para, int sent, int pos, FILE *current);
void addnode(char alpha, Node *pre);
int doc_num = 0;//储存文件夹下文件的数量
int paragraph_word[11][20];//记录每一段的词数
FILE *fp, *crtfp;//crtfp记录当前处理单词所在句子开头的位置
Node *trie;

typedef struct information{
  int no, par, pos;//记录出现文件编号，段落，第几个单词
  struct information *next;
  FILE *fp;//记录本句话开始的位置
}inf;

typedef struct node{
  char letter;
  int fre;
  struct node *next[27];
  inf *information;
}Node;

int main()
{
  scanf("%s", folderPath);//获取文件夹路径
  solve();//完成段落数据处理
  service();
}

void solve()
{
  doc_list_fill();//填充文件列表
  trie = tree();//对所有文件中的单词进行处理，获取一个词典树
}

void doc_list_fill()
{
    WIN32_FIND_DATAA findData;
    HANDLE hFind;
    int pos = 1;
    // 构造搜索路径
    char searchPath[50]; strcpy(searchPath, folderPath);
    strcat(searchPath, "\\*");
    // 开始查找第一个文件
    hFind = FindFirstFileA(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("无法打开文件夹: %s\n", folderPath);
        return;
    }

    do {
        // 忽略当前目录（.）和上级目录（..）
        doc_num++;
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) continue;
        strcpy(doc[pos++], findData.cFileName);
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
}

Node *tree()//获取一个词典树，并完成相应的information的创建
{
  Node *srt = (Node*)malloc(sizeof(Node)); int no = 1;
  for(no = 1; no <= doc_num; no++)//遍历每一个文件
  {
    char search_path[50]; strcat(search_path, folderPath); strcat(search_path, "\\"); strcat(search_path, doc[no]);
    if((fp = fopen(search_path, "r")) == NULL)
    {
      printf("error");
      exit(0);
    }
    int para = 0, sent = 0, num = 0; char sentence[100];
    while (fgetsentence(sentence, &para, &sent, no, num) != NULL) {//尝试获取一句文字,并维护每一句话的段落位置
          char* word = strtok(sentence, " \t\r\n");//分离出一个单词，但是多个空格的非正常文本会出现问题 
          while (word != NULL) {//截取到新单词word，将其变为小写并放入词典树并补充相应的information
            num++;
            word = tolower(word);
            extend(word, srt, no, para, sent, num, crtfp);//将单词的内容用来扩充
            word = strtok(sentence, " \t\r\n ,.");
          }
    }
    paragraph_word[no][0] = para;
  }
}

char *fgetsentence(char *rst, int *para, int *sent, int no, int num)
{
  char chr;
  while(1)//找到句子的开头，存储在crtfp中
  {
    crtfp = fp; chr = fgetc(fp);
    if(chr == '\n') {paragraph_word[no][*para] = num; *para++;}
    if(chr == EOF) return NULL;
    if(isalpha(chr)) break;
  }
  int pos = 0;
  while(1)//获取句子
  {
    rst[pos++] = chr;
    chr = fgetc(fp);
    if(chr == '.' || chr == '!') {rst[pos++] = chr; break;}
    if(chr == EOF) break;
  }
  rst[pos] = '\0';
  *sent++;
  return rst;
}

void extend(char *word, Node *srt, int no, int para, int sent, int pos, FILE *current)
{
  Node *crt = srt; int len = strlen(word);
  for(int i = 0; i < len; i++)//扩充单词树
  {
    if(crt->next[word[i] - 'a' + 1] == NULL)
    {
      addnode(word[i], crt);
      crt = crt->next[word[i] - 'a' + 1];
    }
    else
    {
      crt = crt->next[word[i] - 'a' + 1];
    }
    if(i == len - 1) crt->fre++;
  }
  inf *ptr = (inf*)malloc(sizeof(inf)), *crtptr = crt->information;
  ptr->no = no; ptr->par = para; ptr->pos = pos; ptr->fp = current; ptr->next = NULL;
  if(crtptr == NULL)
  {
    crt->information = ptr;
  }
  else
  {
    while(crtptr->next != NULL)
    {
      crtptr = crtptr->next;
    }
    crtptr->next = ptr;
  }
}

void addnode(char alpha, Node *pre)
{
  Node *newnode = (Node *)malloc(sizeof(Node));
  newnode->letter = alpha; newnode->fre = 0; memset(newnode->next, NULL, 27 * sizeof(Node*));
  pre->next[alpha - 'a' + 1] = newnode;
}

void service()
{
  while(1)
  {
    printf("请选择要进行的操作: (1 / 2)\n");
    printf("1. 搜索单词\n");
    printf("2. 统计单词在指定段落出现次数\n");
    printf("3. 退出程序\n");
    int choice;
    scanf("%d", &choice);
    if(choice == 1)
    {
      char word[30];
      printf("请输入需要查找的单词:  ");
      scanf("%s", word);
      printf("\n");
      Node *crt = trie; int len = strlen(word);
      int i;
      for(i = 0; i < len; i++)//找到结果所在的结构体
      {
          if(crt->next[word[i] - 'a' + 1] != NULL) crt = crt->next[word[i] - 'a' + 1];
          else {printf("您要查找的单词不存在。\n"); break;}
      }
      if(i != len) continue;
      inf *rst = crt->information; int num;
      while(rst != NULL)
      {
        num++;
        printf("%s\t第%d段\t第%d个单词\n");
        FILE *fp2 = rst->fp;
        char sentence[100]; fgets(sentence, 100, fp2);
        puts(sentence);
        rst = rst->next;
      }
      printf("匹配项共%d个\n", num);
    }
    else if(choice == 2)
    {
      printf("请输入需要查找的文件名称：  ");
      char file[30];
      scanf("%s", file); printf("\n");
      int doc_no, i;
      for(i = 1; i <= doc_num; i++)
      {
        if(strcmp(doc[i], file) == 0) doc_num = i;
      }
      if(i == doc_num + 1) {printf("您输入的文件名有误！\n"); continue;}
      printf("请输入需要查找的段落号：  ");
      int para; scanf("%d", &para); printf("\n");
      if(para <= 0 || para > paragraph_word[doc_num][0]) {printf("您输入的段落号有误！\n"); continue;}
      printf("您要查找的段落中的单词数为：%d", paragraph_word[doc_num][para]);
    }
    else if(choice == 3) {printf("感谢您的使用！"); return;}
  }
}