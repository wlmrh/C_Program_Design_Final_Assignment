#include <stdio.h>
#include <string.h>
#include <windows.h>
char folderPath[50];
char doc[11][20];//文件名列表
void solve();
typedef struct node{
  char letter;
  int fre;
  int no[105], par[105], pos[105];//记录出现文件编号，段落，第几个单词
  struct node *next[27];
}Node;

int main()
{
  scanf("%s", folderPath);
  solve();//完成数据处理
  
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
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) continue;
        strcpy(doc[pos++], findData.cFileName);
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
}

void solve()
{
  //填充文件列表
  doc_list_fill();
}
