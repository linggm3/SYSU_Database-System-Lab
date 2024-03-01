DELIMITER $$

-- 第一个存储过程{用于员工查看所有缺货的商品(即货架上数量 小于 最大上架数量的商品，如果等于则不用输出)，没有输入，
-- 输出是商品的(ID, 名称，种类，位置，货架上数量，库存数量，最大上架数量)，
-- 按照((最大上架数量-货架上数量)/最大上架数量)来降序排列，即缺货情况越严重的商品，排在越前面}
CREATE PROCEDURE CheckStockOut()
BEGIN
    -- 查看所有缺货的商品
    SELECT goods_id, name, category, placement, shelf_quantity, stock_quantity, max_on_shelf_quan,
           ((max_on_shelf_quan - shelf_quantity) / max_on_shelf_quan) AS shortage_ratio
    FROM Goods
    WHERE shelf_quantity < max_on_shelf_quan  -- 选择货架上数量小于最大上架数量的商品
    ORDER BY shortage_ratio DESC;  -- 按缺货比例降序排列
END$$
DELIMITER ;


DELIMITER $$
-- 第二个存储过程{用于员工补充货品，输入商品的id，更新商品id对应商品表的行，
-- 修改在货架上的数量，加上min(库存数量，最大上架数量-当前在货架上的数量)，相应地更新库存数量}
CREATE PROCEDURE RestockGoods(
    IN input_goods_id INT  -- 输入的商品ID
)
BEGIN
    DECLARE current_shelf_quan INT;    -- 当前货架上的数量
    DECLARE current_stock_quan INT;    -- 当前库存数量
    DECLARE max_shelf_quan INT;         -- 最大上架数量
    DECLARE restock_quan INT;          -- 补充数量

    -- 获取当前商品的货架上数量和库存数量
    SELECT shelf_quantity, stock_quantity, max_on_shelf_quan INTO current_shelf_quan, current_stock_quan, max_shelf_quan
    FROM Goods
    WHERE goods_id = input_goods_id;

    -- 计算补充数量为库存数量和(最大上架数量 - 当前货架上数量)的较小值
    SET restock_quan = LEAST(current_stock_quan, max_shelf_quan - current_shelf_quan);

    -- 如果补充数量大于0，则更新货架上的数量和库存数量
    IF restock_quan > 0 THEN
        UPDATE Goods
        SET shelf_quantity = shelf_quantity + restock_quan,
            stock_quantity = stock_quantity - restock_quan
        WHERE goods_id = input_goods_id;
    END IF;
END$$
DELIMITER ;


DELIMITER $$
-- 第三个存储过程{用于查看所有过期的商品，即当前日期超过过期日期的商品}
CREATE PROCEDURE CheckExpiredGoods()
BEGIN
    -- 查看所有过期的商品
    SELECT goods_id, name, expiration_date, shelf_quantity, stock_quantity
    FROM Goods
    WHERE expiration_date < CURDATE() AND shelf_quantity > 0 -- 选择当前日期已超过过期日期的商品
    ORDER BY expiration_date;  -- 按过期日期排序
END$$
DELIMITER ;


DELIMITER $$
-- 第四个存储过程{用于清理过期的商品，输入过期商品的goods_id；为订单明细增加一行：数量为商品在货架上的数量；
-- 为订单表增加一行，售价设置为0，利润设置为商品的数量x商品的进价，表明因商品过期导致亏损}
CREATE PROCEDURE ClearExpiredGoods(
    IN input_goods_id INT  -- 输入的过期商品ID
)
BEGIN
		DECLARE _stock_quantity INT;      -- 商品在仓库中的数量
    DECLARE _shelf_quan INT;          -- 商品在货架上的数量
    DECLARE _purchase_price DECIMAL(10, 2); -- 商品的进价

    -- 获取商品在货架上的数量和进价
    SELECT shelf_quantity, purchase_price, stock_quantity INTO _shelf_quan, _purchase_price, _stock_quantity
    FROM Goods
    WHERE goods_id = input_goods_id;

    -- 如果货架上数量大于0，则进行清理操作
    IF _shelf_quan > 0 THEN
        -- 为订单表增加一行，售价为0，利润为数量乘以进价的负值
        INSERT INTO Orders(order_date, total_price, total_profit, is_clearance, customer_id, employee_id)
        VALUES(CURDATE(), 0, -(_shelf_quan+_stock_quantity) * _purchase_price, TRUE, NULL, 1);

        -- 获取新插入的订单ID
        SET @last_order_id = LAST_INSERT_ID();

        -- 为订单明细表增加一行
        INSERT INTO OrderDetail(order_id, goods_id, quantity)
        VALUES(@last_order_id, input_goods_id, _shelf_quan);
				
				-- 更新Goods表，将货架上的数量和库存数量均设置为0
        UPDATE Goods
        SET shelf_quantity = 0, stock_quantity = 0
        WHERE goods_id = input_goods_id;
    END IF;
END$$

DELIMITER ;
