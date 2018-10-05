#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

// cache size for my CPU (i7 7700hq) is 6 MB
static const unsigned CACHE_SIZE = 6 * 1024 * 1024;
static const unsigned MAX_AMOUNT_OF_FRAGMENTS = 20;
static const int END = -1;

static inline unsigned long long rdtsc(void) {
    unsigned hi, lo;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
}

void fillCacheAssociativityTestBuf(int *buf, unsigned amout_of_fragments, unsigned offset) {
    unsigned buf_size = amout_of_fragments * offset;
    for (unsigned i = 0; i < amout_of_fragments; ++i) {
        for (unsigned j = 0; j < offset; ++j) {
            unsigned current_index = i * offset + j, should_point_to = (current_index + offset) % buf_size;
            buf[current_index] = should_point_to;
        }
    }
    // now we have cycles 0->3->6->0, 1->4->7->0, 2->5->8->2
    // we need last fragment to switch cycles for us
    for (unsigned j = 0; j < offset; ++j) {
        unsigned current_index = (amout_of_fragments - 1) * offset + j;
        buf[current_index]++;
    }
    // now last index in buf points to second element of first cycle
    // we need it to point nowhere (to the end)
    unsigned last_index = amout_of_fragments * offset - 1;
    buf[last_index] = END;
}

int *getCacheAssociativityTestBuffer(unsigned amout_of_fragments, unsigned offset) {
    unsigned buf_size = amout_of_fragments * offset;
    int *buf = (int*)malloc(buf_size * sizeof(unsigned));
    fillCacheAssociativityTestBuf(buf, amout_of_fragments, offset);
    return buf;
}

void runCacheAssociativityTest(unsigned amout_of_fragments, unsigned offset) {
    int *buf = getCacheAssociativityTestBuffer(amout_of_fragments, offset);
    int sum = 0;
    unsigned count = 0;
    unsigned long long rdtsc_elapsed = 0, rdtsc_before = 0, rdtsc_after = 0;
    for (int i = buf[0]; buf[i] != END; i = buf[buf[i]]) {
        rdtsc_before = rdtsc();

        sum += i;

        rdtsc_after = rdtsc();
        rdtsc_elapsed += rdtsc_after - rdtsc_before;

        count++;
    }

    free(buf);
    
    unsigned long long rdtsc_avg = rdtsc_elapsed / count;

    printf("Test results:\n"
           "amout_of_fragments = %d, count = %d, rdtsc_avg = %llu\n"
           , amout_of_fragments, count, rdtsc_avg);
}

int main(int argc, char *argv[]) {
    for (unsigned i = 4; i < MAX_AMOUNT_OF_FRAGMENTS; ++i) {
        runCacheAssociativityTest(i, CACHE_SIZE);
    }
    return 0;
}
