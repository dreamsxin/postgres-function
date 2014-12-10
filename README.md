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
    'database',
    'collection',
    '{"field":"value"}',
    '{"query filed":"value"}'
);

SELECT mongo_find(
    'mongodb://127.0.0.1:27017',
    'database',
    'collection',
    '{"query field":"value"}'
)->'postgresql'
```