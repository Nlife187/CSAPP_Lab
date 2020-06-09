#include "cachelab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define unit long unsigned int
#define For(i, a, b) for(int i = a; i < b; i++)
#define INF 0x3f3f3f3f
unit Hits = 0, Misses = 0, Evictions = 0, verbose = 0;
struct _Cache
{
    unit s, E, b;
    unit *** cache;
}Cache;
void Set_Cache(unit _s, unit _E, unit _b)
{
    Cache.s = _s; Cache.E = _E; Cache.b = _b;
}
void Get_Cache()
{
    Cache.cache = (unit ***)malloc(sizeof(unit **) * Cache.s);
    For(i, 0, Cache.s) Cache.cache[i] = (unit **)malloc(sizeof(unit *) * Cache.E);
    For(i, 0, Cache.s)
	For(j, 0, Cache.E)
	    Cache.cache[i][j] = (unit *)malloc(sizeof(unit) * 3);
    For(i, 0, Cache.s) For(j, 0, Cache.E)
    {
	Cache.cache[i][j][0] = 0;
	Cache.cache[i][j][1] = 0;
	Cache.cache[i][j][2] = INF;
    }
}
void Free_Cache()
{
    For(i, 0, Cache.s) For(j, 0, Cache.E) free(Cache.cache[i][j]);
    For(i, 0, Cache.s) free(Cache.cache[i]);
    free(Cache.cache);
}
unit Is_Hit(unit _s, unit Tag)
{
    For(i, 0, Cache.E)
    {
	if (Cache.cache[_s][i][0] == 1 && Cache.cache[_s][i][1] == Tag)
	    return i;
    }
    return -1;
}
unit Get_Min_Lru(unit _s)
{
    unit Lru = INF, pos = 0;
    For(i, 0, Cache.E)
    {
	if (Cache.cache[_s][i][0] == 0) return i;
	unit temp = Cache.cache[_s][i][2];
	if (Lru > temp)
	{
	    Lru = temp;
	    pos = i;
	}
    }
    return pos;
}
void Replace(unit _s, unit Tag)
{
    unit pos = Is_Hit(_s, Tag);
    if (pos == -1)
    {
	++Misses;
	if (verbose) printf("miss ");
	unit side = Get_Min_Lru(_s);
	if (Cache.cache[_s][side][0] == 1)
	{
	    ++Evictions;
	    if (verbose) printf("evctions");
	}
	Cache.cache[_s][side][0] = 1;
	Cache.cache[_s][side][1] = Tag;
	Cache.cache[_s][side][2] = INF;
	For(i, 0, Cache.E) if (i != side) --Cache.cache[_s][i][2];
    }
    else
    {
	++Hits;
	Cache.cache[_s][pos][2] = INF;
	For(i, 0, Cache.E) if (i != pos) --Cache.cache[_s][i][2];
	if (verbose) printf("hit ");
    }
}
void put_Set(unit _s)
{
    For(i, 0, Cache.E) printf("%lu %lx %lu\n", Cache.cache[_s][i][0],
		    Cache.cache[_s][i][1], Cache.cache[_s][i][2]);
}
void Show_Cache()
{
    printf("S =%lu E =%lu B=%lu", Cache.s, Cache.E, Cache.b);
    printf("************************************\n\n");
    For(i, 0, Cache.s) { put_Set(i); printf("\n"); }
    printf("************************************\n");
}
void Print_Help()
{
	printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n\n");

	printf("Options:\n");
	printf("	-h         Print this help message.\n");
    printf("	-v         Optional verbose flag.\n");
    printf("	-s <num>   Number of set index bits.\n");
    printf("	-E <num>   Number of lines per set.\n");
    printf("	-b <num>   Number of block offset bits.\n");
    printf("	-t <file>  Trace file.\n\n");

	printf("Examples:\n");
	printf("linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}
int main(int argc, char * const argv[])
{
    _Bool wrong = 0;
    char opt;
    unit _s = -1, _E = -1, _b = -1;
    FILE * path = NULL;
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
	switch (opt)
	{
	    case 'h': Print_Help(); break;
	    case 'v': verbose = 1; break;
	    case 's': _s = atol(optarg); break;
	    case 'E': _E = atol(optarg); break;
	    case 'b': _b = atol(optarg); break;
	    case 't': path = fopen(optarg, "r"); break;
	    default: wrong = 1;
	}
    }
    if (_s <= 0 || _E <= 0 || _b <= 0 || path == NULL || wrong)
    {
	Print_Help();
	return 0;
    }
    Set_Cache(1 << (unit)_s, _E,1 << (unit)_b);
    Get_Cache();
    unit addr, number, Tag, S;
    char cmd[2], Ch;
    while (fscanf(path, "%s%lx%c%lu",cmd, &addr, &Ch, &number) != EOF)
    {
	if (cmd[0] == 'I') continue;
	Tag = addr >> (_s + _b);
	S = (addr & ((1lu << (_s + _b)) - 1)) >> _b;
	if (verbose) printf("%c %lx%c%lu ", cmd[0], addr, Ch, number);
	if (cmd[0] == 'L' || cmd[0] == 'S')	  
	    Replace(S, Tag);
	else
	{
	    Replace(S, Tag);
	    Replace(S, Tag);
	}
	if (verbose) printf("\n");
    }
    printSummary(Hits, Misses, Evictions);
    Free_Cache();

    return 0;
}
