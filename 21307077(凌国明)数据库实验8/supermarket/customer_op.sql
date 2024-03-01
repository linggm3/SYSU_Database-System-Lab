DELIMITER $$

-- 第一个存储过程{用于查看商品的信息(包括名称，种类，售价，过期日期，货架上数量)
-- 这里的查看是基于名称或种类进行select，结果要按售价升序或降序排序。
-- 输入参数：升序或降序，默认升序；商品名称或商品种类}

-- 顾客查看商品
-- 模式一：按照商品名称查看
-- 模式二：按照商品种类查看
-- 模式三：查看所有商品信息
CREATE PROCEDURE GetGoodsInfo(
    IN orderType VARCHAR(4),  -- 'ASC' 或 'DESC'，用于指定排序方式
    IN searchValue VARCHAR(255), -- 商品名称或商品种类
    IN searchType VARCHAR(9)  -- 'name' 或 'category'，指定搜索类型
)
BEGIN
    IF orderType NOT IN ('ASC', 'DESC') THEN
        SET orderType = 'ASC';  -- 如果输入的排序方式不正确，默认为升序
    END IF;

    IF searchType = 'name' THEN
        -- 基于商品名称搜索
        SELECT name, category, selling_price, expiration_date, shelf_quantity, goods_id, placement
        FROM Goods
        WHERE name LIKE CONCAT('%', searchValue, '%')
        ORDER BY selling_price COLLATE utf8mb4_unicode_ci;
    ELSEIF searchType = 'category' THEN
        -- 基于商品种类搜索
        SELECT name, category, selling_price, expiration_date, shelf_quantity, goods_id, placement
        FROM Goods
        WHERE category LIKE CONCAT('%', searchValue, '%')
        ORDER BY selling_price COLLATE utf8mb4_unicode_ci;
		ELSE
        -- 查看所有商品信息，按照orderType排序
        SELECT name, category, selling_price, expiration_date, shelf_quantity, goods_id, placement
        FROM Goods
        ORDER BY goods_id COLLATE utf8mb4_unicode_ci;

    END IF;
END$$

DELIMITER ;

-- 第二个存储过程{用于顾客下单，为订单表增加一行，为订单明细增加n行。输入参数为goods_id和quantity}
-- 顾客下单并在订单表(Orders)中增加一行，同时在订单明细表(OrderDetail)中增加行

DELIMITER $$

CREATE PROCEDURE PlaceOrder(
    IN input_goods_id INT,
    IN input_quantity INT,
    IN input_customer_id INT
)
BEGIN
		DECLARE new_order_id INT;
    DECLARE v_purchase_price DECIMAL(10, 2);
    DECLARE v_selling_price DECIMAL(10, 2);
    DECLARE v_shelf_quantity INT;
    DECLARE v_total_price DECIMAL(10, 2);
    DECLARE v_total_profit DECIMAL(10, 2);

    -- 尝试获取商品的进价、售价和在架数量
    SELECT purchase_price, selling_price, shelf_quantity 
		INTO v_purchase_price, v_selling_price, v_shelf_quantity 
    FROM Goods 
    WHERE goods_id = input_goods_id;

    -- 检查是否找到了商品和价格，以及在架数量是否足够
    IF v_purchase_price IS NULL OR v_selling_price IS NULL THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = '错误: 商品价格信息不完整或商品不存在';
    ELSEIF v_shelf_quantity < input_quantity THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = '错误: 商品在架数量不足';
    END IF;

    -- 计算总售价和总利润
    SET v_total_price = v_selling_price * input_quantity;
    SET v_total_profit = (v_selling_price - v_purchase_price) * input_quantity;

    -- 插入新订单到Orders表
    INSERT INTO Orders (order_date, total_price, total_profit, is_clearance, customer_id)
    VALUES (CURDATE(), v_total_price, v_total_profit, 0, input_customer_id);

    -- 获取新插入的订单ID
    SET new_order_id = LAST_INSERT_ID();

    -- 插入订单明细到OrderDetail表
    INSERT INTO OrderDetail (order_id, goods_id, quantity)
    VALUES (new_order_id, input_goods_id, input_quantity);
END$$

DELIMITER ;
