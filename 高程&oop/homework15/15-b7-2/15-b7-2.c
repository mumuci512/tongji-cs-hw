/* 2350222 盛曦 计科 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

//根据需要可加入其它头文件
#include <string.h>
#if (__linux__) 
#else
#include <conio.h>
#endif

//此处为示例，允许修改结构体名称，允许修改结构体中的成员内容，要求sizeof必须是64
typedef struct player {
    char buf[64];
}PLAYER;

/* 此处允许新增函数，数量不限
   1、所有新增的函数，均不允许定义新的 FILE* 并进行打开/读/写/关闭等操作
   2、上述限制同样适用于main函数
*/
int usage(char* proname)
{
    printf("usage : %s --modify | --read\n",proname);
    return 0;
}

int print(PLAYER* p, int* size, char (*prompt)[20], char r_m)
{
    if (r_m == 'm')
    {
        printf("--------------------------------------\n");
        printf("游戏存档文件修改工具\n");
        printf("--------------------------------------\n");
    }
    for (int i = 0; i < 25; i++)
    {
        int displacement = 0;//表示在buf中的偏移量
        for (int j = 0; j < i; j++)
            displacement += size[j];

        if (displacement >= 47)//预留值
            displacement++;

        if (r_m == 'r')
        {
            printf("%18s：", prompt[i]);
            if (i == 0)
                printf("%s", p->buf);//第16个是\0
            else if (size[i] == 1)
                printf("%d", (int)*(unsigned char*)(p->buf + displacement));
            else if (size[i] == 2)
                printf("%u", *(unsigned short*)(p->buf + displacement));
            else if (size[i] == 4)
                printf("%u", *(unsigned int*)(p->buf + displacement));
            else if (size[i] == 8)
                printf("%llu", *(unsigned long long int*)(p->buf + displacement));
        }
        else
        {
            printf("  %c.%s", 'a' + i, prompt[i]);
            if (16 - strlen(prompt[i]) - 4 != 0)
                printf("%*c", (int)(16 - strlen(prompt[i]) - 4), ' ');
            if (i == 0)
                printf("(%s)", p->buf);
            else if (size[i] == 1)
                printf("(%d)", (int)*(unsigned char*)(p->buf + displacement));
            else if (size[i] == 2)
                printf("(%u)", *(unsigned short*)(p->buf + displacement));
            else if (size[i] == 4)
                printf("(%u)", *(unsigned int*)(p->buf + displacement));
            else if (size[i] == 8)
                printf("(%llu)", *(unsigned long long int*)(p->buf + displacement));
        }

        printf("\n");
    }

    if (r_m == 'm')
    {
        printf("--------------------------------------\n");
        printf("  0.放弃修改\n");
        printf("  1.存盘退出\n");
        printf("--------------------------------------\n");
        printf("请选择[a..y, 0..1]");
    }
    return 0;
}


long long int input(char* prompt, long long int min, long long int max)
{
    long long int temp;
    int ret = scanf("%lld", &temp);
    if (ret== 0)
    {
        while (getchar() != '\n')
            ;
        return -1;
    }
    if (temp < min || temp>max)
    {
        printf("非法的%s值\n",prompt);
        return -1;
    }
    return temp;
}
int deal_input(PLAYER* p, char (*prompt)[20], char cmd, int displacement)
{
    if (cmd == 'A')
    {
        int i;
        for (i = 0; i < 15; i++)
        {
            p->buf[i] = getchar();
            if (p->buf[i] == '\n' || p->buf[i] == '\r')
                break;
        }
        if(i==15)//没有读到回车，需要清空缓冲区
			while (getchar() != '\n')
				;

        for (; i <= 15; i++)
            p->buf[i] = '\0';        
        return 0;
    }
    else if (cmd == 'B' || cmd == 'C' || cmd == 'S')
    {
        long long int t;
        if ((t = input(prompt[cmd - 'A'], 0, 10000)) == -1)
            return 1;
        else
        {
            unsigned short temp = (unsigned short)t;
            *(unsigned short*)(p->buf + displacement) = temp;
            return 0;
        }
    }
    else if (cmd == 'D')
    {
        long long int t;
        if ((t = input(prompt[cmd - 'A'], 0, 8192)) == -1)
            return 1;
        else
        {
            unsigned short temp = (unsigned short)t;
            *(unsigned short*)(p->buf + displacement) = temp;
            return 0;
        }
    }
    else if (cmd == 'E')
    {
        long long int t;
        if ((t = input(prompt[cmd - 'A'], 0, 1024)) == -1)
            return 1;
        else
        {
            unsigned short temp = (unsigned short)t;
            *(unsigned short*)(p->buf + displacement) = temp;
            return 0;
        }
    }
    else if (cmd == 'F')
    {
        long long int t;
        if ((t = input(prompt[cmd - 'A'], 0, 100000000)) == -1)
            return 1;
        else
        {
            unsigned int temp = (unsigned int)t;
            *(unsigned int*)(p->buf + displacement) = temp;
            return 0;
        }
    }
    else if (cmd == 'G' || cmd == 'H')
    {
        long long int t;
        if ((t = input(prompt[cmd - 'A'], 0, 1000000)) == -1)
            return 1;
        else
        {
            unsigned int temp = (unsigned int)t;
            *(unsigned int*)(p->buf + displacement) = temp;
            return 0;
        }
    }
    else if (cmd == 'I')
    {
        long long int t;
#if (__linux__)       
        t = input(prompt[cmd - 'A'], 0L, 10000000000000000);
#else
        t = input(prompt[cmd - 'A'], 0LL, 10000000000000000);
#endif
        if (t == -1)
            return 1;
        else
        {
            unsigned long long int temp = (unsigned long long int)t;
            *(unsigned long long int*)(p->buf + displacement) = temp;
            return 0;
        }
    }
    else if (cmd == 'J' || cmd == 'K' || cmd == 'L' || cmd == 'O' || cmd == 'P' || cmd == 'Q' || cmd == 'R' || cmd == 'T' || cmd == 'U' || cmd == 'V' || cmd == 'W' || cmd == 'X' || cmd == 'Y')
    {
        long long  int t;
        if ((t = input(prompt[cmd - 'A'], 0, 100)) == -1)
            return 1;
        else
        {
            unsigned char temp = (unsigned char)t;
            *(unsigned char*)(p->buf + displacement) = temp;
            return 0;
        }
    }
    else if (cmd == 'M' || cmd == 'N')
    {
        long long int t;
        if ((t = input(prompt[cmd - 'A'], 0, 2000)) == -1)
            return 1;
        else
        {
            unsigned short temp = (unsigned short)t;
            *(unsigned short*)(p->buf + displacement) = temp;
            return 0;
        }
    }
    return -1;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：整个函数，只允许出现一次fopen、一次fread（因为包含错误处理，允许多次fclose）
***************************************************************************/
int read()
{
    /* 本函数中只允许定义一个 FILE* */
    FILE* fp;

    /* 文件打开，具体要求为：
       1、要求以读方式打开，打开方式***自行指定
       2、除本次fopen外，本函数其它地方不允许再出现fopen/freopen  */
    fp = fopen("game.dat", "rb");

    /* 进行后续操作，包括错误处理、读文件、显示各游戏项的值、关闭文件等，允许调用函数
       其中：只允许用一次性读取64字节的方法将game.dat的内容读入***（缓冲区名称、结构体名称自行指定）
                 fread(***, 1, sizeof(demo), fp);
    */
	if (!fp)
	{
		printf("无法打开文件game.dat\n");
		return -1;
	}
    fseek(fp, 0, SEEK_END);
    if (ftell(fp) != 64)
    {
        printf("game.dat大小不是64字节\n");
        return -1;
    }
    fseek(fp, 0, SEEK_SET);

    PLAYER p;
    fread(p.buf, 1, sizeof(p), fp);

    if (p.buf[15] != '\0')
    {
        printf("玩家昵称错误，最后一个字节应为尾0，显示在屏幕上的是已人为修正过的\n");
        p.buf[15] = '\0';
    }

    char prompt[26][20] = { "玩家昵称", "生命值", "力量值", "体质值", "灵巧值", "金钱值", "名声值", "魅力值", "游戏累计时间(us)值", "移动速度值", "攻击速度值", "攻击范围值", "攻击力值", "防御力值", "敏捷度值", "智力值", "经验值", "等级值", "魔法值", "消耗魔法值", "魔法伤害力值", "命中率值", "魔法防御力值", "暴击率值", "耐力值" };
    int size[26] = { 16, 2, 2, 2, 2, 4, 4, 4, 8, 1, 1, 1, 2, 2, 1, 1,1,1 ,2, 1, 1, 1, 1, 1, 1 };

    print(&p, size, prompt, 'r');

	fclose(fp);
    return 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：整个函数，只允许出现一次open、一次read、一次write（因为包含错误处理，允许多次fclose）
***************************************************************************/
int modify()
{
    /* 本函数中只允许定义一个 FILE* */
    FILE* fp;

    /* 文件打开，具体要求为：
       1、要求以读写方式打开，打开方式***自行指定
       2、除本次fopen外，本函数其它地方不允许再出现fopen/freopen  */
    fp = fopen("game.dat", "r+b");

    /* 进行后续操作，包括错误处理、读文件、显示各游戏项的值、关闭文件等，允许调用函数
       其中：只允许用一次性读取64字节的方法将game.dat的内容读入***（缓冲区名称、结构体名称自行指定）
                 fread(***, 1, sizeof(demo), fp);
             只允许用一次性写入64字节的方法将***的内容写入game.dat中（缓冲区名称、结构体名称自行指定）
                 fwrite(***, 1, sizeof(demo), fp);
    */
    if (!fp)
    {
        printf("无法打开文件game.dat\n");
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    if (ftell(fp) != 64)
    {
        printf("game.dat大小不是64字节\n");
        return -1;
    }
    fseek(fp, 0, SEEK_SET);

    PLAYER p;
    fread(p.buf, 1, sizeof(p), fp);
    if (p.buf[15] != '\0')
    {
        p.buf[15] = '\0';
        printf("玩家昵称错误，最后一个字节应为尾0，显示在屏幕上的是已人为修正过的\n");
    }

    char prompt[25][20] = { "玩家昵称", "生命", "力量", "体质", "灵巧", "金钱", "名声", "魅力", "游戏累计时间", "移动速度", "攻击速度", "攻击范围", "攻击力", "防御力", "敏捷度", "智力", "经验", "等级", "魔法", "魔法消耗", "魔法伤害力", "魔法命中率", "魔法防御力", "暴击率", "耐力" };
    int size[25] = { 16, 2, 2, 2, 2, 4, 4, 4, 8, 1, 1, 1, 2, 2, 1, 1,1,1 ,2, 1, 1, 1, 1, 1, 1 };

    char cmd;
    char medium;
    while (1)
    {
        print(&p, size, prompt, 'm');

#if (__linux__)       //Linux
        medium=getchar();
#elif (_MSC_VER)       //VS2022
        medium = _getch();
#elif (__clang__)||(__GNUC__)  //DevC++
        medium = getch();
#endif
#if (__linux__)
        if (medium != '\n')
        {
            while (getchar() != '\n')
                ;
        }
#endif

        if (medium >= 'a' && medium <= 'z')
            cmd = medium + ('A' - 'a');
        else
            cmd = medium;

#if (__linux__)
#else
        printf("%c\n", medium);
#endif
        printf("\n");

        if (!((cmd >= 'A' && cmd <= 'Y') || cmd == '0' || cmd == '1'))
            continue;

        if (cmd == '0')
            break;
        else if (cmd == '1')
        {
            fseek(fp, 0, SEEK_SET);
            fwrite(p.buf, 1, sizeof(p), fp);
            break;
        }
        else if (cmd >= 'A' && cmd <= 'Y')
        {
            int displacement = 0; // 表示在buf中的偏移量
            for (int i = 0; i < cmd - 'A'; i++)
                displacement += size[i];

            if (displacement >= 47) // 预留值
                displacement++;

            while (1)
            {
                printf("%s，当前值=", prompt[cmd - 'A']);
                if (cmd == 'A')
                    printf("%s", p.buf);
                else if (size[cmd - 'A'] == 1)
                    printf("%d", (int)*(p.buf + displacement));
                else if (size[cmd - 'A'] == 2)
                    printf("%u", *(unsigned short*)(p.buf + displacement));
                else if (size[cmd - 'A'] == 4)
                    printf("%u", *(unsigned int*)(p.buf + displacement));
                else if (size[cmd - 'A'] == 8)
                    printf("%llu", *(unsigned long long int*)(p.buf + displacement));
                printf("，请输入 :");

                if (deal_input(&p, prompt, cmd, displacement) == 0)
                    break;
                else
                    continue;
            }
            continue;
        }
    }

	fclose(fp);
    return 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：main函数允许带参数，不允许进行文件读写
***************************************************************************/
int main(int argc, char** argv)
{
    //modify();
    if (argc == 1)
    {
        usage(argv[0]);
        return -1;
    }

    if (strcmp(argv[1], "--read") == 0)
        read();
    else if (strcmp(argv[1], "--modify") == 0)
        modify();

    return 0;
}