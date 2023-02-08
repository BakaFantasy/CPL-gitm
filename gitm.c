#include "gitm.h"

int main(int argc, char *argv[]) {
    if (argv[1]) {
        if (!strcmp(argv[1], "version")) {

            GitmVersion();

        } else if (!strcmp(argv[1], "init")) {

            GitmInit();

        } else if (!strcmp(argv[1], "commit")) {

            BackUp();
            GitmCommit();

        } else if (!strcmp(argv[1], "checkout")) {

            if (argv[2] == NULL)
                return 1;
            if (!strcmp(argv[2], ".")) {
                CallCommand("diff -Nurx .gitm . .gitm/current >.gitm/patch_reset"
                            "patch -p1 <.gitm/patch_reset >/dev/null");
                remove(".gitm/patch_reset");
            } else if (IsWorktreeClean())
                GitmCheckout(argv[2]);
            else
                return 1;

        } else if (!strcmp(argv[1], "merge")) {

            if (argv[2] == NULL)
                return 1;
            if (!IsWorktreeClean() || GitmMerge(argv[2]))
                return 1;

        } else
            return -1;
    }

    return 0;
}

void GitmVersion(void) {
    fprintf(stderr, "%s",
#include "icon.txt"
    );
}

void GitmInit(void) {
    if (mkdir(".gitm", DIR_MOD))
        perror("failed to make .gitm");
    if (mkdir(".gitm/objects", DIR_MOD))
        perror("failed to make objects");
    if (mkdir(".gitm/current", DIR_MOD))
        perror("failed to make current");
}

unsigned GitmCommit(void) {
    // generate SHA-1 key
    char sha1[SHA_LENGTH];
    CallCommand("sha1sum .gitm/objects/new_blob/patch");
    FILE *fp_sha1 = popen("sha1sum .gitm/objects/new_blob/patch", "r");
    if (!fscanf(fp_sha1, "%8s", sha1))
        perror("failed to get SHA-1 key");
    // when nothing changes
    if (!strcmp(sha1, "da39a3ee")) {
        fp_sha1 = popen("date --rfc-3339='ns' | sha1sum", "r");
        if (!fscanf(fp_sha1, "%8s", sha1))
            perror("failed to get SHA-1 key");
    }
    pclose(fp_sha1);
    printf("%s\n", sha1);
    // rename the blob directory
    char path_blob[50] = ".gitm/objects/";
    strcat(path_blob, sha1);
    rename(".gitm/objects/new_blob", path_blob);

    // add commit to the index
    FILE *fp_index = fopen(".gitm/objects/index", "a");
    fprintf(fp_index, "%s ", sha1);
    FILE *fp_head = fopen(".gitm/HEAD", "r");
    if (fp_head != NULL) {
        char key_head[SHA_LENGTH];
        if (!fscanf(fp_head, "%s", key_head))
            perror("failed to get key in HEAD");
        fclose(fp_head);
        fprintf(fp_index, "%s\n", key_head);
    } else {
        fprintf(fp_index, "00000000\n");
    }
    if (fp_index != NULL)
        fclose(fp_index);

    UpdateHead(sha1);
    return (unsigned int)strtoul(sha1, NULL, 16);
}

void ReadIndex(void) {
    FILE *fp_idx = fopen(".gitm/objects/index", "r");
    while (fscanf(fp_idx, "%x%x", &idx[cnt_idx][0], &idx[cnt_idx][1]) != EOF)
        cnt_idx++;
    if (fp_idx != NULL)
        fclose(fp_idx);
}

void BackUp(void) {
    // save the difference between the file in worktree with counterpart
    if (mkdir(".gitm/objects/new_blob", DIR_MOD))
        perror("failed to make new_blob");
    CallCommand("diff -Nurx .gitm .gitm/current . >.gitm/objects/new_blob/patch; "
                "patch -d .gitm/current -p0 <.gitm/objects/new_blob/patch >/dev/null");
}

void CallCommand(const char *cmd) {
    FILE *fp_cmd = popen(cmd, "r");
    if (fp_cmd != NULL)
        pclose(fp_cmd);
    else
        perror("failed to call command");
}

unsigned GetHead(void) {
    unsigned key_head;
    FILE *fp_head = fopen(".gitm/HEAD", "r");
    if (!fscanf(fp_head, "%x", &key_head))
        perror("failed to get key_head");
    if (fp_head != NULL)
        fclose(fp_head);
    return key_head;
}

void UpdateHead(const char key[SHA_LENGTH]) {
    FILE *fp_head = fopen(".gitm/HEAD", "w");
    fprintf(fp_head, "%s", key);
    if (fp_head != NULL)
        fclose(fp_head);
}

void GitmCheckout(const char dst[SHA_LENGTH]) {
    unsigned dst_u = (unsigned int)strtoul(dst, NULL, 16);
    ChangeVersion(dst_u);
    UpdateHead(dst);
    CallCommand("diff -Nurx .gitm .gitm/current . >.gitm/objects/patch_checkout; "
                "patch -d .gitm/current -p1 <.gitm/objects/patch_checkout >/dev/null");
    remove(".gitm/objects/patch_checkout");
}

void ChangeVersion(unsigned dst) {
    memset(idx, 0, MAX_SIZE);
    ReadIndex();
    unsigned key_blob, key_parent = 0, key_head = GetHead();
    cnt_memo_head = 0, cnt_memo_dst = 0;
    // go backward from HEAD
    key_blob = key_head;
    while (key_blob != 0) {
        if (key_blob == dst) {
            ScrollBackward( cnt_memo_head);
            return;
        }
        memo_head[cnt_memo_head++] = key_blob;
        for (int i = 0; i < cnt_idx; ++i) {
            if (idx[i][0] == key_blob) {
                key_parent = idx[i][1];
                break;
            }
        }
        key_blob = key_parent;
    }
    // go backward from dst
    key_blob = dst;
    while (key_blob != 0) {
        for (int i = 0; i < cnt_memo_head; ++i) {
            if (key_blob == memo_head[i]) {
                ScrollBackward(i);
                ScrollForward( cnt_memo_dst);
                return;
            }
        }
        memo_dst[cnt_memo_dst++] = key_blob;
        for (int i = 0; i < cnt_idx; ++i) {
            if (idx[i][0] == key_blob) {
                key_parent = idx[i][1];
                break;
            }
        }
        key_blob = key_parent;
    }
    perror("commit not found");
}

void ScrollBackward(int cnt_head) {
    char cmd_patch[60] = "patch -Rp1 <.gitm/objects/12345678/patch >/dev/null";
    for (int i = 0; i < cnt_head; ++i) {
        char memo_str[SHA_LENGTH];
        sprintf(memo_str, "%.8x", memo_head[i]);
        memcpy(cmd_patch + 26, memo_str, 8);
        CallCommand(cmd_patch);
//        printf("scrolled back from %s\n", memo_head[i]); // DEBUG
    }
}

void ScrollForward(int cnt_dst) {
    char cmd_patch[60] = "patch -p1 <.gitm/objects/12345678/patch >/dev/null";
    for (int i = cnt_dst - 1; i >= 0; --i) {
        char memo_str[SHA_LENGTH];
        sprintf(memo_str, "%.8x", memo_dst[i]);
        memcpy(cmd_patch + 25, memo_str, 8);
        CallCommand(cmd_patch);
//        printf("scrolled forth to %s\n", memo_dst[i]); // DEBUG
    }
}

bool IsWorktreeClean(void) {
    FILE *fp_diff = popen("diff -Nurx .gitm .gitm/current . >/dev/null; echo $?", "r");
    int chk;
    if (!fscanf(fp_diff, "%1d", &chk))
        perror("failed to judge if worktree is clean");
    if (fp_diff != NULL)
        pclose(fp_diff);
    else
        perror("failed to diff");
    if (chk)
        printf("You've made change. Please commit or garbage your change.\n");
    return !chk;
}

int GitmMerge(const char dst[SHA_LENGTH]) {
    // back up HEAD
    if (mkdir(".gitm/backup_HEAD", DIR_MOD))
        perror("failed to create dir");
    CallCommand("diff -Nurx .gitm .gitm/backup_HEAD . >.gitm/patch_HEAD; "
                "patch -d .gitm/backup_HEAD -p0 <.gitm/patch_HEAD >/dev/null");
    remove(".gitm/patch_HEAD");

    // get ancestor
    memset(idx, 0, MAX_SIZE);
    ReadIndex();
    unsigned key_head = GetHead(), key_dst = (unsigned int)strtoul(dst, NULL, 16);
    FindAncestor(key_head, key_dst, idx[0][0]);
//    printf("key_ancestor = %x\n", key_ancestor); // DEBUG
    char ancestor[SHA_LENGTH];
    sprintf(ancestor, "%.8x", key_ancestor);

    // back up ancestor
    GitmCheckout(ancestor);
    if (mkdir(".gitm/backup_ancestor", DIR_MOD))
        perror("failed to create dir");
    CallCommand("diff -Nurx .gitm .gitm/backup_ancestor . >.gitm/patch_ancestor; "
                "patch -d .gitm/backup_ancestor -p0 <.gitm/patch_ancestor >/dev/null");
    remove(".gitm/patch_ancestor");
    // back up dst and use the appended patch
    GitmCheckout(dst);
    if (mkdir(".gitm/backup_dst", DIR_MOD))
        perror("failed to create dir");
    CallCommand("diff -Nurx .gitm .gitm/backup_dst . >.gitm/patch_dst; "
                "patch -d .gitm/backup_dst -p0 <.gitm/patch_dst >/dev/null");
    remove(".gitm/patch_dst");

    FILE *fp_merge = popen("diff -Nur .gitm/backup_ancestor .gitm/backup_HEAD >.gitm/patch_merge; "
                           "diff -Nur .gitm/backup_ancestor .gitm/backup_dst >>.gitm/patch_merge; "
                           "patch -d .gitm/backup_ancestor --merge -p2 <.gitm/patch_merge >/dev/null; "
                           "echo $?", "r");
    // judge whether conflict exists
    char head[SHA_LENGTH];
    sprintf(head, "%.8x", key_head);
    int chk;
    if (!fscanf(fp_merge, "%d", &chk))
        perror("failed to judge conflict exists");
    if (fp_merge != NULL)
        pclose(fp_merge);
    else
        perror("failed to merge");
    if (chk) {
        CallCommand("rm -rf .gitm/backup_* .gitm/patch_merge");
        GitmCheckout(head);
        puts("conflict");
//        printf("ancestor = %s, chk = %d\n", ancestor, chk); // DEBUG
        return 1;
    }
    // commit
    GitmCheckout(ancestor);
    CallCommand("patch --merge -p2 <.gitm/patch_merge >/dev/null");
    BackUp();
    // link new commit to HEAD
    CallCommand("diff -Nurx .gitm .gitm/backup_HEAD . >.gitm/objects/new_blob/patch");
    UpdateHead(head);
    unsigned key_merge = GitmCommit();
    char merge[SHA_LENGTH];
    sprintf(merge, "%.8x", key_merge);
    // link new commit to dst (patch2 will never be used.)
    char cmd_diff[80] = "diff -Nurx .gitm .gitm/backup_dst . >.gitm/objects/12345678/patch2";
    memcpy(cmd_diff + 51, merge, 8);
    CallCommand(cmd_diff);
    CallCommand("rm -rf .gitm/backup_* .gitm/patch_merge");
    // add commit to the index
    FILE *fp_index = fopen(".gitm/objects/index", "a");
    fprintf(fp_index, "%s %s\n", merge, dst);
    if (fp_index != NULL)
        fclose(fp_index);
    else
        perror("failed to add another key to index");
    UpdateHead(merge);
    return 0;
}

int FindAncestor(unsigned key_head, unsigned key_dst, unsigned key_root) {
    // utilize bitwise operation
    if (key_root == key_head)
        return 1;
    if (key_root == key_dst)
        return 2;
    int vis = 0;
    for (int i = 0; i < cnt_idx; ++i) {
        if (idx[i][1] == key_root && !idx[i][2]) {
            int tmp = FindAncestor(key_head, key_dst, idx[i][0]);
            if (tmp == 0)
                idx[i][2] = 1;
            else if (tmp == 3)
                return 0;
            vis |= tmp;
        }
    }
    if (vis == 3)
        key_ancestor = key_root;
    return vis;
}