#include "cachelab.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> /* atol exit*/
#include <stdint.h> /* uintN_t */
#include <string.h>
#include <getopt.h>  /* getopt -std=c99 POSIX macros defined in <features.h> prevents <unistd.h> from including <getopt.h>*/

typedef  char* string;
typedef int bool;
#define TRUE 1
#define FALSE 0


struct Summary{
    int hit_count;
    int miss_count;
    int eviction_count;
};     //Result of the summary

struct Operation{
    char op;
    uint64_t address;
    int size;
};   //opreation of the trace

const string helpMassage = "Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n" \
        "Options:\n" \
        "  -h         Print this help message.\n" \
        "  -v         Optional verbose flag.\n" \
        "  -s <num>   Number of set index bits, which should be larger than 0.\n" \
        "  -E <num>   Number of lines per set, which should be larger than 0.\n" \
        "  -b <num>   Number of block offset bits, which should be larger than 0.\n" \
        "  -t <file>  Trace file.\n" \
        "\n" \
        "Examples:\n" \
        "  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n" \
        "  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n";

int s = 0,b = 0;
uint64_t E = 0, S = 0, B =0, Heartbeat = 1;   //S = 2^s sets, B = 2^b sets  and C = S x E x B data bytes.
uint64_t *p;                    // The cache
uint64_t opAddress;
string t = NULL;
bool DetailFlag = FALSE;

struct Summary SummaryResult = {0,0,0};
struct Operation OperationStruct = {0,0,0};

void Summary();
void HitDetect();
/*
*   Get the arguments and process the trace file
*/
int main(int argc, char * argv[])
{
    int ch;
    while ((ch = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch (ch)
        {
            case 'h':
                printf(helpMassage);
                exit(EXIT_SUCCESS);
            case 'v':
                DetailFlag = TRUE;
                break;
            case 's':
                s = atoi(optarg);
                if (s <= 0)  /* We assume that there are at least two sets */
                {
                    printf("%s", helpMassage);
                    exit(EXIT_FAILURE);
                }
                S = 1<<s;
                break;
            case 'E':
                E = atoi(optarg);
                if (E <= 0)  /* We assume that there are at least two sets */
                {
                    printf("%s", helpMassage);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'b':
                b = atoi(optarg);
                if (b <= 0)  /* We assume that there are at least two sets */
                {
                    printf("%s", helpMassage);
                    exit(EXIT_FAILURE);
                }
                B = 1<<b;
                break;
            case 't':
                t = optarg;
                break;
            default:

                printf("%s", helpMassage);
                exit(EXIT_FAILURE);
            }

    }
    if (s == 0 || b ==0 || E == 0 || t == NULL)
    {
        printf("%s", helpMassage);
        exit(EXIT_FAILURE);
    }

    Summary();
    //printf("%d %d %d\n", SummaryResult.eviction_count, SummaryResult.hit_count, SummaryResult.miss_count);
    printSummary(SummaryResult.hit_count, SummaryResult.miss_count, SummaryResult.eviction_count);
    return 0;
}

/*
*   Manange the cache and test the trace
*/
void Summary()
{
    //Initialize the Cache

    p = (uint64_t*)malloc(sizeof(uint64_t) * S*E*2);
    if(NULL == p)
    {
        perror("error...");
        exit(1);
    }

    for(int i=0;i<S*E*2;)
    {
        p[i] = (int64_t)(-1);
        i+=2;
    }

    opAddress = ~(uint64_t)(-1<<s);

    // Read  the trace file
    FILE *trace_file = fopen(t, "r");
    if(trace_file==NULL)
    {
        printf("%s", helpMassage);
        exit(EXIT_FAILURE);
    }

    char line[80];
    string pline = NULL;

    while (fgets(line, 80, trace_file) != NULL) {
        pline = line;
        if (*pline++ == 'I')
            continue;
        sscanf(pline, "%c %lx,%d", &OperationStruct.op, &OperationStruct.address, &OperationStruct.size);

        // Detect the trace line
        HitDetect();
    }
    free(p);
    p = NULL;

    return;
}
/*
*   Detect the trace of a specific line
*/
void HitDetect()
{

    int flag = 0;
    uint64_t transAddress = OperationStruct.address>>b;
    uint64_t tag = OperationStruct.address>>b>>s;
    uint64_t index = transAddress & opAddress;
    uint64_t transIndex = 0, transHeadbeat = Heartbeat;

    for(int i=0;i<E;i++)
    {
        if(p[(index*E+i)*2]==tag)
        {
            flag = 2;
            p[(index*E+i)*2+1] = Heartbeat;
            Heartbeat++;
            break;
        }else
        {
            if(p[(index*E+i)*2+1]==0)
                flag = 1;

            if(p[(index*E+i)*2+1]<transHeadbeat)
            {
                transIndex = (index*E+i)*2;
                transHeadbeat = p[(index*E+i)*2+1];
            }
        }
    }

    if(flag>1)
    {
        if(DetailFlag)
            printf("%c %lx,%d hit",OperationStruct.op, OperationStruct.address, OperationStruct.size);
        SummaryResult.hit_count++;
    }else
    {
        p[transIndex] = tag;
        p[transIndex+1] = Heartbeat;
        Heartbeat++;
        if(flag==0)
        {
            if(DetailFlag)
                printf("%c %lx,%d miss eviction",OperationStruct.op, OperationStruct.address, OperationStruct.size);
            SummaryResult.miss_count++;
            SummaryResult.eviction_count++;
        }else
        {
            if(DetailFlag)
                printf("%c %lx,%d miss",OperationStruct.op, OperationStruct.address, OperationStruct.size);
            SummaryResult.miss_count++;
        }

    }

    if (OperationStruct.op == 'M') {
        if(DetailFlag)
            printf(" hit\n");
        SummaryResult.hit_count++;
    }
    else if(DetailFlag)
        printf("\n");

    return;
}
