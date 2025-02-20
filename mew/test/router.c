#include "http/router.c"

#include "mewtest.h"

mewtest test_ok() {
    int a = 34;
    int b = 35;
    int c = a + b;
    mewassert("Sum should work", c == 69);
    return NULL;
}

mewtest test_not_ok() {
    int a = 34;
    int b = 35;
    int c = a + b;
    mewassert("Sum should work", c == 70);
    return NULL;
}

int main() {
    mewtest_run(test_ok);
    mewtest_run(test_not_ok);
    return 0;
}
