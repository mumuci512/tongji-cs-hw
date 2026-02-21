#include <stdio.h>

#define m 60

int main() {
    int a[m], b[m], c[m], d[m];
    
    // 初始化a[0]和b[0]
    a[0] = 0;
    b[0] = 1;
    
    // 计算a[i]和b[i]
    for (int i = 1; i < m; i++) {
        a[i] = a[i - 1] + i;
        b[i] = b[i - 1] + 3 * i;
    }
    
    // 计算c[i]
    for (int i = 0; i < m; i++) {
        if (i <= 19) {
            c[i] = a[i];
        } else if (i <= 39) {
            c[i] = a[i] + b[i];
        } else { // 40 ≤ i ≤ 59
            c[i] = a[i] * b[i];
        }
    }
    
    // 计算d[i]
    for (int i = 0; i < m; i++) {
        if (i <= 19) {
            d[i] = b[i];
        } else if (i <= 39) {
            d[i] = a[i] * c[i];
        } else { // 40 ≤ i ≤ 59
            d[i] = c[i] * b[i];
        }
    }
    
    // 输出结果
    printf("i\ta[i]\tb[i]\tc[i]\td[i]\n");
    for (int i = 0; i < m; i++) {
        printf("%d\t%d\t%d\t%d\t%d\n", i, a[i], b[i], c[i], d[i]);
    }
    
    return 0;
}
