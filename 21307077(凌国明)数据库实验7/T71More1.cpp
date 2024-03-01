#include <windows.h>
#include <stdio.h>
#include <sql.h>
#include <sqlext.h>
int copyTableByODBC(char *src_dsn,char *src_user_id,char *src_user_password,
			char *target_dsn,char *target_user_id,char *target_user_password);

int main() {
	copyTableByODBC("university_in_mysql80","root","Ling.0304","university_in_sqlserver","sa","Ling.0304");
    return 0;
}

int copyTableByODBC(char *src_dsn,char *src_user_id,char *src_user_password,
					char *target_dsn,char *target_user_id,char *target_user_password)
{
    SQLRETURN ret; //调用结果
	/*Step 1：定义句柄和变量*/
    SQLHENV src_env;  //环境句柄
    SQLHENV target_env;
    SQLHDBC src_dbc;  //连接句柄 
    SQLHDBC target_dbc;
    SQLHSTMT src_stmt; //语句句柄 
    SQLHSTMT target_stmt; 

	/*Step 2：初始化环境*/
    SQLAllocEnv(&src_env);
    SQLAllocEnv(&target_env);
    //设置管理环境的属性
	SQLSetEnvAttr(src_env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
	SQLSetEnvAttr(target_env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
   
	/*Step 3：建立连接*/
    //分配连接句柄
	ret = SQLAllocConnect(src_env, &src_dbc);
	ret = SQLAllocConnect(target_env, &target_dbc);

    ret = SQLConnect(src_dbc, (SQLCHAR *)src_dsn, SQL_NTS, (SQLCHAR *)src_user_id, SQL_NTS, (SQLCHAR *)src_user_password, SQL_NTS);
	if(!SQL_SUCCEEDED(ret)) //连接失败时返回错误值
		return -1;
    ret = SQLConnect(target_dbc, (SQLCHAR *)target_dsn, SQL_NTS, (SQLCHAR *)target_user_id, SQL_NTS, (SQLCHAR *)target_user_password, SQL_NTS);
	if(!SQL_SUCCEEDED(ret)) //连接失败时返回错误值
		return -1;
	
	/*Step 4：初始化语句句柄*/
    ret = SQLAllocStmt(src_dbc, &src_stmt);
    ret = SQLAllocStmt(target_dbc, &target_stmt);

    /*Step 5：两种方式执行语句*/
    /*执行SQL语句*/
	/*创建目标表 T71_department*/ 
	ret = SQLExecDirect(target_stmt, (SQLCHAR *) "create table T71_department	(dept_name varchar(20) not null, building varchar(15), budget numeric(12,2) )", SQL_NTS);
	SQLCHAR dept_name[21] = {0};
    SQLCHAR building[16] = {0};
    SQLREAL budget;
    
    /*方式一：预编译带有参数的语句*/
	//需要多次执行插入，因此预先声明插人语句
    SQLLEN lenIn1 = SQL_NTS,lenIn2 = SQL_NTS,lenIn3 = 0;
	ret=SQLPrepare(target_stmt, (SQLCHAR *)"INSERT INTO T71_department(dept_name, building, budget) VALUES(?, ?, ?)",SQL_NTS);
	if(ret==SQL_SUCCESS)
	{
    	//绑定参数
		ret=SQLBindParameter(target_stmt, 1,SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 
							20,0,dept_name, sizeof(dept_name), &lenIn1);
    	ret=SQLBindParameter(target_stmt, 2,SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 
							15,0,building, sizeof(building), &lenIn2);
    	ret=SQLBindParameter(target_stmt, 3,SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_FLOAT,
							12,2, &budget, 0, &lenIn3);
	}

    SQLLEN lenOut1,lenOut2,lenOut3;
    unsigned char query[] = "select dept_name,building,budget from department";
    /*执行SQL语句*/
	ret = SQLExecDirect(src_stmt, (SQLCHAR *) query, SQL_NTS);
	if (ret == SQL_SUCCESS){
		//将结果集中的属性列一一绑定至变量
    	SQLBindCol(src_stmt, 1, SQL_C_CHAR, dept_name, sizeof(dept_name), &lenOut1);
    	SQLBindCol(src_stmt, 2, SQL_C_CHAR, building, sizeof(building), &lenOut2);
    	SQLBindCol(src_stmt, 3, SQL_C_FLOAT,&budget, 0, &lenOut3);
	    /*Step 6：处理结果集并执行预编译后的语句*/
		while ((ret=SQLFetch(src_stmt))==SQL_SUCCESS) {
    	    printf("%s\t %s\t %g\n", dept_name,building,budget);
			ret=SQLExecute(target_stmt);
    	}
   	}
    else
  	    printf("%d\n", ret);
/*Step 7：中止处理*/
  	SQLFreeStmt(src_stmt,SQL_DROP);
	SQLDisconnect(src_dbc);				    
	SQLFreeConnect(src_dbc);				    
	SQLFreeEnv(src_env);
	
  	SQLFreeStmt(target_stmt,SQL_DROP);
	SQLDisconnect(target_dbc);				    
	SQLFreeConnect(target_dbc);				    
	SQLFreeEnv(target_env);
}

