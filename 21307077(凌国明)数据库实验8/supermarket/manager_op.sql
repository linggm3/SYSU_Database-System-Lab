DELIMITER $$

CREATE PROCEDURE GetSalesReport(IN start_date DATE, IN end_date DATE)
BEGIN
    -- 查询指定日期范围内的商品销售情况
    SELECT 
        g.goods_id, 
        g.name, 
        SUM(od.quantity) AS total_quantity, 
        SUM(od.quantity * g.selling_price) AS total_sales, 
        SUM(od.quantity * (g.selling_price - g.purchase_price)) AS total_profit,
        g.max_on_shelf_quan,
        g.stock_quantity
    FROM 
        OrderDetail od
    JOIN 
        Orders o ON od.order_id = o.order_id
    JOIN 
        Goods g ON od.goods_id = g.goods_id
    WHERE 
        o.order_date BETWEEN start_date AND end_date
    GROUP BY 
        g.goods_id
    ORDER BY 
        g.goods_id;
END$$

DELIMITER ;


DELIMITER $$

CREATE PROCEDURE FireEmployee(
    IN employeeUsername VARCHAR(255)
)
BEGIN
    DECLARE exit handler for sqlexception
    BEGIN
        -- 错误处理
        ROLLBACK;
    END;

    START TRANSACTION;

    -- 从数据库中删除员工账户
    SET @s = CONCAT('DROP USER ', employeeUsername, '@\'localhost\'');
    PREPARE stmt FROM @s;
    EXECUTE stmt;
    DEALLOCATE PREPARE stmt;

    COMMIT;
END$$

DELIMITER ;


DELIMITER $$

CREATE PROCEDURE HireEmployee(
    IN newEmployeeUsername VARCHAR(255)
)
BEGIN
    DECLARE exit handler for sqlexception
    BEGIN
        -- 错误处理
        ROLLBACK;
    END;

    START TRANSACTION;

    -- 创建新员工账户并设置默认密码为 '123456'
    SET @s = CONCAT('CREATE USER ', newEmployeeUsername, '@\'localhost\' IDENTIFIED BY \'123456\'');
    PREPARE stmt FROM @s;
    EXECUTE stmt;
    DEALLOCATE PREPARE stmt;

    -- 分配员工权限：查看和修改商品、插入订单、插入订单明细
    SET @s = CONCAT('GRANT SELECT, UPDATE ON supermarket.Goods TO ', newEmployeeUsername, '@\'localhost\'');
    PREPARE stmt FROM @s;
    EXECUTE stmt;
    DEALLOCATE PREPARE stmt;

    SET @s = CONCAT('GRANT INSERT ON supermarket.Orders TO ', newEmployeeUsername, '@\'localhost\'');
    PREPARE stmt FROM @s;
    EXECUTE stmt;
    DEALLOCATE PREPARE stmt;

    SET @s = CONCAT('GRANT INSERT ON supermarket.OrderDetail TO ', newEmployeeUsername, '@\'localhost\'');
    PREPARE stmt FROM @s;
    EXECUTE stmt;
    DEALLOCATE PREPARE stmt;

    -- 授予对特定存储过程的执行权限
    SET @s = CONCAT('GRANT EXECUTE ON PROCEDURE supermarket.CheckStockOut TO ', newEmployeeUsername, '@\'localhost\'');
    PREPARE stmt FROM @s;
    EXECUTE stmt;
    DEALLOCATE PREPARE stmt;

    SET @s = CONCAT('GRANT EXECUTE ON PROCEDURE supermarket.RestockGoods TO ', newEmployeeUsername, '@\'localhost\'');
    PREPARE stmt FROM @s;
    EXECUTE stmt;
    DEALLOCATE PREPARE stmt;

    SET @s = CONCAT('GRANT EXECUTE ON PROCEDURE supermarket.CheckExpiredGoods TO ', newEmployeeUsername, '@\'localhost\'');
    PREPARE stmt FROM @s;
    EXECUTE stmt;
    DEALLOCATE PREPARE stmt;

    SET @s = CONCAT('GRANT EXECUTE ON PROCEDURE supermarket.ClearExpiredGoods TO ', newEmployeeUsername, '@\'localhost\'');
    PREPARE stmt FROM @s;
    EXECUTE stmt;
    DEALLOCATE PREPARE stmt;

    COMMIT;
END$$

DELIMITER ;

