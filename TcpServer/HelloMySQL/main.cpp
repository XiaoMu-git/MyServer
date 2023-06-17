#include <stdio.h>
#include <mysql.h>

int main() {
	MYSQL mysql;	// ���ݿ���
	MYSQL_RES* res;	// ��ѯ���
	MYSQL_ROW row;	// ��¼�ṹ��

	// ��ʼ�����ݿ�
	mysql_init(&mysql);

	// �����ַ�����
	mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");

	// �������ݿ�
	if (mysql_real_connect(&mysql, "127.0.0.1", "root", "123/liumx", "xiaomu", 3306, nullptr, 0) == nullptr) {
		printf("����ʧ�ܣ�\n");
		printf("����ԭ��:%s\n", mysql_error(&mysql));
		return 0;
	}

	// ��ѯ����
	int ret = mysql_query(&mysql, "select * from emp;");
	printf("��ѯ������:%d\n", ret);

	// ��ȡ�����
	res = mysql_store_result(&mysql);

	// �� row ��ֵ
	while (row = mysql_fetch_row(res)) {
		for (int i = 0; i < mysql_num_fields(res); i++) printf("%s ", row[i]);
		printf("\n");
	}

	// �ͷŽ����
	mysql_free_result(res);

	// �ر����ݿ�
	mysql_close(&mysql);

	system("pause");
	return 0;
}