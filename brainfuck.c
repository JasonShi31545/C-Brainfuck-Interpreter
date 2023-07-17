#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

typedef struct _stack {
    union Data {
        size_t id;
        char ch;
    } space[30000];
    size_t head;
} Stack;

void StackInit(Stack *s) {
    s->head = 30000;
}

void StackPushChar(Stack *s, char ch) {
    s->head--;
    s->space[s->head].ch = ch;
}

void StackPushIt(Stack *s, size_t id) {
    s->head--;
    s->space[s->head].id = id;
}

void StackPopChar(Stack *s) {
    s->space[s->head].ch = 0x0;
    s->head++;
    assert(s->head <= 30000);
}

void StackPopIt(Stack *s) {
    s->space[s->head].id = 0;
    s->head++;
    assert(s->head <= 30000);
}

char StackTopChar(Stack *s) {
    return s->space[s->head].ch;
}

size_t StackTopIt(Stack *s) {
    return s->space[s->head].id;
}

#define BRAINFUCK_MEMORY_SIZE 30000

int main(int argc, const char *argv[]) {


    printf("Starting Allocation...\n");
    char *_mem = (char *)calloc(sizeof(char), BRAINFUCK_MEMORY_SIZE);
    char *mem = _mem;
    printf("Allocated Successfully\n");


    FILE *src = NULL;
    if (argc == 2) {
        if (argv[1] != NULL) {
            FILE *_src = fopen(argv[1], "r");
            if (_src != NULL) {
                src = _src;
            }
        } else {
            return -1;
        }
    } else {
        fprintf(stderr, "No file specified\n");
        return -1;
    }
    

    // memory safe;

    long size;
    rewind(src);
    (void)fseek(src, 0, SEEK_END);
    size = ftell(src);
    rewind(src);

    assert(size > 0);

    char *buf = (char *)malloc(sizeof(char) * size);
    if (buf == NULL) {
        fprintf(stderr, "Error: failed to allocate source buffer\n");
    }
    
    int i = 0;
    char character = fgetc(src);
    while(character != EOF) {
        buf[i] = character;
        i++;
        character = fgetc(src);
    }
    rewind(src);

    fclose(src);

    char *text = buf;

    printf("%s\n\n", text);

    int dptr = 0;
    size_t srcid = 0;

    Stack bstack;
    StackInit(&bstack);
    Stack istack;
    StackInit(&istack);

    while (srcid < size) {
        char ch = text[srcid];
        switch (ch) {
        case '>': {
            dptr++;
            assert(dptr < BRAINFUCK_MEMORY_SIZE);
            srcid++;
            break;
        }
        case '<': {
            dptr--;
            assert(dptr >= 0);
            srcid++;
            break;
        }
        case '+': {
            mem[dptr]++;
            srcid++;
            break;
        }
        case '.': {
//            printf("%c", mem[dptr]);
            putchar(mem[dptr]);
            srcid++;
            break;
        }
        case ',': {
            scanf("%c", &mem[dptr]);
            srcid++;
            break;
        }
        case '-': {
            mem[dptr]--;
            srcid++;
            break;
        }
        case '[': {
            if (mem[dptr] == 0) {
                int brace_count = 1;
                srcid++;
                while (brace_count != 0) {
                    if (text[srcid] == '[') {
                        brace_count++;
                    } else if (text[srcid] == ']') {
                        brace_count--;
                    }
                    srcid++;
                }
                break;
            }
            StackPushChar(&bstack, '[');
            StackPushIt(&istack, srcid);
            srcid++;
            break;
        }
        case ']': {
            if (StackTopChar(&bstack) == '[') {
                StackPopChar(&bstack);
                if (mem[dptr] != 0) {
                    srcid = StackTopIt(&istack);
                } else {
                    srcid++;
                }
                StackPopIt(&istack);
            } else {
                fprintf(stderr, "Mismathcing ]\n");
                free(mem);
                free(buf);
                return -5;
            }
            break;
        }
        default:
            srcid++;
            break;
        }
    }
    free(mem);
    free(buf);
    buf = NULL;
    text = NULL;
    _mem = NULL;
    mem = NULL;
    return 0;
}
