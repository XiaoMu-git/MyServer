#include <stdio.h>
#include <mysql.h>

int main() {
	MYSQL mysql;	// 数据库句柄
	MYSQL_RES* res;	// 查询结果
	MYSQL_ROW row;	// 记录结构体

	// 初始化数据库
	mysql_init(&mysql);

	// 设置字符编码
	mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");

	// 连接数据库
	if (mysql_real_connect(&mysql, "127.0.0.1", "root", "123/liumx", "xiaomu", 3306, nullptr, 0) == nullptr) {
		printf("连接失败！\n");
		printf("错误原因:%s\n", mysql_error(&mysql));
		return 0;
	}

	// 查询数据
	int ret = mysql_query(&mysql, "select * from emp;");
	printf("查询到数据:%d\n", ret);

	// 获取结果集
	res = mysql_store_result(&mysql);

	// 给 row 赋值
	while (row = mysql_fetch_row(res)) {
		for (int i = 0; i < mysql_num_fields(res); i++) printf("%s ", row[i]);
		printf("\n");
	}

	// 释放结果集
	mysql_free_result(res);

	// 关闭数据库
	mysql_close(&mysql);

	system("pause");
	return 0;
}