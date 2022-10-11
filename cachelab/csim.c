#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct CacheUnit
{
    int vaild;
    int tag;
    int time;
} CacheLine;

int s, S, E, b;
char *path;

int hits = 0;
int miss = 0;
int evict = 0;

CacheLine **cache;
FILE *trace;

int main(int argc, char **argv)
{
    processArgs(argc, argv);
    trace = fopen(path, "r");
    printf("%p", trace);

    initCache();
    processTrace();
    freeCache();

    fclose(trace);
    printSummary(hits, miss, evict);
    return 0;
}

void processArgs(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        // printf("Args:%s\n", argv[i]);
        if (strcmp(argv[i], "-s") == 0)
        {
            i++;
            s = atoi(argv[i]);
            S = 1 << s;
        }
        else if (strcmp(argv[i], "-E") == 0)
        {
            i++;
            E = atoi(argv[i]);
        }
        else if (strcmp(argv[i], "-b") == 0)

        {
            i++;
            b = atoi(argv[i]);
        }
        else if (strcmp(argv[i], "-t") == 0)
        {
            i++;
            path = argv[i];
        }
    }
}

void processTrace()
{
    char instruction;
    __uint64_t address;
    int size;
    while (fscanf(trace, "%c %lx %d", &instruction, &address, &size) != EOF)
    {
        if (instruction == 'I')
        {
            continue;
        }
        else if (instruction == 'L')
        {
            load(address, size);
        }
        else if (instruction == 'M')
        {
            modify(address, size);
        }
        else if (instruction == 'S')
        {
            store(address, size);
        }
        addAllTime();
    }
}

void load(__uint64_t address, int size)
{
    int tag = address >> (b + s);
    int indexOfSet = (address << (64 - (b + s))) >> (64 - s);
    int maxTime = -__INT32_MAX__;
    int indexOfMax = -1;

    CacheLine *lines = cache[indexOfSet];
    for (int i = 0; i < E; ++i)
    {
        if (lines[i].tag == tag)
        {
            hits++;
            lines[i].time = 0;
            return;
        }
    }

    for (int i = 0; i < E; ++i)
    {
        if (lines[i].vaild == 0)
        {
            lines[i].tag = tag;
            lines[i].time = 0;
            lines[i].vaild = 1;
            miss++;
            return;
        }
    }

    for (int i = 0; i < E; ++i)
    {
        if (lines[i].time > maxTime)
        {
            maxTime = lines[i].time;
            indexOfMax = i;
        }
    }

    lines[indexOfMax].tag = tag;
    lines[indexOfMax].time = 0;
    miss++;
    evict++;
}

void modify(__uint64_t address, int size)
{
    load(address, size);
    store(address, size);
}

void store(__uint64_t address, int size)
{
    load(address, size);
}

void initCache(void)
{
    cache = (CacheLine **)malloc(S * sizeof(CacheLine *));
    for (int i = 0; i < S; ++i)
    {
        cache[i] = (CacheLine *)malloc(E * sizeof(CacheLine));
        for (int j = 0; j < E; ++j)
        {
            cache[i][j].vaild = 0;
            cache[i][j].tag = -1;
            cache[i][j].time = -1;
        }
    }
}

void addAllTime()
{
    for (int i = 0; i < S; ++i)
    {
        for (int j = 0; j < E; ++j)
        {
            if (cache[i][j].vaild == 1)
            {
                cache[i][j].time++;
            }
        }
    }
}

void freeCache(void)
{
    for (int i = 0; i < S; ++i)
    {
        free(cache[i]);
    }
    free(cache);
}
