-- 创建顾客用户
CREATE USER 'customer_0'@'localhost' IDENTIFIED BY '123456';

-- 分配顾客权限：查看商品、插入订单、插入订单明细
GRANT SELECT ON supermarket.Goods TO 'customer_0'@'localhost'; 
GRANT INSERT ON supermarket.Orders TO 'customer_0'@'localhost';
GRANT INSERT ON supermarket.OrderDetail TO 'customer_0'@'localhost';
GRANT EXECUTE ON PROCEDURE supermarket.GetGoodsInfo TO 'customer_0'@'localhost';
GRANT EXECUTE ON PROCEDURE supermarket.PlaceOrder TO 'customer_0'@'localhost';


-- 创建员工用户
CREATE USER 'employee_0'@'localhost' IDENTIFIED BY '123456'; 

-- 分配员工权限：查看和修改商品、插入订单、插入订单明细
GRANT SELECT, UPDATE ON supermarket.Goods TO 'employee_0'@'localhost';
GRANT INSERT ON supermarket.Orders TO 'employee_0'@'localhost';
GRANT INSERT ON supermarket.OrderDetail TO 'customer_0'@'localhost';
GRANT EXECUTE ON PROCEDURE supermarket.CheckStockOut TO 'employee_0'@'localhost';
GRANT EXECUTE ON PROCEDURE supermarket.RestockGoods TO 'employee_0'@'localhost';
GRANT EXECUTE ON PROCEDURE supermarket.CheckExpiredGoods TO 'employee_0'@'localhost';
GRANT EXECUTE ON PROCEDURE supermarket.ClearExpiredGoods TO 'employee_0'@'localhost';


-- 创建经理用户
CREATE USER 'manager_0'@'localhost' IDENTIFIED BY '123456'; 

-- 分配经理权限：全部权限
GRANT ALL PRIVILEGES ON supermarket.* TO 'manager_0'@'localhost';


-- 应用权限更改
FLUSH PRIVILEGES;
