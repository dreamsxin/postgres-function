postgres-function
=================

```shell
git clone https://github.com/dreamsxin/postgres-function.git
cd postgres-function
make && sudo make install
```

```sql
CREATE EXTENSION mongo;

SELECT mongo_save(
    'mongodb://127.0.0.1:27017',
    'file',
    'images',
    '{"postgresql":"fine"}',
    '{"name":"name1"}'
);

SELECT mongo_find(
    'mongodb://127.0.0.1:27017',
    'file',
    'images',
    '{"name":"name1"}'
)->'postgresql'
```