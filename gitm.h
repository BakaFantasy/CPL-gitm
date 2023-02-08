#ifndef GITM_H_
#define GITM_H_

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_SIZE 10007
#define DIR_MOD 0777
#define SHA_LENGTH 9

unsigned memo_head[MAX_SIZE], memo_dst[MAX_SIZE], idx[MAX_SIZE][3], key_ancestor;
int cnt_memo_head, cnt_memo_dst, cnt_idx;

void GitmVersion(void);
void GitmInit(void);

unsigned GitmCommit(void);
void ReadIndex(void);
void BackUp(void);
void CallCommand(const char *cmd);
unsigned GetHead(void);
void UpdateHead(const char key[SHA_LENGTH]);

void GitmCheckout(const char dst[SHA_LENGTH]);
bool IsWorktreeClean(void);
void ChangeVersion(unsigned dst);
void ScrollBackward(int cnt_head);
void ScrollForward(int cnt_dst);

int GitmMerge(const char dst[SHA_LENGTH]);
int FindAncestor(unsigned key_head, unsigned key_dst, unsigned key_root);

#endif // GITM_H_