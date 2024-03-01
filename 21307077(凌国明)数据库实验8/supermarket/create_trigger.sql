DELIMITER $$

-- 更改订单明细时，减少商品在货架上的数量
CREATE TRIGGER after_orderdetail_insert
AFTER INSERT ON OrderDetail
FOR EACH ROW
BEGIN
    -- 更新Goods表中对应商品的货架上数量
    UPDATE Goods
    SET shelf_quantity = shelf_quantity - NEW.quantity
    WHERE goods_id = NEW.goods_id;
END$$

DELIMITER ;
