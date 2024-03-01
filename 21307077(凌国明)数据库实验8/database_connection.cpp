#include <windows.h>
#include <stdio.h>
#include <sql.h>
#include <sqlext.h>
#include <iostream>
#include <string>
#include <iomanip> 
using namespace std;

// 修改后的 print_table 函数，只接受 SQLHSTMT 句柄
int print_table(SQLHSTMT src_stmt) {
    SQLRETURN ret; // 调用结果
    wchar_t name[256] = {0};
    SQLLEN lenOut1;

    unsigned char query[] = "SELECT name FROM customer";
    /* 执行SQL语句 */
    ret = SQLExecDirect(src_stmt, (SQLCHAR *)query, SQL_NTS);
    if (ret == SQL_SUCCESS) {
        // 将结果集中的属性列绑定至变量
        SQLBindCol(src_stmt, 1, SQL_C_WCHAR, name, sizeof(name), &lenOut1);
        /* 处理结果集并执行预编译后的语句 */
        while ((ret = SQLFetch(src_stmt)) == SQL_SUCCESS) {
            wprintf(L"%s\n", name);
        }
    } else {
        printf("%d\n", ret);
    }

    /* 释放语句句柄 */
    SQLFreeStmt(src_stmt, SQL_DROP);
    return 0;
}

// 函数用于调用 顾客的 GetGoodsInfo 存储过程
int callGetGoodsInfo(SQLHDBC src_dbc, const std::string& orderType, const std::string& searchValue, const std::string& searchType) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // 分配语句句柄
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // 准备调用存储过程的 SQL 命令
    std::string procCall = "CALL GetGoodsInfo(?, ?, ?)";

    // 绑定参数
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, (SQLCHAR*)orderType.c_str(), 0, NULL);
    ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, (SQLCHAR*)searchValue.c_str(), 0, NULL);
    ret = SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, (SQLCHAR*)searchType.c_str(), 0, NULL);

    // 执行存储过程
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    // 处理返回的数据
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        char name[256];
        char category[256];
        double selling_price;
        char expiration_date[256];
        char place[256];
        int shelf_quantity;
        int id;

        // 获取返回的列数据
        SQLGetData(hStmt, 1, SQL_C_CHAR, name, sizeof(name), NULL);
        SQLGetData(hStmt, 2, SQL_C_CHAR, category, sizeof(category), NULL);
        SQLGetData(hStmt, 3, SQL_C_DOUBLE, &selling_price, 0, NULL);
        SQLGetData(hStmt, 4, SQL_C_CHAR, expiration_date, sizeof(expiration_date), NULL);
        SQLGetData(hStmt, 5, SQL_C_SLONG, &shelf_quantity, 0, NULL);
        SQLGetData(hStmt, 6, SQL_C_SLONG, &id, 0, NULL);
        SQLGetData(hStmt, 7, SQL_C_CHAR, place, sizeof(place), NULL);

        // 设置每个字段的宽度
		const int width_id = 6;
		const int width_name = 15;
		const int width_category = 15;
		const int width_expiration_date = 15;
		const int width_shelf_quantity = 8;
		
		std::cout << "ID: " << std::left << std::setw(width_id)  << id
		          << "名称: "<< std::left << std::setw(width_name)  << name
		          << "种类: " << std::left << std::setw(width_category)  << category
		          << "过期日期: "<< std::left << std::setw(width_expiration_date)  << expiration_date
		          << "在架数量: "<< std::left << std::setw(width_shelf_quantity)  << shelf_quantity
		          << "位置: "<< std::left << std::setw(width_shelf_quantity)  << place
		          << std::endl;
    }

    // 释放语句句柄
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// 函数用于调用 顾客的 PlaceOrder 存储过程
int callPlaceOrderProcedure(SQLHDBC src_dbc, int goods_id, int quantity, int customer_id) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // 分配语句句柄
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // 准备调用存储过程的 SQL 命令
    std::string procCall = "CALL PlaceOrder(?, ?, ?)";

    // 绑定参数
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &goods_id, 0, NULL);
    ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &quantity, 0, NULL);
    ret = SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &customer_id, 0, NULL);

    // 执行存储过程
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    // 释放语句句柄
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// 函数用于调用 员工的 CheckStockOut 存储过程并输出结果
int callCheckStockOutProcedure(SQLHDBC src_dbc) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // 分配语句句柄
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // 准备调用存储过程的 SQL 命令
    std::string procCall = "CALL CheckStockOut()";

    // 执行存储过程
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    // 处理返回的数据
    SQLINTEGER goods_id;
    SQLCHAR name[256];
    SQLCHAR category[256];
    SQLCHAR placement[256];
    SQLINTEGER shelf_quantity;
    SQLINTEGER stock_quantity;
    SQLINTEGER max_on_shelf_quan;
    SQLDOUBLE shortage_ratio;

    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLGetData(hStmt, 1, SQL_C_SLONG, &goods_id, 0, NULL);
        SQLGetData(hStmt, 2, SQL_C_CHAR, name, sizeof(name), NULL);
        SQLGetData(hStmt, 3, SQL_C_CHAR, category, sizeof(category), NULL);
        SQLGetData(hStmt, 4, SQL_C_CHAR, placement, sizeof(placement), NULL);
        SQLGetData(hStmt, 5, SQL_C_SLONG, &shelf_quantity, 0, NULL);
        SQLGetData(hStmt, 6, SQL_C_SLONG, &stock_quantity, 0, NULL);
        SQLGetData(hStmt, 7, SQL_C_SLONG, &max_on_shelf_quan, 0, NULL);
        SQLGetData(hStmt, 8, SQL_C_DOUBLE, &shortage_ratio, 0, NULL);

        const int width_goods_id = 3;
		const int width_name = 13;
		const int width_category = 14;
		const int width_placement = 5;
		const int width_shelf_quantity = 5;
		const int width_stock_quantity = 5;
		const int width_max_on_shelf_quan = 5;
		const int width_shortage_ratio = 2;
		
		std::cout << "ID: " << std::left << std::setw(width_goods_id) << goods_id
		          << "名称: " << std::left << std::setw(width_name) << name
		          << "种类: " << std::left << std::setw(width_category) << category
		          << "位置: " << std::left << std::setw(width_placement) << placement
		          << "在架数量: " << std::left << std::setw(width_shelf_quantity) << shelf_quantity
		          << "库存数量: " << std::left << std::setw(width_stock_quantity) << stock_quantity
		          << "最大上架数量: " << std::left << std::setw(width_max_on_shelf_quan) << max_on_shelf_quan
		          << "缺货比: " << std::left << std::setw(width_shortage_ratio) << shortage_ratio
		          << std::endl;
    }

    // 释放语句句柄
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// 函数用于调用 员工的 RestockGoods 存储过程
int callRestockGoodsProcedure(SQLHDBC src_dbc, int goods_id) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // 分配语句句柄
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // 准备调用存储过程的 SQL 命令
    std::string procCall = "CALL RestockGoods(?)";

    // 绑定参数
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &goods_id, 0, NULL);

    // 执行存储过程
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    } else {
        std::cout << "Restocking procedure executed successfully for goods ID: " << goods_id << std::endl;
    }

    // 释放语句句柄
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// 函数用于调用 员工的 CheckExpiredGoods 存储过程并输出结果
int callCheckExpiredGoodsProcedure(SQLHDBC src_dbc) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // 分配语句句柄
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // 准备调用存储过程的 SQL 命令
    std::string procCall = "CALL CheckExpiredGoods()";

    // 执行存储过程
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    // 处理返回的数据
    SQLINTEGER goods_id;
    SQLCHAR name[256];
    SQLCHAR expiration_date[256];
    SQLINTEGER shelf_quantity;
    SQLINTEGER stock_quantity;

    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLGetData(hStmt, 1, SQL_C_SLONG, &goods_id, 0, NULL);
        SQLGetData(hStmt, 2, SQL_C_CHAR, name, sizeof(name), NULL);
        SQLGetData(hStmt, 3, SQL_C_CHAR, expiration_date, sizeof(expiration_date), NULL);
        SQLGetData(hStmt, 4, SQL_C_SLONG, &shelf_quantity, 0, NULL);
        SQLGetData(hStmt, 5, SQL_C_SLONG, &stock_quantity, 0, NULL);

        // 设置每个字段的宽度
		const int width_goods_id = 3;
		const int width_name = 16;
		const int width_expiration_date = 14; // 过期日期的宽度
		const int width_shelf_quantity = 5;
		const int width_stock_quantity = 5;
		
		std::cout << "ID: " << std::left << std::setw(width_goods_id) << goods_id
		          << "名称: " << std::left << std::setw(width_name) << name
		          << "过期日期: " << std::left << std::setw(width_expiration_date) << expiration_date
		          << "在架数量: " << std::left << std::setw(width_shelf_quantity) << shelf_quantity
		          << "库存数量: " << std::left << std::setw(width_stock_quantity) << stock_quantity
		          << std::endl;
    }

    // 释放语句句柄
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// 函数用于调用 员工的 ClearExpiredGoods 存储过程
int callClearExpiredGoodsProcedure(SQLHDBC src_dbc, int goods_id) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // 分配语句句柄
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // 准备调用存储过程的 SQL 命令
    std::string procCall = "CALL ClearExpiredGoods(?)";

    // 绑定参数
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &goods_id, 0, NULL);

    // 执行存储过程
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    } else {
        std::cout << "ClearExpiredGoods procedure executed successfully for goods ID: " << goods_id << std::endl;
    }

    // 释放语句句柄
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// 函数用于 员工 获取并打印商品信息
int printGoodsInfo(SQLHDBC dbc) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // 分配语句句柄
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // 准备SQL查询命令
    const char* sqlQuery = "SELECT name, placement, expiration_date, shelf_quantity, goods_id, max_on_shelf_quan, stock_quantity FROM Goods";

    // 执行SQL命令
    ret = SQLExecDirect(hStmt, (SQLCHAR*)sqlQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the select statement." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    // 绑定列
    char name[256], placement[256], expiration_date[11];
    int shelf_quantity, goods_id, max_on_shelf_quan, stock_quantity;

    SQLBindCol(hStmt, 1, SQL_C_CHAR, name, sizeof(name), NULL);
    SQLBindCol(hStmt, 2, SQL_C_CHAR, placement, sizeof(placement), NULL);
    SQLBindCol(hStmt, 3, SQL_C_CHAR, expiration_date, sizeof(expiration_date), NULL);
    SQLBindCol(hStmt, 4, SQL_C_LONG, &shelf_quantity, 0, NULL);
    SQLBindCol(hStmt, 5, SQL_C_LONG, &goods_id, 0, NULL);
    SQLBindCol(hStmt, 6, SQL_C_LONG, &max_on_shelf_quan, 0, NULL);
    SQLBindCol(hStmt, 7, SQL_C_LONG, &stock_quantity, 0, NULL);

    // 设置每个字段的宽度
    const int width_id = 4;
    const int width_name = 15;
    const int width_placement = 6;
    const int width_expiration_date = 13;
    const int width_shelf_quantity = 5;
    const int width_max_on_shelf_quan = 5;
    const int width_stock_quantity = 5;

    // 循环遍历结果集
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        std::cout << "ID: " << std::left << std::setw(width_id) << goods_id
                  << "名称: " << std::left << std::setw(width_name) << name
                  << "位置: " << std::left << std::setw(width_placement) << placement
                  << "过期日期: " << std::left << std::setw(width_expiration_date) << expiration_date
                  << "在架数量: " << std::left << std::setw(width_shelf_quantity) << shelf_quantity
                  << "最大上架数量: " << std::left << std::setw(width_max_on_shelf_quan) << max_on_shelf_quan
                  << "库存数量: " << std::left << std::setw(width_stock_quantity) << stock_quantity
                  << std::endl;
    }

    // 释放语句句柄
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// 函数用于调用 经理的 GetSalesReport 存储过程并输出结果
int callGetSalesReportProcedure(SQLHDBC src_dbc, const std::string& start_date, const std::string& end_date) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // 分配语句句柄
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // 准备调用存储过程的 SQL 命令
    std::string procCall = "CALL GetSalesReport(?, ?)";

    // 绑定参数
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, (SQLCHAR*)start_date.c_str(), 0, NULL);
    ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, (SQLCHAR*)end_date.c_str(), 0, NULL);

    // 执行存储过程
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    // 处理返回的数据
    SQLINTEGER goods_id;
    SQLCHAR name[256];
    SQLINTEGER total_quantity;
    SQLDOUBLE total_sales;
    SQLDOUBLE total_profit;
    SQLINTEGER max_on_shelf_quan;
    SQLINTEGER stock_quantity;

    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLGetData(hStmt, 1, SQL_C_SLONG, &goods_id, 0, NULL);
        SQLGetData(hStmt, 2, SQL_C_CHAR, name, sizeof(name), NULL);
        SQLGetData(hStmt, 3, SQL_C_SLONG, &total_quantity, 0, NULL);
        SQLGetData(hStmt, 4, SQL_C_DOUBLE, &total_sales, 0, NULL);
        SQLGetData(hStmt, 5, SQL_C_DOUBLE, &total_profit, 0, NULL);
        SQLGetData(hStmt, 6, SQL_C_SLONG, &max_on_shelf_quan, 0, NULL);
        SQLGetData(hStmt, 7, SQL_C_SLONG, &stock_quantity, 0, NULL);

        // 设置每个字段的宽度
		const int width_goods_id = 3;
		const int width_name = 13;
		const int width_total_quantity = 5;
		const int width_total_sales = 8;
		const int width_total_profit = 8;
		const int width_max_on_shelf_quan = 6;
		const int width_stock_quantity = 5;
		
		std::cout << "商品ID: " << std::left << std::setw(width_goods_id) << goods_id
		          << "名称: " << std::left << std::setw(width_name) << name
		          << "总数量: " << std::left << std::setw(width_total_quantity) << total_quantity
		          << "总销售额: " << std::left << std::setw(width_total_sales) << total_sales
		          << "总利润: " << std::left << std::setw(width_total_profit) << total_profit
		          << "最大上架数量: " << std::left << std::setw(width_max_on_shelf_quan) << max_on_shelf_quan
		          << "库存数量: " << std::left << std::setw(width_stock_quantity) << stock_quantity
		          << std::endl;
    }

    // 释放语句句柄
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// 函数用于调用经理的 FireEmployee 存储过程
int callFireEmployeeProcedure(SQLHDBC src_dbc, const std::string& employeeUsername) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // 分配语句句柄
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // 准备调用存储过程的 SQL 命令
    std::string procCall = "CALL FireEmployee(?)";

    // 绑定参数
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, (SQLCHAR*)employeeUsername.c_str(), 0, NULL);

    // 执行存储过程
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    std::cout << "Employee '" << employeeUsername << "' has been successfully fired." << std::endl;

    // 释放语句句柄
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// 函数用于调用经理的 HireEmployee 存储过程
int callHireEmployeeProcedure(SQLHDBC src_dbc, const std::string& newEmployeeUsername) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // 分配语句句柄
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // 准备调用存储过程的 SQL 命令
    std::string procCall = "CALL HireEmployee(?)";

    // 绑定参数
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, (SQLCHAR*)newEmployeeUsername.c_str(), 0, NULL);

    // 执行存储过程
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    std::cout << "New employee '" << newEmployeeUsername << "' has been successfully hired." << std::endl;

    // 释放语句句柄
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// 函数用于 经理 修改商品的最大上架数量
int updateMaxShelfQuantity(SQLHDBC src_dbc, int goods_id, int max_shelf_quantity) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // 分配语句句柄
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // 准备SQL更新命令
    char sqlQuery[256];
    sprintf(sqlQuery, "UPDATE Goods SET max_on_shelf_quan = %d WHERE goods_id = %d", max_shelf_quantity, goods_id);

    // 执行SQL命令
    ret = SQLExecDirect(hStmt, (SQLCHAR*)sqlQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the update statement." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    } else {
        std::cout << "Updated max shelf quantity successfully for goods ID: " << goods_id << std::endl;
    }

    // 释放语句句柄
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// 函数用于 经理 增加商品的库存数量
int updateStockQuantity(SQLHDBC src_dbc, int goods_id, int increase_amount) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // 分配语句句柄
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // 准备SQL更新命令
    char sqlQuery[256];
    sprintf(sqlQuery, "UPDATE Goods SET stock_quantity = stock_quantity + %d WHERE goods_id = %d", increase_amount, goods_id);

    // 执行SQL命令
    ret = SQLExecDirect(hStmt, (SQLCHAR*)sqlQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the update statement." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    } else {
        std::cout << "Updated stock quantity successfully for goods ID: " << goods_id << std::endl;
    }

    // 释放语句句柄
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// 函数用于 经理 获取并打印商品信息
int printGoodsInfo2(SQLHDBC dbc) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // 分配语句句柄
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // 准备SQL查询命令
    const char* sqlQuery = "SELECT goods_id, name, category, purchase_price, selling_price, max_on_shelf_quan, stock_quantity FROM Goods";

    // 执行SQL命令
    ret = SQLExecDirect(hStmt, (SQLCHAR*)sqlQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the select statement." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    // 绑定列
    char name[256], category[256];
    double purchase_price, selling_price;
    int goods_id, max_on_shelf_quan, stock_quantity;

    SQLBindCol(hStmt, 1, SQL_C_LONG, &goods_id, 0, NULL);
    SQLBindCol(hStmt, 2, SQL_C_CHAR, name, sizeof(name), NULL);
    SQLBindCol(hStmt, 3, SQL_C_CHAR, category, sizeof(category), NULL);
    SQLBindCol(hStmt, 4, SQL_C_DOUBLE, &purchase_price, 0, NULL);
    SQLBindCol(hStmt, 5, SQL_C_DOUBLE, &selling_price, 0, NULL);
    SQLBindCol(hStmt, 6, SQL_C_LONG, &max_on_shelf_quan, 0, NULL);
    SQLBindCol(hStmt, 7, SQL_C_LONG, &stock_quantity, 0, NULL);

    // 设置每个字段的宽度
    const int width_id = 3;
    const int width_name = 15;
    const int width_category = 15;
    const int width_purchase_price = 7;
    const int width_selling_price = 7;
    const int width_max_on_shelf_quan = 7;
    const int width_stock_quantity = 7;

    // 循环遍历结果集
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        std::cout << "ID: " << std::left << std::setw(width_id) << goods_id
                  << "名称: " << std::left << std::setw(width_name) << name
                  << "种类: " << std::left << std::setw(width_category) << category
                  << "进价: " << std::left << std::setw(width_purchase_price) << purchase_price
                  << "售价: " << std::left << std::setw(width_selling_price) << selling_price
                  << "最大上架数量: " << std::left << std::setw(width_max_on_shelf_quan) << max_on_shelf_quan
                  << "库存数量: " << std::left << std::setw(width_stock_quantity) << stock_quantity
                  << std::endl;
    }

    // 释放语句句柄
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

void customerFunctions(SQLHDBC src_dbc) {
    int functionChoice;
    std::string sortOrder, item, type;

    bool inCustomerMenu = true;

    while (inCustomerMenu) {
        std::cout << "\n=== 顾客功能 ===\n"
        		  << "1. 查看商品\n"
                  << "2. 查找商品\n"
                  << "3. 购买商品\n"
                  << "4. 退出账号\n"
                  << "请选择操作: ";
        std::cin >> functionChoice;

        switch (functionChoice) {
        	case 1:
                // 查询商品的实现
                callGetGoodsInfo(src_dbc, "ASC", "", "");
                break;
            case 2:
                std::cout << "请输入查询类型 (name/category): ";
                std::cin >> type;
                std::cout << "请输入商品名称或类别: ";
                std::cin >> item;
                
                // 查询商品的实现
                callGetGoodsInfo(src_dbc, "ASC", item, type);
                break;
            case 3:
                int goodsId, quantity;
                std::cout << "请输入商品ID: ";
                std::cin >> goodsId;
                std::cout << "请输入商品数量: ";
                std::cin >> quantity;
                // 下订单的实现
                callPlaceOrderProcedure(src_dbc, goodsId, quantity, 2);
                break;
            case 4:
                inCustomerMenu = false;
                cout << endl;
                break;
            default:
                std::cout << "无效选择，请重新选择。" << std::endl;
        }
    }
}

void employeeFunctions(SQLHDBC src_dbc) {
    int functionChoice;
    bool inEmployeeMenu = true;

    while (inEmployeeMenu) {
        std::cout << "\n=== 员工功能 ===\n"
                  << "1. 查看缺货商品\n"
                  << "2. 补充货架商品\n"
                  << "3. 查看过期商品\n"
                  << "4. 清理过期商品\n"
                  << "5. 查看所有商品\n"
                  << "6. 退出账户\n"
                  << "请选择操作: ";
        std::cin >> functionChoice;

        switch (functionChoice) {
            case 1:
                // 检查库存的实现
                callCheckStockOutProcedure(src_dbc);
                break;
            case 2:
                int productId;
                std::cout << "请输入需要补货的商品ID: ";
                std::cin >> productId;
                // 补货的实现
                callRestockGoodsProcedure(src_dbc, productId);
                break;
            case 3:
                // 查看过期商品的实现
                callCheckExpiredGoodsProcedure(src_dbc);
                break;
            case 4:
                int expiredGoodsId;
                std::cout << "请输入需要清理的过期商品ID: ";
                std::cin >> expiredGoodsId;
                // 清理过期商品的实现
                callClearExpiredGoodsProcedure(src_dbc, expiredGoodsId);
                break;
            case 5:
                // 查询商品的实现
                printGoodsInfo(src_dbc); 
                break;
            case 6:
                inEmployeeMenu = false;
                cout << endl; 
                break;
            default:
                std::cout << "无效选择，请重新选择。" << std::endl;
        }
    }
}

void managerFunctions(SQLHDBC src_dbc) {
    int functionChoice;
    bool inManagerMenu = true;

    while (inManagerMenu) {
        std::cout << "\n=== 经理功能 ===\n"
        		  << "1. 查看所有商品\n"
                  << "2. 修改上架数量\n"
                  << "3. 补充库存数量\n"
                  << "4. 查看销售报告\n"
                  << "5. 开除员工\n"
                  << "6. 招聘员工\n"
                  << "7. 退出账号\n"
                  << "请选择操作: ";
        std::cin >> functionChoice;
        
        std::string startDate, endDate, employee_name;

        switch (functionChoice) {
        	case 1:
                // 查询商品的实现
                printGoodsInfo2(src_dbc); 
                break;   
            case 2:
                int shelfId, maxQuantity;
                std::cout << "请输入商品ID: ";
                std::cin >> shelfId;
                std::cout << "请输入最大上架数量: ";
                std::cin >> maxQuantity;
                // 修改货架数量的实现
                updateMaxShelfQuantity(src_dbc, shelfId, maxQuantity);
                break;
            case 3:
                int stockId, stockQuantity;
                std::cout << "请输入商品ID: ";
                std::cin >> stockId;
                std::cout << "请输入要增加的库存数量: ";
                std::cin >> stockQuantity;
                // 补充库存数量的实现
                updateStockQuantity(src_dbc, stockId, stockQuantity);
                break;
            case 4:
                std::cout << "请输入销售报告的开始日期 (YYYY-MM-DD): ";
                std::cin >> startDate;
                std::cout << "请输入销售报告的结束日期 (YYYY-MM-DD): ";
                std::cin >> endDate;
                // 销售报告的实现
                callGetSalesReportProcedure(src_dbc, startDate, endDate);
                break;    
            case 5: 
            	std::cout << "请输入要删除的员工: ";
                std::cin >> employee_name;
            	callFireEmployeeProcedure(src_dbc, employee_name);
            	break; 
            case 6:
				 std::cout << "请输入要增加的员工: ";
                std::cin >> employee_name;
            	callHireEmployeeProcedure(src_dbc, employee_name);
            	break;
            case 7:
                inManagerMenu = false;
                cout << endl;
                break;
            default:
                std::cout << "无效选择，请重新选择。" << std::endl;
        }
    }
}


int main() {
    
	SQLRETURN ret; // 调用结果
    /* 定义句柄和变量 */
    SQLHENV src_env;  // 环境句柄
    SQLHDBC src_dbc;  // 连接句柄 
    SQLHSTMT src_stmt; // 语句句柄 

    int roleChoice;
    bool running = true;
    std::string username, password;

    while (running) {
    	std::cout << "===========欢迎登录超市管理系统============" << std::endl;
        std::cout << "请选择角色: 1. 顾客 2. 员工 3. 经理 4. 退出" << std::endl;
        std::cin >> roleChoice;
        // 现在连接是基于用户输入的用户名和密码
	    std::string username, password;
	    std::cout << "请输入账户: ";
	    std::cin >> username;
	    std::cout << "请输入密码: ";
	    std::cin >> password;
	    
	
	    /* 初始化环境 */
	    SQLAllocEnv(&src_env);
	    // 设置管理环境的属性
	    SQLSetEnvAttr(src_env, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
	
	    /* 建立连接 */
	    // 分配连接句柄
	    ret = SQLAllocConnect(src_env, &src_dbc);
	
	    ret = SQLConnect(src_dbc, (SQLCHAR *)"supermarket_in_mysql80", SQL_NTS, 
	                     (SQLCHAR *)username.c_str(), SQL_NTS, 
	                     (SQLCHAR *)password.c_str(), SQL_NTS);
	
	    if (!SQL_SUCCEEDED(ret)) {
	        std::cout << "账户或密码错误\n" << std::endl;
	        /* 断开连接 */
		    SQLDisconnect(src_dbc);
		    SQLFreeConnect(src_dbc);
		    SQLFreeEnv(src_env);
	        continue;
	    }

        switch (roleChoice) {
            case 1:
	            customerFunctions(src_dbc);
                break;
            case 2:
	            employeeFunctions(src_dbc);
                break;
            case 3:
				managerFunctions(src_dbc);
                break;
            case 4:
                running = false;
                break;
            default:
                std::cout << "无效选择，请重新选择。" << std::endl;
        }
    }
    

    /* 断开连接 */
    SQLDisconnect(src_dbc);
    SQLFreeConnect(src_dbc);
    SQLFreeEnv(src_env);

    return 0;
}

