-- 创建图书(Book)表
CREATE TABLE Book (
    book_id VARCHAR(20) PRIMARY KEY,
    title VARCHAR(100),
    author VARCHAR(100),
    publisher VARCHAR(100),
    is_borrowed BOOLEAN DEFAULT FALSE
);
-- 为图书表的书名、作者和出版社创建非聚集索引
CREATE INDEX idx_book_title ON Book (title);
CREATE INDEX idx_book_author ON Book (author);
CREATE INDEX idx_book_publisher ON Book (publisher);

-- 创建类别(Category)表
CREATE TABLE Category (
    category_id INT PRIMARY KEY,
    name VARCHAR(50)
);
-- 为类别名称创建非聚集索引
CREATE INDEX idx_category_name ON Category (name);

-- 创建读者(Reader)表
CREATE TABLE Reader (
    reader_id INT PRIMARY KEY,
    name VARCHAR(100),
    contact_info VARCHAR(100)
);
-- 为读者姓名创建非聚集索引
CREATE INDEX idx_reader_name ON Reader (name);

-- 创建借阅证(Borrowing Card)表
CREATE TABLE BorrowingCard (
    card_id INT PRIMARY KEY,
    valid_until DATE,
    reader_id INT,
    FOREIGN KEY (reader_id) REFERENCES Reader(reader_id)
);

-- 创建员工(Staff)表
CREATE TABLE Staff (
    staff_id INT PRIMARY KEY,
    name VARCHAR(100),
    position VARCHAR(100)
);

-- 创建借阅记录(Borrowing Record)表
CREATE TABLE BorrowingRecord (
    record_id INT PRIMARY KEY,
    borrow_date DATE,
    renew_date DATE,
    return_date DATE,
    book_id VARCHAR(20),
    card_id INT,
    staff_id INT,
    FOREIGN KEY (book_id) REFERENCES Book(book_id),
    FOREIGN KEY (card_id) REFERENCES BorrowingCard(card_id),
    FOREIGN KEY (staff_id) REFERENCES Staff(staff_id)
);

-- 创建罚款记录(Fine Record)表
CREATE TABLE FineRecord (
    fine_id INT PRIMARY KEY,
    details VARCHAR(255),
    reader_id INT,
    FOREIGN KEY (reader_id) REFERENCES Reader(reader_id)
);

-- 创建位置(Location)表
CREATE TABLE Location (
    location_id INT PRIMARY KEY,
    area VARCHAR(100),
    book_id VARCHAR(20),
    FOREIGN KEY (book_id) REFERENCES Book(book_id)
);

-- 创建图书与类别关系(Book-Category)表
CREATE TABLE BookCategory (
    book_id VARCHAR(20),
    category_id INT,
    FOREIGN KEY (book_id) REFERENCES Book(book_id),
    FOREIGN KEY (category_id) REFERENCES Category(category_id),
    PRIMARY KEY (book_id, category_id)
);


-- 创建触发器：借书时自动更新图书的is_borrowed字段
DELIMITER $$
CREATE TRIGGER trigger_borrow_book
AFTER INSERT ON BorrowingRecord
FOR EACH ROW
BEGIN
    UPDATE Book SET is_borrowed = TRUE WHERE book_id = NEW.book_id;
END$$
DELIMITER ;

-- 创建触发器：还书时自动更新图书的is_borrowed字段
DELIMITER $$
CREATE TRIGGER trigger_return_book
AFTER UPDATE ON BorrowingRecord
FOR EACH ROW
BEGIN
    IF NEW.return_date IS NOT NULL THEN
        UPDATE Book SET is_borrowed = FALSE WHERE book_id = NEW.book_id;
    END IF;
END$$
DELIMITER ;

-- 创建视图：查询各种图书的书号、书名、总数和在册数（未被借出的图书数）
CREATE VIEW view_book_details AS
SELECT book_id, title, COUNT(*) AS total_count, SUM(NOT is_borrowed) AS available_count
FROM Book
GROUP BY book_id, title;