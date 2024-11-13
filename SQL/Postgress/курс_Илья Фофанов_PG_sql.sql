-----------------------------------------------------------------------
--вставка данных
-----------------------------------------------------------------------
insert into book
values
(1, 'War and peace', '0789652'),
(2, 'Pride and Prejiduse', '0968522'),
(3, 'To Kill a Mockingbird', '0968522');



-----------------------------------------------------------------------
--Добавить FK в готовую таблицу
--Сначал добавим новую колонку (fk_publisher_id), потом 2-ым запросом, добавим ограничение на Fk
------------------------------------------------------------------------
alter table book
add column fk_publisher_id int;

alter table book
add constraint fk_book_publisher
foreign key (fk_publisher_id)
references publisher(publisher_id);


---------------------------------------------------------------------
--Многие ко многим (M2M)
----------------------------------------------------------------------
CREATE TABLE book
(
    book_id int PRIMARY KEY,
    title text NOT NULL,
    isbn text NOT NULL
);
CREATE TABLE author
(
    author_id int PRIMARY KEY,
    full_name text NOT NULL,
    rating real
);
CREATE TABLE book_author
(
    book_id int REFERENCES book(book_id),
    author_id int REFERENCES author(author_id),

    CONSTRAINT book_author_pk PRIMARY KEY(book_id, author_id) -- composite key
)

------------------------------------------------------------------------
--один к одному (O2O), как вырожденное условие (O2M)
---------------------------------------------------------------------
create table person
(
  person_id int primary key,
  first_name varchar(64) not null,
  last_name varchar(64) not null
);
create table passport
(
    passport_id int primary key,
    serial_number int not null,
    registration text not null,
    fk_passport_person int references person(person_id)
);
--добавить данные
insert into person
values
(1, 'Jone', 'Show'),
(2, 'Ned', 'Stark'),
(3, 'Rob', 'Baratheon');
insert into passport
values
(1, 23658, 'Wintrefell', 1),
(2, 36959, 'Wintrefell', 2),
(3, 63982, 'Pole', 3);


---------------------------------------------------------------------
--вывести города без повторов
---------------------------------------------------------------------
SELECT distinct(city)
FROM
customers;


---------------------------------------------------------------------
--вывести кол-во городов без повторов
----------------------------------------------------------------------
SELECT count(distinct(country))
FROM
customers;


--------------------------------------------------------------------
--вывести Заказчиков у которых город не из списка NOT IN
--------------------------------------------------------------------
select *
from customers
where country NOT IN('Mexico','Germany');


--------------------------------------------------------------------
--вывести все страны без повторов в порядке возрастания
--------------------------------------------------------------------
select distinct country
from customers
order by country asc;

--------------------------------------------------------------------
--вывести все страны  без повторов в порядке возрастания стран и городов внутри группы стран.
--------------------------------------------------------------------
select distinct country, city
from customers
order by country desc, city desc;


--------------------------------------------------------------------
--вывести мин. цену продукта, которого в наличии более 30ед  (БЕЗ ФУНКЦИЙ АГРЕГИРОВАНИЯ НЕ МОЖЕМ ВЫВЕСТИ КАКОЙ ИМЕННО ПРОДУКТ)
--------------------------------------------------------------------
select Min(unit_price)
from products
where units_in_stock > 30;


--------------------------------------------------------------------
--Среднее вермя доставки для товаров в USA
--------------------------------------------------------------------
select avg(shipped_date-order_date)
from orders
where ship_country = 'USA';


--------------------------------------------------------------------
--CASE WHEN
--------------------------------------------------------------------
SELECT product_name, unit_price, units_in_stock,
        CASE WHEN units_in_stock > 100 THEN 'lots of'
             WHEN units_in_stock >= 50 AND  units_in_stock < 100 THEN 'average'
             ELSE 'low number'
        END AS amount
FROM products
ORDER BY units_in_stock;


--------------------------------------------------------------------
-- COALESCE
-- Функция coalescec заменяет NULL аргумент на литерал.
-- ship_region который NULL заменится на unknown
--------------------------------------------------------------------
SELECT order_id, order_date, coalesce(ship_region, 'unknown') as ship_region
FROM orders
LIMIT 10;


--------------------------------------------------------------------
--Все города Buenos Aires заменить на Fix
--------------------------------------------------------------------
SELECT contact_name, coalesce(nullif(city, 'Buenos Aires'), 'Fix') as city
FROM customers;


--------------------------------------------------------------------
--Вывести всех заказчиков, отсортировав по contact_name и по city, если city равнен null то по country
--------------------------------------------------------------------
SELECT contact_name, city, country
FROM customers
ORDER BY contact_name,
(
  CASE WHEN city is NULL THEN country
       ELSE city
   END
);


--------------------------------------------------------------------
--Найти заказчиков, не сделавших ни одного заказа. Вывести имя заказчика и значение 'no orders' если order_id = NULL.
-- в функции coalesce нужно передавать аргументы одного типа, поэтому приводим rder_id::text к text
--------------------------------------------------------------------
SELECT contact_name, coalesce(order_id::text, 'no orders')
FROM customers
LEFT JOIN orders o on customers.customer_id = o.customer_id
WHERE order_id is NULL;

--------------------------------------------------------------------
--Вывести ФИО сотрудников и их должности. В случае если должность = Sales Representative вывести вместо неё Sales Stuff.
--------------------------------------------------------------------
SELECT concat(first_name, ' ', last_name) as FIO, coalesce(nullif(title, 'Sales Representative'), 'Sales Stuff') As title
FROM employees;


--------------------------------------------------------------------
--Создать функцию (хранимую процедуру). В таблице tmp_customers region который не задан заменить на unknown
--------------------------------------------------------------------
create function fix_customer_region() returns void
language sql as
$$
UPDATE tmp_customers
SET region= 'unknown'
WHERE region IS NULL
$$;


--------------------------------------------------------------------

--------------------------------------------------------------------



--------------------------------------------------------------------

--------------------------------------------------------------------




--------------------------------------------------------------------

--------------------------------------------------------------------




--------------------------------------------------------------------

--------------------------------------------------------------------




--------------------------------------------------------------------

--------------------------------------------------------------------




--------------------------------------------------------------------

--------------------------------------------------------------------









