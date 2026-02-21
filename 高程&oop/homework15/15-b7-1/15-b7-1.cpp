/* 2350222 盛曦 计科 */
#include <iostream>
#include <fstream>
//根据需要可加入其它头文件
#include <iomanip>
#if (__linux__)   
#include <cstring>
#else
#include <conio.h>
#endif
using namespace std;

//此处为示例，允许修改结构体名称，允许修改结构体中的成员内容，要求sizeof必须是64
//typedef struct player {
//    unsigned long long int play_time;         //37-44：游戏累计时间 (0-1000000000000)
//    unsigned int money;             //25-28：金钱数量 (0-100000000)
//    unsigned int fame;              //29-32：名声值 (0-100000000)
//    unsigned int charm;           //33-36：魅力值 (0-1000000)
//    unsigned short HP;              //17-18：生命值 (0-10000)
//    unsigned short strength;        //19-20：力量值 (0-10000)
//    unsigned short physique;        //21-22：体质 (0-8192)
//    unsigned short agility;         //23-24：灵巧 (0-1024)
//    unsigned short attack_power;    //49-50：攻击力 (0-2000)
//    unsigned short defense_power;   //51-52：防御力 (0-2000)
//    unsigned short MP;              //57-58：魔法值 (0-10000)
//    unsigned char move_speed;       //45：移动速度 (0-100)
//    unsigned char attack_speed;     //46：攻击速度 (0-100)
//    unsigned char attack_range;     //47：攻击范围 (0-100)
//	unsigned char empty;            //48：预留值，未使用
//    unsigned char dexterity;        //53：敏捷度 (0-100)
//    unsigned char intelligence;     //54：智力 (0-100)
//    unsigned char exp;              //55：经验 (0-100)
//    unsigned char level;            //56：等级 (0-100)
//    unsigned char MP_consumption;   //59：使用魔法时每次消耗的魔法值 (0-100)
//    unsigned char magic_damage;     //60：魔法伤害 (0-100)
//    unsigned char hit_rate;         //61：命中率 (0-100)
//    unsigned char magic_defense;    //62：魔法防御力 (0-100)
//    unsigned char critical_rate;    //63：暴击率 (0-100)
//    unsigned char stamina;          //64：耐力 (0-100)
//    char name[16];                  //玩家昵称，最多 15 个字符
//}PLAYER;
typedef struct player {
    char buf[64];
}PLAYER;

/* 此处允许新增函数，数量不限
   1、所有新增的函数，均不允许定义新的 fstream / ifstream / ofstream 流对象，并进行打开/读/写/关闭等操作
   2、所有新增的函数，均不允许用C方式进行文件处理
   3、上述两个限制同样适用于main函数
*/

int usage(char* proname)
{
	cout << "usage : " << proname << " --modify | --read" << endl;
	return 0;
}

//int assignment(PLAYER* p, char* buf)
//{
//    memcpy(p->name, buf, 15);
//    p->name[15] = '\0';
//    p->HP = *(unsigned short*)(buf + 16);
//    p->strength = *(unsigned short*)(buf + 18);
//    p->physique = *(unsigned short*)(buf + 20);
//    p->agility = *(unsigned short*)(buf + 22);
//    p->money = *(unsigned int*)(buf + 24);
//    p->fame = *(unsigned int*)(buf + 28);
//    p->charm = *(unsigned int*)(buf + 32);
//    p->play_time = *(unsigned long long int*)(buf + 36);
//    p->move_speed = *(unsigned char*)(buf + 44);
//    p->attack_speed = *(unsigned char*)(buf + 45);
//    p->attack_range = *(unsigned char*)(buf + 46);
//    p->empty = *(unsigned char*)(buf + 47);
//    p->attack_power = *(unsigned short*)(buf + 48);
//    p->defense_power = *(unsigned short*)(buf + 50);
//    p->dexterity = *(unsigned char*)(buf + 52);
//    p->intelligence = *(unsigned char*)(buf + 53);
//    p->exp = *(unsigned char*)(buf + 54);
//    p->level = *(unsigned char*)(buf + 55);
//    p->MP = *(unsigned short*)(buf + 56);
//    p->MP_consumption = *(unsigned char*)(buf + 58);
//    p->magic_damage = *(unsigned char*)(buf + 59);
//    p->hit_rate = *(unsigned char*)(buf + 60);
//    p->magic_defense = *(unsigned char*)(buf + 61);
//    p->critical_rate = *(unsigned char*)(buf + 62);
//    p->stamina = *(unsigned char*)(buf + 63);
//    return 0;
//}

int print(PLAYER *p,int* size,char (*prompt)[20],char r_m)
{
    if(r_m=='m')
    {
        cout << "--------------------------------------" << endl;
        cout << "游戏存档文件修改工具" << endl;
        cout << "--------------------------------------" << endl;
    }
    for (int i = 0; i < 25; i++)
    {
        int displacement = 0;//表示在buf中的偏移量
        for (int j = 0; j < i; j++)
            displacement += size[j];

        if (displacement >= 47)//预留值
            displacement++;

        if(r_m=='r')
        {
            cout << setw(18) << prompt[i] << "：";
            if (i == 0)
                cout << p->buf;//第16个是\0
            else if (size[i] == 1)
                cout << (int)*(unsigned char*)(p->buf + displacement);
            else if (size[i] == 2)
                cout << *(unsigned short*)(p->buf + displacement);
            else if (size[i] == 4)
                cout << *(unsigned int*)(p->buf + displacement);
            else if (size[i] == 8)
                cout << *(unsigned long long int*)(p->buf + displacement);
        }
        else
        {
            cout << "  " << char('a' + i) << '.' << prompt[i];
            if (16 - strlen(prompt[i]) - 4!= 0)
                cout << setw(16 - strlen(prompt[i]) - 4) << ' ';
            if (i == 0)
                cout << '(' << p->buf << ')';
            else if (size[i] == 1)
                cout << '('<<(int)*(unsigned char*)(p->buf + displacement)<<')';
            else if (size[i] == 2)
                cout << '(' << *(unsigned short*)(p->buf + displacement) << ')';
            else if (size[i] == 4)
                cout << '(' << *(unsigned int*)(p->buf + displacement) << ')';
            else if (size[i] == 8)
                cout << '(' << *(unsigned long long int*)(p->buf + displacement) << ')';
        }

        cout << endl;
    }
   /* cout << setw(16) << "  a.玩家昵称：" << "("<<p->name << ")"<<endl;
    cout << setw(16) << "  b.生命值：" << "(" << p->HP << ")" << endl;
    cout << setw(16) << "  c.力量值：" << "(" << p->strength << ")" << endl;
    cout << setw(16) << "  d.体质值：" << "(" << p->physique << ")" << endl;
    cout << setw(16) << "  e.灵巧值：" << "(" << p->agility << ")" << endl;
    cout << setw(16) << "  f.金钱值：" << "(" << p->money << ")" << endl;
    cout << setw(16) << "  g.名声值：" << "(" << p->fame << ")"<<endl;
    cout << setw(16) << "  h.魅力值：" << "(" << p->charm << ")"<<endl;
    cout << setw(16) << "  i.游戏累计时间(us)值：" << "(" << p->play_time << ")"<<endl;
    cout << setw(16) << "  j.移动速度值：" << "(" << (int)p->move_speed << ")"<<endl;
    cout << setw(16) << "  k.攻击速度值：" << "(" << (int)p->attack_speed << ")"<<endl;
    cout << setw(16) << "  l.攻击范围值：" << "(" << (int)p->attack_range << ")"<<endl;
    cout << setw(16) << "  m.攻击力值：" << "(" << p->attack_power << ")"<<endl;
    cout << setw(16) << "  n.防御力值：" << "(" << p->defense_power << ")"<<endl;
    cout << setw(16) << "  o.敏捷度值：" << "(" << (int)p->dexterity << ")"<<endl;
    cout << setw(16) << "  p.智力值：" << "(" << (int)p->intelligence << ")"<<endl;
    cout << setw(16) << "  q.经验值：" << "(" << (int)p->exp << ")"<<endl;
    cout << setw(16) << "  r.等级值：" << "(" << (int)p->level << ")"<<endl;
    cout << setw(16) << "  s.魔法值：" << "(" << p->MP << ")"<<endl;
    cout << setw(16) << "  t.消耗魔法值：" << "(" << (int)p->MP_consumption << ")"<<endl;
    cout << setw(16) << "  u.魔法伤害力值：" << "(" << (int)p->magic_damage << ")"<<endl;
    cout << setw(16) << "  v.命中率值：" << "(" << (int)p->hit_rate << ")"<<endl;
    cout << setw(16) << "  w.魔法防御力值：" << "(" << (int)p->magic_defense << ")"<<endl;
    cout << setw(16) << "  x.暴击率值：" << "(" << (int)p->critical_rate << ")"<<endl;
    cout << setw(16) << "  y.耐力值：" << "(" << (int)p->stamina << ")"<<endl;*/
    if(r_m=='m')
    {
        cout << "--------------------------------------" << endl;
        cout << "  0.放弃修改" << endl;
        cout << "  1.存盘退出" << endl;
        cout << "--------------------------------------" << endl;
        cout << "请选择[a..y, 0..1]";
    }
    return 0;
}

template <typename T>
T input(char* prompt, T min, T max)
{
    T temp;
    cin >> temp;
    if (cin.fail() == 1)
    {
        cin.clear();
        cin.ignore(1410065407, '\n');     
        return -1;
    }
    if (temp < min || temp>max)
    {
        cout << "非法的" << prompt << "值" << endl;
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
        if (i == 15)//没有读到回车，需要清空缓冲区
            while (getchar() != '\n')
                ;

		for (; i <= 15; i++)
            p->buf[i] = '\0';
        return 0;
    }
    else if (cmd == 'B' || cmd == 'C' || cmd == 'S')
    {
        int t;
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
        int t;
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
        int t;
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
        int t;
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
        int t;
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
        int t;
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
        int t;
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
  说    明：整个函数，只允许出现一次open、一次read（因为包含错误处理，允许多次close）
***************************************************************************/
int read()
{
    /* 本函数中只允许定义一个 ifstream流对象，不再允许定义任何形式的fstream/ifstream/ofstream流对象，也不允许使用C方式的文件处理 */
    ifstream gfile;

    /* 文件打开，具体要求为：
       1、要求以读方式打开，打开方式***自行指定
       2、除本次open外，本函数其它地方不允许再出现open  */
    gfile.open("game.dat", ios::in | ios::binary);
	
    /* 进行后续操作，包括错误处理、读文件、显示各游戏项的值、关闭文件等，允许调用函数
       其中：只允许用一次性读取64字节的方法将game.dat的内容读入***（缓冲区名称、结构体名称自行指定）
                 gfile.read(***, sizeof(demo));
    */
    if (!gfile)
    {
        cout << "无法打开文件game.dat" << endl;
        return -1;
    }
    gfile.seekg(0, ios::end);
    if (gfile.tellg() != 64)
    {
        cout << "game.dat大小不是64字节" << endl;
        return -1;
    }
    if (!gfile.good())
        gfile.clear();
    gfile.seekg(0, ios::beg);

    PLAYER p;
	gfile.read(p.buf, sizeof(p));
    if (p.buf[15] != '\0')
    {
        printf("玩家昵称错误，最后一个字节应为尾0，显示在屏幕上的是已人为修正过的\n");
        p.buf[15] = '\0';
    }
	//assignment(&p, buf);

    char prompt[26][20] = { "玩家昵称", "生命值", "力量值", "体质值", "灵巧值", "金钱值", "名声值", "魅力值", "游戏累计时间(us)值", "移动速度值", "攻击速度值", "攻击范围值", "攻击力值", "防御力值", "敏捷度值", "智力值", "经验值", "等级值", "魔法值", "消耗魔法值", "魔法伤害力值", "命中率值", "魔法防御力值", "暴击率值", "耐力值" };
    int size[26] = { 16, 2, 2, 2, 2, 4, 4, 4, 8, 1, 1, 1, 2, 2, 1, 1,1,1 ,2, 1, 1, 1, 1, 1, 1 };
    
	print(&p, size, prompt, 'r');
	
	gfile.close();
    return 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：整个函数，只允许出现一次open、一次read、一次write（因为包含错误处理，允许多次close）
***************************************************************************/
int modify()
{
    /* 本函数中只允许定义一个 fstream流对象，不再允许定义任何形式的fstream/ifstream/ofstream流对象，也不允许使用C方式的文件处理 */
    fstream gfile;

    /* 文件打开，具体要求为：
       1、要求以读写方式打开，打开方式***自行指定
       2、除本次open外，本函数其它地方不允许再出现open  */
    gfile.open("game.dat", ios::in | ios::out | ios::binary);
    
    /* 进行后续操作，包括错误处理、读文件、显示各游戏项的值、关闭文件等，允许调用函数
       其中：只允许用一次性读取64字节的方法将game.dat的内容读入***（缓冲区名称、结构体名称自行指定）
                 gfile.read(***, sizeof(demo));
             只允许用一次性写入64字节的方法将***的内容写入game.dat中（缓冲区名称、结构体名称自行指定）
                 gfile.write(***, sizeof(demo));
    */
    if (!gfile)
    {
        cout << "无法打开文件game.dat" << endl;
        return -1;
    }
    gfile.seekg(0, ios::end);
    if (gfile.tellg() != 64)
    {
        cout << "game.dat大小不是64字节" << endl;
        return -1;
    }
    if (!gfile.good())
        gfile.clear();
    gfile.seekg(0, ios::beg);

    PLAYER p;
    gfile.read(p.buf, sizeof(p));
    if (p.buf[15] != '\0')
    {
        printf("玩家昵称错误，最后一个字节应为尾0，显示在屏幕上的是已人为修正过的\n");
        p.buf[15] = '\0';
    }
	//assignment(&p, buf);

	char prompt[25][20] = { "玩家昵称", "生命", "力量", "体质", "灵巧", "金钱", "名声", "魅力", "游戏累计时间", "移动速度", "攻击速度", "攻击范围", "攻击力", "防御力", "敏捷度", "智力", "经验", "等级", "魔法", "魔法消耗", "魔法伤害力", "魔法命中率", "魔法防御力", "暴击率", "耐力" };
	int size[25] = { 16, 2, 2, 2, 2, 4, 4, 4, 8, 1, 1, 1, 2, 2, 1, 1,1,1 ,2, 1, 1, 1, 1, 1, 1 };

    char cmd;
    char medium;
    while (1)
    {
        print(&p,size,prompt,'m');

#if (__linux__)       //Linux
        medium = getchar();
#elif (_MSC_VER)       //VS2022
        medium=_getch();
#elif (__clang__)||(__GNUC__)  //DevC++
        medium=getch();
#endif
#if (__linux__)
        if(medium!='\n')
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
        cout << medium << endl;
#endif
        cout << endl;

        if (!((cmd >= 'A' && cmd <= 'Y') || cmd == '0' || cmd == '1'))
            continue;

        if (cmd == '0')
            break;
        else if (cmd == '1')
        {
            gfile.seekg(0, ios::beg);
            gfile.write(p.buf, sizeof(p));
            break;
        }
        else if (cmd >= 'A' && cmd <= 'Y')
        {
            int displacement = 0;//表示在buf中的偏移量
            for (int i = 0; i < cmd - 'A'; i++)
                displacement += size[i];

            if (displacement >= 47)//预留值
                displacement++;

            while(1)
            {
                cout << prompt[cmd - 'A'] << "，当前值=";
                if (cmd == 'A')
                    cout << p.buf;
                else if (size[cmd - 'A'] == 1)
                    cout << (int)*(p.buf + displacement);
                else if (size[cmd - 'A'] == 2)
                    cout << *(unsigned short*)(p.buf + displacement);
                else if (size[cmd - 'A'] == 4)
                    cout << *(unsigned int*)(p.buf + displacement);
                else if (size[cmd - 'A'] == 8)
                    cout << *(unsigned long long int*)(p.buf + displacement);
                cout << "，请输入 :";

                if (deal_input(&p,prompt,cmd,displacement) == 0)
                    break;
                else
                    continue;
            }
            continue;
        }
    }

	gfile.close();
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
    if (sizeof(player) != 64)
    {
		cout << "结构体大小为"<< sizeof(player)<<", 不是64字节" << endl;
		return -1;
    }

    if(argc==1)
	{
		usage(argv[0]);
		return -1;
	}

    if (strcmp(argv[1],"--read")==0)
            read();
    else if(strcmp(argv[1],"--modify")==0)
            modify();

    return 0;
}