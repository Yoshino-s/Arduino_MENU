#include <Wire.h>

#include "U8glib.h"


U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);//oled��������

void menuDrawerSetup() {
	//I2C���߿��ʼ��
	Wire.begin();

	u8g.setFont(u8g_font_chikitar);
	u8g.firstPage();
	do {
		u8g.setPrintPos(0, 10);
		u8g.print("Initialize...");
	} while (u8g.nextPage());

}

/**
���Ƶ����˵���

@var struct mymenu item Ҫ���Ƶ���Ŀ
@var int row �������꣨LCD5110��������ʱֻ��һ�������꼴�ɣ��ɸ���ʵ����ʾ�豸������
@var bool rev �Ƿ�����ƣ�Ĭ��Ϊ��������ƣ����ڵ�ǰѡ������ƣ��ɸ���ʵ����ʾ�豸������
*/
void renderItem(struct mymenu item, int row, bool rev = false)
{
	char label[15];
	//�淶��ʾ��Ŀ���֣�����룬���ո�
	sprintf(label, "%-14s", item.label);

	//���Ʋ˵���
	// ��5110�ķ�����ƹ���
	u8g.setDefaultForegroundColor();




	if (rev) {
		u8g.drawBox(0, row, 120, 10);
		u8g.setDefaultBackgroundColor();
	}
	u8g.setPrintPos(0, row + 10);
	u8g.print(label);
}

/**
����ĳһ���˵�

@var struct mymenu* items ��Ҫ���ƵĲ˵�
@var int menu_num �˵���Ŀ����
*/
void renderMenu(struct mymenu *items, int menu_num)
{
	//�����������ܳ���ÿһ��������������
	int num = menu_num > MENU_MAX_ROW ? MENU_MAX_ROW : menu_num;
	u8g.firstPage();
	do {
		for (int i = 0; i < num; i++) {
			//����ÿ���˵���
			renderItem(items[i + start_index], i * 8, i == item_index ? true : false);
		}
	} while (u8g.nextPage());
}

void menuDrawerLoop() {
	//��ȡ����ʱ��
	unsigned long endtime = millis();
	//��ȡ��ǰ�˵�����Ŀ����
	int menu_num = cur_item[0].num;
	//����һ����ʱ����
	int idx;

	//���Ʋ˵�
	renderMenu(&cur_item[0], menu_num > MENU_MAX_ROW ? MENU_MAX_ROW : menu_num);

	//��ȡ����
	int k_down = digitalRead(KEY_DOWN);
	int k_up = digitalRead(KEY_UP);
	int k_enter = digitalRead(KEY_ENTER);
	int k_exit = digitalRead(KEY_EXIT);


	//��ⰴ����������Ӧ����
	if (k_up == LOW && k_up != old_up_stat) {
		//����
		delay(KEY_TREMBLE_TIME);
		if (k_up == LOW) {
			item_index--;
		}
	}
	else if (k_down == LOW && k_down != old_down_stat) {
		//����
		delay(KEY_TREMBLE_TIME);
		if (k_down == LOW) {
			item_index++;
		}
	}
	else if (k_enter == LOW && k_enter != old_enter_stat) {
		//����
		delay(KEY_TREMBLE_TIME);
		if (k_enter == LOW) {
			//�����ʱ�Ĳ˵�������ֵ
			idx = start_index + item_index;
			if (cur_item[idx].next != NULL && cur_item[idx].type == MENU_SUBMENU) {
				//��������˵���˲˵���ָ������һ���˵�
				//�˼��˵��������һ���˵�
				prev_item = cur_item;
				//��ָ�����һ���˵�����Ϊ��ǰ�˵�
				cur_item = cur_item[idx].next;
				//���ò˵��������ͻ�������
				item_index = 0;
				start_index = 0;
				//����

			}
			else if (cur_item[idx].action != NULL && cur_item[idx].type != MENU_PARAM) {
				//��������˵������˵��Ƕ���
				//�˼��˵������һ���˵�
				prev_item = cur_item;
				//���ݶ������͵�����Ӧ����һ���˵�
				switch (cur_item[idx].type) {
				case MENU_ACTION_LOGIC:
					//�������������ݸ��߼��˵���ʹ�߼��˵��ܹ���ȷִ�ж���
					logic_menu[0].action = cur_item[idx].action;
					//���õ�ǰ�˵�Ϊ�߼��˵�
					cur_item = logic_menu;
					//���ò˵��������ͻ�������
					item_index = 0;
					start_index = 0;
					break;
				case MENU_ACTION_NUMBER:
					break;
				case MENU_ACTION_STRING:
					break;
				default:
					break;
				}
				//����

			}
			else if (cur_item[idx].type == MENU_PARAM) {
				//��������˵������ִ�ж���
				//������Ӧ�Ķ��������������ݲ���������ֻ�����߼����ã�
				cur_item[0].action((const char *)cur_item[idx].label);
			}
		}
	}
	else if (k_exit == LOW && k_exit != old_exit_stat) {
		//����
		delay(KEY_TREMBLE_TIME);
		if (k_exit == LOW) {
			//������һ���˵��Ĳ���
			if (prev_item != NULL) {
				//������һ���˵�Ϊ��ǰ�˵�
				cur_item = prev_item;
				//���õ�ǰ�˵�����һ���˵�
				prev_item = cur_item[0].prev;
				//���ò˵��������ͻ�������
				item_index = 0;
				start_index = 0;
				//����

			}
		}
	}

	/**
	�˵�������ѡ���Ƿ�Խ��
	*/
	if (item_index < 0) {
		item_index = 0;
		start_index--;
		if (start_index < 0) start_index = 0;
	}
	if (item_index >= MENU_MAX_ROW || item_index >= menu_num) {
		if (item_index >= menu_num) item_index = menu_num - 1;
		if (item_index >= MENU_MAX_ROW) item_index = MENU_MAX_ROW - 1;
		if (start_index + MENU_MAX_ROW < menu_num) start_index++;
	}

	//���水��״̬
	old_up_stat = digitalRead(KEY_UP);
	old_down_stat = digitalRead(KEY_DOWN);
	old_enter_stat = digitalRead(KEY_ENTER);
	old_exit_stat = digitalRead(KEY_EXIT);
}



