#include <windows.h>
#include <stdio.h>
#include <sql.h>
#include <sqlext.h>
#include <iostream>
#include <string>
#include <iomanip> 
using namespace std;

// �޸ĺ�� print_table ������ֻ���� SQLHSTMT ���
int print_table(SQLHSTMT src_stmt) {
    SQLRETURN ret; // ���ý��
    wchar_t name[256] = {0};
    SQLLEN lenOut1;

    unsigned char query[] = "SELECT name FROM customer";
    /* ִ��SQL��� */
    ret = SQLExecDirect(src_stmt, (SQLCHAR *)query, SQL_NTS);
    if (ret == SQL_SUCCESS) {
        // ��������е������а�������
        SQLBindCol(src_stmt, 1, SQL_C_WCHAR, name, sizeof(name), &lenOut1);
        /* ����������ִ��Ԥ��������� */
        while ((ret = SQLFetch(src_stmt)) == SQL_SUCCESS) {
            wprintf(L"%s\n", name);
        }
    } else {
        printf("%d\n", ret);
    }

    /* �ͷ������ */
    SQLFreeStmt(src_stmt, SQL_DROP);
    return 0;
}

// �������ڵ��� �˿͵� GetGoodsInfo �洢����
int callGetGoodsInfo(SQLHDBC src_dbc, const std::string& orderType, const std::string& searchValue, const std::string& searchType) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // ���������
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // ׼�����ô洢���̵� SQL ����
    std::string procCall = "CALL GetGoodsInfo(?, ?, ?)";

    // �󶨲���
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, (SQLCHAR*)orderType.c_str(), 0, NULL);
    ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, (SQLCHAR*)searchValue.c_str(), 0, NULL);
    ret = SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, (SQLCHAR*)searchType.c_str(), 0, NULL);

    // ִ�д洢����
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    // �����ص�����
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        char name[256];
        char category[256];
        double selling_price;
        char expiration_date[256];
        char place[256];
        int shelf_quantity;
        int id;

        // ��ȡ���ص�������
        SQLGetData(hStmt, 1, SQL_C_CHAR, name, sizeof(name), NULL);
        SQLGetData(hStmt, 2, SQL_C_CHAR, category, sizeof(category), NULL);
        SQLGetData(hStmt, 3, SQL_C_DOUBLE, &selling_price, 0, NULL);
        SQLGetData(hStmt, 4, SQL_C_CHAR, expiration_date, sizeof(expiration_date), NULL);
        SQLGetData(hStmt, 5, SQL_C_SLONG, &shelf_quantity, 0, NULL);
        SQLGetData(hStmt, 6, SQL_C_SLONG, &id, 0, NULL);
        SQLGetData(hStmt, 7, SQL_C_CHAR, place, sizeof(place), NULL);

        // ����ÿ���ֶεĿ��
		const int width_id = 6;
		const int width_name = 15;
		const int width_category = 15;
		const int width_expiration_date = 15;
		const int width_shelf_quantity = 8;
		
		std::cout << "ID: " << std::left << std::setw(width_id)  << id
		          << "����: "<< std::left << std::setw(width_name)  << name
		          << "����: " << std::left << std::setw(width_category)  << category
		          << "��������: "<< std::left << std::setw(width_expiration_date)  << expiration_date
		          << "�ڼ�����: "<< std::left << std::setw(width_shelf_quantity)  << shelf_quantity
		          << "λ��: "<< std::left << std::setw(width_shelf_quantity)  << place
		          << std::endl;
    }

    // �ͷ������
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// �������ڵ��� �˿͵� PlaceOrder �洢����
int callPlaceOrderProcedure(SQLHDBC src_dbc, int goods_id, int quantity, int customer_id) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // ���������
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // ׼�����ô洢���̵� SQL ����
    std::string procCall = "CALL PlaceOrder(?, ?, ?)";

    // �󶨲���
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &goods_id, 0, NULL);
    ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &quantity, 0, NULL);
    ret = SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &customer_id, 0, NULL);

    // ִ�д洢����
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    // �ͷ������
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// �������ڵ��� Ա���� CheckStockOut �洢���̲�������
int callCheckStockOutProcedure(SQLHDBC src_dbc) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // ���������
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // ׼�����ô洢���̵� SQL ����
    std::string procCall = "CALL CheckStockOut()";

    // ִ�д洢����
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    // �����ص�����
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
		          << "����: " << std::left << std::setw(width_name) << name
		          << "����: " << std::left << std::setw(width_category) << category
		          << "λ��: " << std::left << std::setw(width_placement) << placement
		          << "�ڼ�����: " << std::left << std::setw(width_shelf_quantity) << shelf_quantity
		          << "�������: " << std::left << std::setw(width_stock_quantity) << stock_quantity
		          << "����ϼ�����: " << std::left << std::setw(width_max_on_shelf_quan) << max_on_shelf_quan
		          << "ȱ����: " << std::left << std::setw(width_shortage_ratio) << shortage_ratio
		          << std::endl;
    }

    // �ͷ������
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// �������ڵ��� Ա���� RestockGoods �洢����
int callRestockGoodsProcedure(SQLHDBC src_dbc, int goods_id) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // ���������
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // ׼�����ô洢���̵� SQL ����
    std::string procCall = "CALL RestockGoods(?)";

    // �󶨲���
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &goods_id, 0, NULL);

    // ִ�д洢����
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    } else {
        std::cout << "Restocking procedure executed successfully for goods ID: " << goods_id << std::endl;
    }

    // �ͷ������
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// �������ڵ��� Ա���� CheckExpiredGoods �洢���̲�������
int callCheckExpiredGoodsProcedure(SQLHDBC src_dbc) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // ���������
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // ׼�����ô洢���̵� SQL ����
    std::string procCall = "CALL CheckExpiredGoods()";

    // ִ�д洢����
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    // �����ص�����
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

        // ����ÿ���ֶεĿ��
		const int width_goods_id = 3;
		const int width_name = 16;
		const int width_expiration_date = 14; // �������ڵĿ��
		const int width_shelf_quantity = 5;
		const int width_stock_quantity = 5;
		
		std::cout << "ID: " << std::left << std::setw(width_goods_id) << goods_id
		          << "����: " << std::left << std::setw(width_name) << name
		          << "��������: " << std::left << std::setw(width_expiration_date) << expiration_date
		          << "�ڼ�����: " << std::left << std::setw(width_shelf_quantity) << shelf_quantity
		          << "�������: " << std::left << std::setw(width_stock_quantity) << stock_quantity
		          << std::endl;
    }

    // �ͷ������
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// �������ڵ��� Ա���� ClearExpiredGoods �洢����
int callClearExpiredGoodsProcedure(SQLHDBC src_dbc, int goods_id) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // ���������
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // ׼�����ô洢���̵� SQL ����
    std::string procCall = "CALL ClearExpiredGoods(?)";

    // �󶨲���
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &goods_id, 0, NULL);

    // ִ�д洢����
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    } else {
        std::cout << "ClearExpiredGoods procedure executed successfully for goods ID: " << goods_id << std::endl;
    }

    // �ͷ������
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// �������� Ա�� ��ȡ����ӡ��Ʒ��Ϣ
int printGoodsInfo(SQLHDBC dbc) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // ���������
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // ׼��SQL��ѯ����
    const char* sqlQuery = "SELECT name, placement, expiration_date, shelf_quantity, goods_id, max_on_shelf_quan, stock_quantity FROM Goods";

    // ִ��SQL����
    ret = SQLExecDirect(hStmt, (SQLCHAR*)sqlQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the select statement." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    // ����
    char name[256], placement[256], expiration_date[11];
    int shelf_quantity, goods_id, max_on_shelf_quan, stock_quantity;

    SQLBindCol(hStmt, 1, SQL_C_CHAR, name, sizeof(name), NULL);
    SQLBindCol(hStmt, 2, SQL_C_CHAR, placement, sizeof(placement), NULL);
    SQLBindCol(hStmt, 3, SQL_C_CHAR, expiration_date, sizeof(expiration_date), NULL);
    SQLBindCol(hStmt, 4, SQL_C_LONG, &shelf_quantity, 0, NULL);
    SQLBindCol(hStmt, 5, SQL_C_LONG, &goods_id, 0, NULL);
    SQLBindCol(hStmt, 6, SQL_C_LONG, &max_on_shelf_quan, 0, NULL);
    SQLBindCol(hStmt, 7, SQL_C_LONG, &stock_quantity, 0, NULL);

    // ����ÿ���ֶεĿ��
    const int width_id = 4;
    const int width_name = 15;
    const int width_placement = 6;
    const int width_expiration_date = 13;
    const int width_shelf_quantity = 5;
    const int width_max_on_shelf_quan = 5;
    const int width_stock_quantity = 5;

    // ѭ�����������
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        std::cout << "ID: " << std::left << std::setw(width_id) << goods_id
                  << "����: " << std::left << std::setw(width_name) << name
                  << "λ��: " << std::left << std::setw(width_placement) << placement
                  << "��������: " << std::left << std::setw(width_expiration_date) << expiration_date
                  << "�ڼ�����: " << std::left << std::setw(width_shelf_quantity) << shelf_quantity
                  << "����ϼ�����: " << std::left << std::setw(width_max_on_shelf_quan) << max_on_shelf_quan
                  << "�������: " << std::left << std::setw(width_stock_quantity) << stock_quantity
                  << std::endl;
    }

    // �ͷ������
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// �������ڵ��� ����� GetSalesReport �洢���̲�������
int callGetSalesReportProcedure(SQLHDBC src_dbc, const std::string& start_date, const std::string& end_date) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // ���������
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // ׼�����ô洢���̵� SQL ����
    std::string procCall = "CALL GetSalesReport(?, ?)";

    // �󶨲���
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, (SQLCHAR*)start_date.c_str(), 0, NULL);
    ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, (SQLCHAR*)end_date.c_str(), 0, NULL);

    // ִ�д洢����
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    // �����ص�����
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

        // ����ÿ���ֶεĿ��
		const int width_goods_id = 3;
		const int width_name = 13;
		const int width_total_quantity = 5;
		const int width_total_sales = 8;
		const int width_total_profit = 8;
		const int width_max_on_shelf_quan = 6;
		const int width_stock_quantity = 5;
		
		std::cout << "��ƷID: " << std::left << std::setw(width_goods_id) << goods_id
		          << "����: " << std::left << std::setw(width_name) << name
		          << "������: " << std::left << std::setw(width_total_quantity) << total_quantity
		          << "�����۶�: " << std::left << std::setw(width_total_sales) << total_sales
		          << "������: " << std::left << std::setw(width_total_profit) << total_profit
		          << "����ϼ�����: " << std::left << std::setw(width_max_on_shelf_quan) << max_on_shelf_quan
		          << "�������: " << std::left << std::setw(width_stock_quantity) << stock_quantity
		          << std::endl;
    }

    // �ͷ������
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// �������ڵ��þ���� FireEmployee �洢����
int callFireEmployeeProcedure(SQLHDBC src_dbc, const std::string& employeeUsername) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // ���������
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // ׼�����ô洢���̵� SQL ����
    std::string procCall = "CALL FireEmployee(?)";

    // �󶨲���
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, (SQLCHAR*)employeeUsername.c_str(), 0, NULL);

    // ִ�д洢����
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    std::cout << "Employee '" << employeeUsername << "' has been successfully fired." << std::endl;

    // �ͷ������
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// �������ڵ��þ���� HireEmployee �洢����
int callHireEmployeeProcedure(SQLHDBC src_dbc, const std::string& newEmployeeUsername) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // ���������
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // ׼�����ô洢���̵� SQL ����
    std::string procCall = "CALL HireEmployee(?)";

    // �󶨲���
    ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, (SQLCHAR*)newEmployeeUsername.c_str(), 0, NULL);

    // ִ�д洢����
    ret = SQLExecDirect(hStmt, (SQLCHAR*)procCall.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the stored procedure." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    std::cout << "New employee '" << newEmployeeUsername << "' has been successfully hired." << std::endl;

    // �ͷ������
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// �������� ���� �޸���Ʒ������ϼ�����
int updateMaxShelfQuantity(SQLHDBC src_dbc, int goods_id, int max_shelf_quantity) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // ���������
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // ׼��SQL��������
    char sqlQuery[256];
    sprintf(sqlQuery, "UPDATE Goods SET max_on_shelf_quan = %d WHERE goods_id = %d", max_shelf_quantity, goods_id);

    // ִ��SQL����
    ret = SQLExecDirect(hStmt, (SQLCHAR*)sqlQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the update statement." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    } else {
        std::cout << "Updated max shelf quantity successfully for goods ID: " << goods_id << std::endl;
    }

    // �ͷ������
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// �������� ���� ������Ʒ�Ŀ������
int updateStockQuantity(SQLHDBC src_dbc, int goods_id, int increase_amount) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // ���������
    ret = SQLAllocHandle(SQL_HANDLE_STMT, src_dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // ׼��SQL��������
    char sqlQuery[256];
    sprintf(sqlQuery, "UPDATE Goods SET stock_quantity = stock_quantity + %d WHERE goods_id = %d", increase_amount, goods_id);

    // ִ��SQL����
    ret = SQLExecDirect(hStmt, (SQLCHAR*)sqlQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the update statement." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    } else {
        std::cout << "Updated stock quantity successfully for goods ID: " << goods_id << std::endl;
    }

    // �ͷ������
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

// �������� ���� ��ȡ����ӡ��Ʒ��Ϣ
int printGoodsInfo2(SQLHDBC dbc) {
    SQLHSTMT hStmt;
    SQLRETURN ret;

    // ���������
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        return -1;
    }

    // ׼��SQL��ѯ����
    const char* sqlQuery = "SELECT goods_id, name, category, purchase_price, selling_price, max_on_shelf_quan, stock_quantity FROM Goods";

    // ִ��SQL����
    ret = SQLExecDirect(hStmt, (SQLCHAR*)sqlQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error executing the select statement." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return -1;
    }

    // ����
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

    // ����ÿ���ֶεĿ��
    const int width_id = 3;
    const int width_name = 15;
    const int width_category = 15;
    const int width_purchase_price = 7;
    const int width_selling_price = 7;
    const int width_max_on_shelf_quan = 7;
    const int width_stock_quantity = 7;

    // ѭ�����������
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        std::cout << "ID: " << std::left << std::setw(width_id) << goods_id
                  << "����: " << std::left << std::setw(width_name) << name
                  << "����: " << std::left << std::setw(width_category) << category
                  << "����: " << std::left << std::setw(width_purchase_price) << purchase_price
                  << "�ۼ�: " << std::left << std::setw(width_selling_price) << selling_price
                  << "����ϼ�����: " << std::left << std::setw(width_max_on_shelf_quan) << max_on_shelf_quan
                  << "�������: " << std::left << std::setw(width_stock_quantity) << stock_quantity
                  << std::endl;
    }

    // �ͷ������
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return 0;
}

void customerFunctions(SQLHDBC src_dbc) {
    int functionChoice;
    std::string sortOrder, item, type;

    bool inCustomerMenu = true;

    while (inCustomerMenu) {
        std::cout << "\n=== �˿͹��� ===\n"
        		  << "1. �鿴��Ʒ\n"
                  << "2. ������Ʒ\n"
                  << "3. ������Ʒ\n"
                  << "4. �˳��˺�\n"
                  << "��ѡ�����: ";
        std::cin >> functionChoice;

        switch (functionChoice) {
        	case 1:
                // ��ѯ��Ʒ��ʵ��
                callGetGoodsInfo(src_dbc, "ASC", "", "");
                break;
            case 2:
                std::cout << "�������ѯ���� (name/category): ";
                std::cin >> type;
                std::cout << "��������Ʒ���ƻ����: ";
                std::cin >> item;
                
                // ��ѯ��Ʒ��ʵ��
                callGetGoodsInfo(src_dbc, "ASC", item, type);
                break;
            case 3:
                int goodsId, quantity;
                std::cout << "��������ƷID: ";
                std::cin >> goodsId;
                std::cout << "��������Ʒ����: ";
                std::cin >> quantity;
                // �¶�����ʵ��
                callPlaceOrderProcedure(src_dbc, goodsId, quantity, 2);
                break;
            case 4:
                inCustomerMenu = false;
                cout << endl;
                break;
            default:
                std::cout << "��Чѡ��������ѡ��" << std::endl;
        }
    }
}

void employeeFunctions(SQLHDBC src_dbc) {
    int functionChoice;
    bool inEmployeeMenu = true;

    while (inEmployeeMenu) {
        std::cout << "\n=== Ա������ ===\n"
                  << "1. �鿴ȱ����Ʒ\n"
                  << "2. ���������Ʒ\n"
                  << "3. �鿴������Ʒ\n"
                  << "4. ���������Ʒ\n"
                  << "5. �鿴������Ʒ\n"
                  << "6. �˳��˻�\n"
                  << "��ѡ�����: ";
        std::cin >> functionChoice;

        switch (functionChoice) {
            case 1:
                // ������ʵ��
                callCheckStockOutProcedure(src_dbc);
                break;
            case 2:
                int productId;
                std::cout << "��������Ҫ��������ƷID: ";
                std::cin >> productId;
                // ������ʵ��
                callRestockGoodsProcedure(src_dbc, productId);
                break;
            case 3:
                // �鿴������Ʒ��ʵ��
                callCheckExpiredGoodsProcedure(src_dbc);
                break;
            case 4:
                int expiredGoodsId;
                std::cout << "��������Ҫ����Ĺ�����ƷID: ";
                std::cin >> expiredGoodsId;
                // ���������Ʒ��ʵ��
                callClearExpiredGoodsProcedure(src_dbc, expiredGoodsId);
                break;
            case 5:
                // ��ѯ��Ʒ��ʵ��
                printGoodsInfo(src_dbc); 
                break;
            case 6:
                inEmployeeMenu = false;
                cout << endl; 
                break;
            default:
                std::cout << "��Чѡ��������ѡ��" << std::endl;
        }
    }
}

void managerFunctions(SQLHDBC src_dbc) {
    int functionChoice;
    bool inManagerMenu = true;

    while (inManagerMenu) {
        std::cout << "\n=== ������ ===\n"
        		  << "1. �鿴������Ʒ\n"
                  << "2. �޸��ϼ�����\n"
                  << "3. ����������\n"
                  << "4. �鿴���۱���\n"
                  << "5. ����Ա��\n"
                  << "6. ��ƸԱ��\n"
                  << "7. �˳��˺�\n"
                  << "��ѡ�����: ";
        std::cin >> functionChoice;
        
        std::string startDate, endDate, employee_name;

        switch (functionChoice) {
        	case 1:
                // ��ѯ��Ʒ��ʵ��
                printGoodsInfo2(src_dbc); 
                break;   
            case 2:
                int shelfId, maxQuantity;
                std::cout << "��������ƷID: ";
                std::cin >> shelfId;
                std::cout << "����������ϼ�����: ";
                std::cin >> maxQuantity;
                // �޸Ļ���������ʵ��
                updateMaxShelfQuantity(src_dbc, shelfId, maxQuantity);
                break;
            case 3:
                int stockId, stockQuantity;
                std::cout << "��������ƷID: ";
                std::cin >> stockId;
                std::cout << "������Ҫ���ӵĿ������: ";
                std::cin >> stockQuantity;
                // ������������ʵ��
                updateStockQuantity(src_dbc, stockId, stockQuantity);
                break;
            case 4:
                std::cout << "���������۱���Ŀ�ʼ���� (YYYY-MM-DD): ";
                std::cin >> startDate;
                std::cout << "���������۱���Ľ������� (YYYY-MM-DD): ";
                std::cin >> endDate;
                // ���۱����ʵ��
                callGetSalesReportProcedure(src_dbc, startDate, endDate);
                break;    
            case 5: 
            	std::cout << "������Ҫɾ����Ա��: ";
                std::cin >> employee_name;
            	callFireEmployeeProcedure(src_dbc, employee_name);
            	break; 
            case 6:
				 std::cout << "������Ҫ���ӵ�Ա��: ";
                std::cin >> employee_name;
            	callHireEmployeeProcedure(src_dbc, employee_name);
            	break;
            case 7:
                inManagerMenu = false;
                cout << endl;
                break;
            default:
                std::cout << "��Чѡ��������ѡ��" << std::endl;
        }
    }
}


int main() {
    
	SQLRETURN ret; // ���ý��
    /* �������ͱ��� */
    SQLHENV src_env;  // �������
    SQLHDBC src_dbc;  // ���Ӿ�� 
    SQLHSTMT src_stmt; // ����� 

    int roleChoice;
    bool running = true;
    std::string username, password;

    while (running) {
    	std::cout << "===========��ӭ��¼���й���ϵͳ============" << std::endl;
        std::cout << "��ѡ���ɫ: 1. �˿� 2. Ա�� 3. ���� 4. �˳�" << std::endl;
        std::cin >> roleChoice;
        // ���������ǻ����û�������û���������
	    std::string username, password;
	    std::cout << "�������˻�: ";
	    std::cin >> username;
	    std::cout << "����������: ";
	    std::cin >> password;
	    
	
	    /* ��ʼ������ */
	    SQLAllocEnv(&src_env);
	    // ���ù�����������
	    SQLSetEnvAttr(src_env, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
	
	    /* �������� */
	    // �������Ӿ��
	    ret = SQLAllocConnect(src_env, &src_dbc);
	
	    ret = SQLConnect(src_dbc, (SQLCHAR *)"supermarket_in_mysql80", SQL_NTS, 
	                     (SQLCHAR *)username.c_str(), SQL_NTS, 
	                     (SQLCHAR *)password.c_str(), SQL_NTS);
	
	    if (!SQL_SUCCEEDED(ret)) {
	        std::cout << "�˻����������\n" << std::endl;
	        /* �Ͽ����� */
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
                std::cout << "��Чѡ��������ѡ��" << std::endl;
        }
    }
    

    /* �Ͽ����� */
    SQLDisconnect(src_dbc);
    SQLFreeConnect(src_dbc);
    SQLFreeEnv(src_env);

    return 0;
}

