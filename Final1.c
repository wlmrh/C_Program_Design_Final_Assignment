#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>
#define PRINT_BLUE(fmt,args...)   ({printf("\033[0;32;34m info: %s(%d) %s: ",__FILE__, __LINE__, __func__);printf(fmt"\r\n" ,##args);})
char folderPath[50];
char doc[11][20];//文件名列表,从1开始记录
void solve();
void doc_list_fill();
int doc_num = 0;//储存文件夹下文件的数量
int paragraph_word[11][20];//记录每一段的词数
FILE *fp;

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
  scanf("%s", folderPath);
  solve();//完成段落数据处理
}

void solve()
{
  doc_list_fill();//填充文件列表
  Node *trie = tree();//获取一个词典树
}

void node_initialize(Node *node)
{
  node->letter = '\0';
  node->fre = 0;
  memset(node->next, NULL, sizeof(node->next));
  node->information = NULL;
}

Node *tree()//获取一个词典树
{
  Node *srt = (Node*)malloc(sizeof(Node)); int no = 1;
  for(no = 1; no <= doc_num; no++)//遍历每一个文件
  {
    char search_path[50]; strcat(search_path, folderPath); strcat(search_path, "\\"); strcat(search_path, doc[no]);//合成一个文件路径
    if((fp = fopen(search_path, "r")) == NULL)
    {
      printf("error");
      exit(0);
    }
    while(!feof(fp))
    {
      char crt_word[20]; getword(crt_word, 20);//提取出一个单词
      int len = strlen(crt_word); Node *crt = srt;
      for(int i = 0; i < len; i++)//扩充单词树
      {
        if(crt->next[crt_word[i] - 'a' + 1] == NULL)
        {
          addnode(crt_word[i], crt);
          crt = crt->next[crt_word[i] - 'a' + 1];
        }
        else
        {
          crt = crt->next[crt_word[i] - 'a' + 1];
        }
        if(i == len - 1) crt->fre++;
      }
    }
  }
}

void addnode(char alpha, Node *pre)
{
  Node *newnode = (Node *)malloc(sizeof(Node));
  node_initialize(newnode); newnode->letter = alpha;
  pre->next[alpha - 'a' + 1] = newnode;
}

void getword(char *rst, int len)
{
  int pos = 0; char pre_char = '\0'; int flag = 0;
  while(1)
  {
    char chr = fgetc(fp);
    if((isalpha(pre_char) || pre_char == '\'') && isalpha(chr) || chr == '\'')
    {
      flag = 1;
      if(pos == 0)
      {
        rst[pos++] = pre_char; rst[pos++] = chr;
      }
      else rst[pos++] = chr;
      pre_char = chr;
    }
    else
    {
      if(flag == 1)
      {
        return;
      }
    }
  }
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
