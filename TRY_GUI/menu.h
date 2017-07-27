
// 菜单最大显示行数
#define MENU_MAX_ROW 5

/**
以下定义菜单项类型
*/
// 具有子菜单的菜单项
#define MENU_SUBMENU 0
// 参数项（用于执行参数设置）
#define MENU_PARAM 1
// 无动作的菜单项
#define MENU_ACTION_NONE 101
// 执行逻辑参数设置的菜单项（开/关、真/假等设置）
#define MENU_ACTION_LOGIC 102
// 执行数值调整设置的菜单项（时间、音量等设置）
#define MENU_ACTION_NUMBER 103
// 执行字符串设置的菜单项（欢迎语、LED显示文字等设置）
#define MENU_ACTION_STRING 104

//字符串的定义
char maintime[6] = "13:14";
char temp[6] = "Temp:";
char humi[6] = "Humi;";

/**
以下定义按键引脚
设置上拉电阻，低电平有效
*/
#define KEY_UP 7
#define KEY_DOWN 6
#define KEY_ENTER 5
#define KEY_EXIT 4

// 定义按键消抖延时时间
#define KEY_TREMBLE_TIME 20

/**
菜单结构
一个数组既代表一级菜单，数组中的一个元素就是一个菜单项，就是一个单独的结构体，
数组中的第一个元素中的num值很重要，表示本级菜单具有多少个菜单项。

@var int num 本级菜单数量，必须在第一项中设置正确的菜单项数量
@var char* label 菜单文本
@var int type 此项类型，参考宏定义
@var void (*action)(const char *) 指向动作的函数指针，此项要执行的具体动作函数
@var mymenu* next 下一级菜单，只需在菜单的第一项设置，其他项置空（NULL）即可
@var mymenu* prev 上一级菜单，同上
*/
struct mymenu {
	int num;
	char *label; //display label
	int type; //0:item, 1:submenu, 2:param
	void(*action)(const char *);
	mymenu * next;
	mymenu * prev;
};

/**
逻辑参数设置菜单
逻辑菜单表示菜单项中的action函数要执行逻辑设置动作，即设置某个参数的逻辑值
逻辑菜单只需两个菜单项代表true和false
需要遵守的规则为：菜单中的文本需要设置为“ON”和“OFF”，
在执行动作函数的时候，可以将正确的参数传递过去，
动作函数的规则参照函数说明
*/
mymenu logic_menu[2] = {
	{ 2, "ON", MENU_PARAM, NULL, NULL, NULL },
	{ 2, "OFF", MENU_PARAM, NULL, NULL, NULL }
};

/**
下面定义了三级菜单说明了菜单应该如何个定义
*/
// 第二级菜单
mymenu light_menu[3] = {
	//第一项什么也不做，所以设置了类型为MENU_ACTION_NONE
	{ 3, "Band", MENU_ACTION_NONE, NULL, NULL, NULL },
	//第二项指向了下级菜单，所以设置了类型为MENU_SUBMENU
	{ 3, "Device1", MENU_SUBMENU, NULL, NULL, NULL },
	//第一项什么也不做，所以设置了类型为MENU_ACTION_NONE
	{ 3, "Device2", MENU_ACTION_NONE, NULL, NULL, NULL }
};
// 第三级菜单
mymenu test_level3_menu[2] = {
	{ 7, "level3menu-4", MENU_ACTION_NONE, NULL, NULL, NULL },
	//定义了一个逻辑动作，这里是控制PIN13脚的LED
	{ 7, "LED TEST", MENU_ACTION_LOGIC, NULL, NULL, NULL }
};
// 第一级菜单
mymenu main_menu[4] = {
	{ 4, "item 1", MENU_ACTION_NONE, NULL, NULL, NULL },
	{ 4, "item 2", MENU_ACTION_NONE, NULL, NULL, NULL },
	{ 4, "item 3", MENU_ACTION_NONE, NULL, NULL, NULL },
	//指向下一级菜单
	{ 4, "Next Page", MENU_SUBMENU, NULL, NULL, NULL }

};

// 定义菜单操作需要的全局变量
// 分别为当前菜单、上一级菜单、当前菜单项索引和开始显示的菜单项索引
mymenu *cur_item, *prev_item;
int item_index, start_index;

// 定义菜单的操作按键（上、下、进入和返回）状态变量
bool old_up_stat, old_down_stat, old_enter_stat, old_exit_stat;

void menuSetup() {
	/**
	菜单的进一步设置
	在这里将每一个菜单的关联设置好
	对照每一个初始设置仔细填写它们之间的关系
	*/
	//第一级（main_menu）的第三项指向了下一级菜单（light_menu)
	main_menu[3].next = light_menu;

	//第二级（light_menu）的上一级（main_menu）
	light_menu[0].prev = main_menu;

	//第二级（light_menu）的第二项指向了下一级菜单（test_level3_menu）
	light_menu[1].next = test_level3_menu;

	//第三级（test_level3_menu）的上一级（light_menu）
	test_level3_menu[0].prev = light_menu;

	//第三级（test_level3_menu）的第五项定义了个逻辑动作
	test_level3_menu[1].action = nrfsent;



	/**
	初始化当前菜单为第一级（main_menu）
	*/
	cur_item = main_menu;
	/**
	上一级菜单为空
	*/
	prev_item = NULL;
	/**
	当前选择了第一项
	*/
	item_index = 0;
	/**
	从第一项开始显示菜单
	*/
	start_index = 0;

	//一些状态变量的初始化
	old_up_stat = HIGH;
	old_down_stat = HIGH;
	old_enter_stat = HIGH;
	old_exit_stat = HIGH;

	//为了视频中能看清楚，延时定义了3秒
	delay(1000);
}

/*菜单项标题的操作*/
void title()
{
	float h = DHT11.humidity; //读湿度
	float t = DHT11.temperature - 2; //读温度，默认为摄氏度
	char hu[16] = { 0 };
	sprintf(hu, "%.2f", h);
	strcat(humi, hu);
	main_menu[2].label = humi;
	char te[16] = { 0 };

	sprintf(te, "%.2f", t);
	strcat(temp, te);
	main_menu[1].label = temp;

}

void menuLoop() {
	title();
}

