

-- выделить часть ГОД из даты
select date_part('year', order_date)
from orders;


select * from employees
where birth_date > '1952-02-19'
order by birth_date;


select
    city,
    coalesce(region, 'нет региона')     --Заменяет NULL значение в столбце на указанное
from employees;

--Создание строки по формату
select
    city,
    concat(city, '****', address) as concat_City_address,
    format('City= %s  Addr= %s', city, address) as format_City_ddress
from employees
limit 2;

-- Самый дорогой заказ из Парижа
select employee_id, ship_city, order_date
from orders
where ship_city = 'Paris'
order by order_date desc
limit 1;

-- Статистика продаж по городам
select ship_city, count(*), min(freight), max(freight)
from orders
group by ship_city;


--В какой категории больше всего товаров
select category_id, count(*) as product_count
from products
group by category_id
order by product_count desc
limit 1;

--Кол-во товаров проданных в каждую страну
select ship_country, count(*)
from orders
group by ship_country
order by ship_country;

-- Order by по 2-ум значениям
--Кол-во товаров проданных по годам в каждую страну
select ship_country, date_part('year', order_date) as year,  count(*) as count
from orders
group by ship_country, year
order by ship_country;







