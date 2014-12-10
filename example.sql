SELECT mongo_find(
    'mongodb://127.0.0.1:27017',
    'file',
    'images',
    '{"name":"name1"}'
)->'_id'->'$oid';

SELECT mongo_find(
    'mongodb://127.0.0.1:27017',
    'file',
    'images',
    '{"name":"name1"}'
)->'name';
    
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