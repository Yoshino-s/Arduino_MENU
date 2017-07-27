
// �˵������ʾ����
#define MENU_MAX_ROW 5

/**
���¶���˵�������
*/
// �����Ӳ˵��Ĳ˵���
#define MENU_SUBMENU 0
// ���������ִ�в������ã�
#define MENU_PARAM 1
// �޶����Ĳ˵���
#define MENU_ACTION_NONE 101
// ִ���߼��������õĲ˵����/�ء���/�ٵ����ã�
#define MENU_ACTION_LOGIC 102
// ִ����ֵ�������õĲ˵��ʱ�䡢���������ã�
#define MENU_ACTION_NUMBER 103
// ִ���ַ������õĲ˵����ӭ�LED��ʾ���ֵ����ã�
#define MENU_ACTION_STRING 104

//�ַ����Ķ���
char maintime[6] = "13:14";
char temp[6] = "Temp:";
char humi[6] = "Humi;";

/**
���¶��尴������
�����������裬�͵�ƽ��Ч
*/
#define KEY_UP 7
#define KEY_DOWN 6
#define KEY_ENTER 5
#define KEY_EXIT 4

// ���尴��������ʱʱ��
#define KEY_TREMBLE_TIME 20

/**
�˵��ṹ
һ������ȴ���һ���˵��������е�һ��Ԫ�ؾ���һ���˵������һ�������Ľṹ�壬
�����еĵ�һ��Ԫ���е�numֵ����Ҫ����ʾ�����˵����ж��ٸ��˵��

@var int num �����˵������������ڵ�һ����������ȷ�Ĳ˵�������
@var char* label �˵��ı�
@var int type �������ͣ��ο��궨��
@var void (*action)(const char *) ָ�����ĺ���ָ�룬����Ҫִ�еľ��嶯������
@var mymenu* next ��һ���˵���ֻ���ڲ˵��ĵ�һ�����ã��������ÿգ�NULL������
@var mymenu* prev ��һ���˵���ͬ��
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
�߼��������ò˵�
�߼��˵���ʾ�˵����е�action����Ҫִ���߼����ö�����������ĳ���������߼�ֵ
�߼��˵�ֻ�������˵������true��false
��Ҫ���صĹ���Ϊ���˵��е��ı���Ҫ����Ϊ��ON���͡�OFF����
��ִ�ж���������ʱ�򣬿��Խ���ȷ�Ĳ������ݹ�ȥ��
���������Ĺ�����պ���˵��
*/
mymenu logic_menu[2] = {
	{ 2, "ON", MENU_PARAM, NULL, NULL, NULL },
	{ 2, "OFF", MENU_PARAM, NULL, NULL, NULL }
};

/**
���涨���������˵�˵���˲˵�Ӧ����θ�����
*/
// �ڶ����˵�
mymenu light_menu[3] = {
	//��һ��ʲôҲ��������������������ΪMENU_ACTION_NONE
	{ 3, "Band", MENU_ACTION_NONE, NULL, NULL, NULL },
	//�ڶ���ָ�����¼��˵�����������������ΪMENU_SUBMENU
	{ 3, "Device1", MENU_SUBMENU, NULL, NULL, NULL },
	//��һ��ʲôҲ��������������������ΪMENU_ACTION_NONE
	{ 3, "Device2", MENU_ACTION_NONE, NULL, NULL, NULL }
};
// �������˵�
mymenu test_level3_menu[2] = {
	{ 7, "level3menu-4", MENU_ACTION_NONE, NULL, NULL, NULL },
	//������һ���߼������������ǿ���PIN13�ŵ�LED
	{ 7, "LED TEST", MENU_ACTION_LOGIC, NULL, NULL, NULL }
};
// ��һ���˵�
mymenu main_menu[4] = {
	{ 4, "item 1", MENU_ACTION_NONE, NULL, NULL, NULL },
	{ 4, "item 2", MENU_ACTION_NONE, NULL, NULL, NULL },
	{ 4, "item 3", MENU_ACTION_NONE, NULL, NULL, NULL },
	//ָ����һ���˵�
	{ 4, "Next Page", MENU_SUBMENU, NULL, NULL, NULL }

};

// ����˵�������Ҫ��ȫ�ֱ���
// �ֱ�Ϊ��ǰ�˵�����һ���˵�����ǰ�˵��������Ϳ�ʼ��ʾ�Ĳ˵�������
mymenu *cur_item, *prev_item;
int item_index, start_index;

// ����˵��Ĳ����������ϡ��¡�����ͷ��أ�״̬����
bool old_up_stat, old_down_stat, old_enter_stat, old_exit_stat;

void menuSetup() {
	/**
	�˵��Ľ�һ������
	�����ｫÿһ���˵��Ĺ������ú�
	����ÿһ����ʼ������ϸ��д����֮��Ĺ�ϵ
	*/
	//��һ����main_menu���ĵ�����ָ������һ���˵���light_menu)
	main_menu[3].next = light_menu;

	//�ڶ�����light_menu������һ����main_menu��
	light_menu[0].prev = main_menu;

	//�ڶ�����light_menu���ĵڶ���ָ������һ���˵���test_level3_menu��
	light_menu[1].next = test_level3_menu;

	//��������test_level3_menu������һ����light_menu��
	test_level3_menu[0].prev = light_menu;

	//��������test_level3_menu���ĵ�������˸��߼�����
	test_level3_menu[1].action = nrfsent;



	/**
	��ʼ����ǰ�˵�Ϊ��һ����main_menu��
	*/
	cur_item = main_menu;
	/**
	��һ���˵�Ϊ��
	*/
	prev_item = NULL;
	/**
	��ǰѡ���˵�һ��
	*/
	item_index = 0;
	/**
	�ӵ�һ�ʼ��ʾ�˵�
	*/
	start_index = 0;

	//һЩ״̬�����ĳ�ʼ��
	old_up_stat = HIGH;
	old_down_stat = HIGH;
	old_enter_stat = HIGH;
	old_exit_stat = HIGH;

	//Ϊ����Ƶ���ܿ��������ʱ������3��
	delay(1000);
}

/*�˵������Ĳ���*/
void title()
{
	float h = DHT11.humidity; //��ʪ��
	float t = DHT11.temperature - 2; //���¶ȣ�Ĭ��Ϊ���϶�
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

