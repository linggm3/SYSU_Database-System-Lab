DROP TABLE IF EXISTS orderdetail;
DROP TABLE IF EXISTS orders;
DROP TABLE IF EXISTS customer;
DROP TABLE IF EXISTS employee;
DROP TABLE IF EXISTS manager;
DROP TABLE IF EXISTS goods;


-- 创建顾客表
CREATE TABLE Customer (
    customer_id INT AUTO_INCREMENT PRIMARY KEY, -- 顾客ID，主键，自动增长
    name VARCHAR(255) NOT NULL,                 -- 姓名，非空
    contact_info VARCHAR(255),                  -- 联系方式
    address VARCHAR(255),                       -- 地址
		INDEX (customer_id),                        -- 主键索引
    INDEX (name)                                -- 基于姓名的二级索引
);

-- 创建经理表
CREATE TABLE Manager (
    manager_id INT AUTO_INCREMENT PRIMARY KEY,  -- 经理ID，主键，自动增长
    name VARCHAR(255) NOT NULL,                 -- 姓名，非空
    contact_info VARCHAR(255),                  -- 联系方式
    responsibility VARCHAR(255),                 -- 职责
		INDEX (manager_id)                          -- 主键索引
);


-- 创建员工表
CREATE TABLE Employee (
    employee_id INT AUTO_INCREMENT PRIMARY KEY, -- 员工ID，主键，自动增长
    name VARCHAR(255) NOT NULL,                 -- 姓名，非空
    contact_info VARCHAR(255),                  -- 联系方式
    position VARCHAR(255),                      -- 职位
    working_hours VARCHAR(255),                 -- 工作时间
		boss_id INT,                                -- 老板ID，外键
		FOREIGN KEY (boss_id) REFERENCES Manager(manager_id),  -- 外键约束
		INDEX (employee_id),                        -- 主键索引
    INDEX (position)                            -- 基于职位的二级索引
);


-- 创建商品表
CREATE TABLE Goods (
    goods_id INT AUTO_INCREMENT PRIMARY KEY,    -- 商品ID，主键，自动增长
    name VARCHAR(255) NOT NULL,                 -- 名称，非空
		type VARCHAR(255),                          -- 名称
    purchase_price DECIMAL(10, 2),              -- 进价
    selling_price DECIMAL(10, 2),               -- 售价
    expiration_date DATE,                       -- 过期日期
		placement VARCHAR(255),                     -- 摆放位置
    max_on_shelf_quan INT CHECK(max_on_shelf_quan >= 0),  -- 最大上架数量
    shelf_quantity INT CHECK(shelf_quantity >= 0),        -- 货架上数量
    stock_quantity INT CHECK(stock_quantity >= 0),        -- 库存数量
		INDEX (goods_id),                           -- 主键索引
    INDEX (name)                                -- 基于名称的二级索引
);


-- 创建订单表
CREATE TABLE Orders (
    order_id INT AUTO_INCREMENT PRIMARY KEY,    -- 订单号，主键，自动增长
    order_date DATE NOT NULL,                   -- 日期，非空
    total_price DECIMAL(10, 2),                 -- 总售价
    total_profit DECIMAL(10, 2),                -- 总利润
    is_clearance BOOLEAN,                       -- 是否是过期清理
    customer_id INT,                            -- 顾客ID，外键
		employee_id INT DEFAULT(NULL),              -- 员工ID，外键
    FOREIGN KEY (customer_id) REFERENCES Customer(customer_id),  -- 外键约束
		FOREIGN KEY (employee_id) REFERENCES Employee(employee_id),  -- 外键约束
		INDEX (order_id),                           -- 主键索引
    INDEX (order_date),                         -- 基于日期的二级索引
    INDEX (customer_id)                         -- 基于顾客ID的二级索引
);


-- 创建商品订单明细表
CREATE TABLE OrderDetail (
    detail_id INT AUTO_INCREMENT PRIMARY KEY,   -- 明细ID，主键，自动增长
    order_id INT,                               -- 订单号，外键
    goods_id INT,                               -- 商品ID，外键
    quantity INT,                               -- 数量
    FOREIGN KEY (order_id) REFERENCES Orders(order_id),          -- 订单号外键约束
    FOREIGN KEY (goods_id) REFERENCES Goods(goods_id),           -- 商品ID外键约束
		INDEX (detail_id),                          -- 主键索引
		INDEX (order_id),                           -- 基于订单号的二级索引
		INDEX (goods_id)                            -- 基于商品号的二级索引
);

