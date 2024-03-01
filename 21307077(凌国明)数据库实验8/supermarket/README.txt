按照以下顺序执行 sql query 即可完成数据库的创建
init_table:      初始化六个表，定义完整性，创建数据索引。
create_trigger:  创建触发器，包括下单自动更新货架商品的逻辑
create_user:     创建初始用户，包括顾客、员工、经理
insert_table:    对goods表插入示例数据
customer_op:     定义顾客的存储过程：查看商品，查找商品，购买下单
employee_op:     定义员工的存储过程：查看缺货商品，补充缺货商品，查看过期商品，清理过期商品
manager_op:      定义经理的存储过程：查看一段时间内的销售情况，开除员工，雇佣员工