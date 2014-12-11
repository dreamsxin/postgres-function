postgres-function
=================

```shell
git clone https://github.com/dreamsxin/postgres-function.git
cd postgres-function
cd mongo-c-driver-1.0.2
autoreconf -ivf .
./configure && make && sudo make install
cd ..
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
)->'postgresql';

SELECT mongo_save(
    'mongodb://127.0.0.1:27017',
    'database',
    'collection',
    (SELECT row_to_json(t) FROM (SELECT id as user_id, phone FROM users WHERE id=1) t),
    '{"user_id":1}'
);

SELECT * FROM mongo_find_all(
    'mongodb://127.0.0.1:27017',
    'database',
    'collection',
    '{}'
);
```