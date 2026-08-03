/*
 * dpack/stub/stub.c
 * Djazair Self-Extracting Executable Stub
 *
 * This is a minimal runner that is prepended to a ZIP bundle.
 * Layout of the final packed executable:
 *
 *   [ stub binary ] [ ZIP data ] [ 8-byte LE: offset of ZIP from file start ]
 *
 * On execution:
 *   1. Locates itself and reads the ZIP offset from the last 8 bytes.
 *   2. Copies the ZIP to a temp directory.
 *   3. Extracts the ZIP (contains: djazair[.exe], __main__.dz, std/, prelude/, extensions/).
 *   4. Launches: <tmp>/djazair[.exe] <tmp>/__main__.dz [user args...]
 *   5. Cleans up the temp directory.
 *   6. Exits with the same return code as the script.
 *
 * Developer: Harizi Riyadh (hariziriyadh@gmail.com)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#ifdef _WIN32
#  include <windows.h>
#  include <process.h>
#  define PATH_SEP "\\"
#  define INTERP   "djazair.exe"
#else
#  include <unistd.h>
#  include <sys/stat.h>
#  include <sys/types.h>
#  include <sys/wait.h>
#  include <ftw.h>
#  define PATH_SEP "/"
#  define INTERP   "djazair"
#endif

/* ── Marker written at build time by the packer ─────────────────────────── */
#define DPACK_MAGIC 0x4B434150445A4A44ULL  /* "DJZDPACK" in LE */

/* ── Portable uint64 read (little-endian) ────────────────────────────────── */
static uint64_t read_u64_le(const unsigned char *p) {
    uint64_t v = 0;
    for (int i = 0; i < 8; i++)
        v |= ((uint64_t)p[i]) << (8 * i);
    return v;
}

/* ── Get path to this executable ─────────────────────────────────────────── */
static int get_self_path(char *buf, size_t sz) {
#ifdef _WIN32
    DWORD n = GetModuleFileNameA(NULL, buf, (DWORD)sz);
    return (n > 0 && n < sz) ? 1 : 0;
#else
    ssize_t n = readlink("/proc/self/exe", buf, sz - 1);
    if (n < 0) {
        /* fallback: use argv[0] set by caller */
        return 0;
    }
    buf[n] = '\0';
    return 1;
#endif
}

/* ── Create a unique temp directory ─────────────────────────────────────── */
static int make_temp_dir(char *out, size_t sz) {
#ifdef _WIN32
    char tmp[MAX_PATH];
    if (!GetTempPathA(MAX_PATH, tmp)) return 0;
    /* Use PID for uniqueness */
    snprintf(out, sz, "%sdpack-%lu", tmp, (unsigned long)GetCurrentProcessId());
    return CreateDirectoryA(out, NULL) ? 1 : 0;
#else
    const char *tmp = getenv("TMPDIR");
    if (!tmp) tmp = "/tmp";
    snprintf(out, sz, "%s/dpack-%d-XXXXXX", tmp, (int)getpid());
    return mkdtemp(out) ? 1 : 0;
#endif
}

/* ── Copy a range of bytes from one file to another ─────────────────────── */
static int copy_range(const char *src_path, uint64_t offset, uint64_t length, const char *dst_path) {
    FILE *in = fopen(src_path, "rb");
    if (!in) return 0;
    FILE *out = fopen(dst_path, "wb");
    if (!out) { fclose(in); return 0; }

    if (fseek(in, (long)offset, SEEK_SET) != 0) {
        fclose(in); fclose(out); return 0;
    }

    char buf[65536];
    uint64_t remaining = length;
    while (remaining > 0) {
        size_t to_read = (size_t)(remaining < sizeof(buf) ? remaining : sizeof(buf));
        size_t n = fread(buf, 1, to_read, in);
        if (n == 0) break;
        fwrite(buf, 1, n, out);
        remaining -= n;
    }

    fclose(in);
    fclose(out);
    return remaining == 0 ? 1 : 0;
}

/* ── Extract ZIP to a directory ─────────────────────────────────────────── */
static int extract_zip(const char *zip_path, const char *dest_dir) {
    char cmd[4096];
#ifdef _WIN32
    /* PowerShell Expand-Archive */
    snprintf(cmd, sizeof(cmd),
        "powershell -NoProfile -Command \"Expand-Archive -Path '%s' -DestinationPath '%s' -Force\"",
        zip_path, dest_dir);
#else
    snprintf(cmd, sizeof(cmd), "unzip -q -o \"%s\" -d \"%s\"", zip_path, dest_dir);
#endif
    int ret = system(cmd);
    return ret == 0 ? 1 : 0;
}

/* ── Recursively delete a directory ─────────────────────────────────────── */
#ifdef _WIN32
static void rmdir_recursive(const char *path) {
    char cmd[2048];
    snprintf(cmd, sizeof(cmd), "rmdir /s /q \"%s\"", path);
    system(cmd);
}
#else
static int _rm_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    (void)sb; (void)typeflag; (void)ftwbuf;
    return remove(fpath);
}
static void rmdir_recursive(const char *path) {
    nftw(path, _rm_cb, 64, FTW_DEPTH | FTW_PHYS);
}
#endif

/* ── Main ────────────────────────────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    /* ── Step 1: locate self ── */
    char self_path[4096] = {0};
    if (!get_self_path(self_path, sizeof(self_path))) {
        /* fallback to argv[0] */
        if (argc > 0) strncpy(self_path, argv[0], sizeof(self_path) - 1);
    }

    FILE *self = fopen(self_path, "rb");
    if (!self) {
        fprintf(stderr, "[dpack] Cannot open self: %s\n", self_path);
        return 1;
    }

    /* ── Step 2: read ZIP offset from last 8 bytes ── */
    if (fseek(self, -8, SEEK_END) != 0) {
        fprintf(stderr, "[dpack] Cannot seek in executable\n");
        fclose(self);
        return 1;
    }

    long file_size_long = 0;
    fseek(self, 0, SEEK_END);
    file_size_long = ftell(self);
    uint64_t file_size = (uint64_t)file_size_long;

    fseek(self, -8, SEEK_END);
    unsigned char offset_buf[8];
    if (fread(offset_buf, 1, 8, self) != 8) {
        fprintf(stderr, "[dpack] Cannot read ZIP offset\n");
        fclose(self);
        return 1;
    }
    fclose(self);

    uint64_t zip_offset = read_u64_le(offset_buf);
    uint64_t zip_size   = file_size - zip_offset - 8;

    if (zip_offset == 0 || zip_offset >= file_size || zip_size == 0) {
        fprintf(stderr, "[dpack] Invalid bundle: no embedded data found.\n");
        fprintf(stderr, "        This executable was not packed with dpack.\n");
        return 1;
    }

    /* ── Step 3: create temp directory ── */
    char tmp_dir[2048] = {0};
    if (!make_temp_dir(tmp_dir, sizeof(tmp_dir))) {
        fprintf(stderr, "[dpack] Cannot create temp directory\n");
        return 1;
    }

    /* ── Step 4: copy ZIP from self to temp ── */
    char zip_path[2200];
    snprintf(zip_path, sizeof(zip_path), "%s%sbundle.zip", tmp_dir, PATH_SEP);
    if (!copy_range(self_path, zip_offset, zip_size, zip_path)) {
        fprintf(stderr, "[dpack] Cannot extract bundle data\n");
        rmdir_recursive(tmp_dir);
        return 1;
    }

    /* ── Step 5: extract ZIP ── */
    if (!extract_zip(zip_path, tmp_dir)) {
        fprintf(stderr, "[dpack] Cannot extract bundle\n");
        rmdir_recursive(tmp_dir);
        return 1;
    }
    /* Remove temp zip */
#ifdef _WIN32
    DeleteFileA(zip_path);
#else
    remove(zip_path);
#endif

    /* ── Step 6: set up paths ── */
    char interp_path[4096];
    char script_path[4096];
#ifdef _WIN32
    snprintf(interp_path, sizeof(interp_path), "%s\\bin\\djazair.exe", tmp_dir);
    snprintf(script_path, sizeof(script_path), "%s\\__main__.dz", tmp_dir);
#else
    snprintf(interp_path, sizeof(interp_path), "%s/bin/djazair", tmp_dir);
    snprintf(script_path, sizeof(script_path), "%s/__main__.dz", tmp_dir);
#endif

#ifndef _WIN32
    chmod(interp_path, 0755);
#endif

    /* ── Step 7: build command ── */
#ifdef _WIN32
    /* Build command line for CreateProcess */
    char cmd[8192];
    int  pos = 0;
    pos += snprintf(cmd + pos, sizeof(cmd) - pos, "\"%s\" \"%s\"", interp_path, script_path);
    for (int i = 1; i < argc && pos < (int)sizeof(cmd) - 64; i++) {
        pos += snprintf(cmd + pos, sizeof(cmd) - pos, " \"%s\"", argv[i]);
    }

    STARTUPINFOA        si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);

    int exit_code = 1;
    if (CreateProcessA(
            NULL,               /* application name (use cmd line) */
            cmd,                /* command line */
            NULL,               /* process security */
            NULL,               /* thread security */
            FALSE,              /* inherit handles */
            CREATE_NO_WINDOW,   /* ← no black console window */
            NULL,               /* environment */
            tmp_dir,            /* working directory = extracted tmp */
            &si,
            &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD code = 1;
        GetExitCodeProcess(pi.hProcess, &code);
        exit_code = (int)code;
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
#else
    /* On Unix, use execv for proper signal handling */
    char **new_argv = malloc(sizeof(char *) * (argc + 2));
    if (!new_argv) { rmdir_recursive(tmp_dir); return 1; }
    new_argv[0] = interp_path;
    new_argv[1] = script_path;
    for (int i = 1; i < argc; i++) new_argv[i + 1] = argv[i];
    new_argv[argc + 1] = NULL;

    pid_t pid = fork();
    int exit_code = 1;
    if (pid == 0) {
        execv(interp_path, new_argv);
        fprintf(stderr, "[dpack] execv failed: %s\n", strerror(errno));
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    }
    free(new_argv);
#endif

    /* ── Step 8: cleanup ── */
    rmdir_recursive(tmp_dir);

    return exit_code;
}
