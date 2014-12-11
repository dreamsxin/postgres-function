#include "postgres.h"
#include "funcapi.h"
#include "utils/builtins.h"
#include "catalog/pg_type.h"

#include "mongo.h"

#if PG_VERSION_NUM >= 90300
	#include "access/htup_details.h"
#endif

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(mongo_find);
PG_FUNCTION_INFO_V1(mongo_save);
PG_FUNCTION_INFO_V1(mongo_find_all);

Datum
mongo_find(PG_FUNCTION_ARGS)
{
	char const* uri = GET_STR(PG_GETARG_CSTRING(0));
	char const* dbname = GET_STR(PG_GETARG_CSTRING(1));
	char const* collectionname = GET_STR(PG_GETARG_CSTRING(2));
	char const* query_data = GET_STR(PG_GETARG_CSTRING(3));
	mongoc_collection_t *collection = NULL;
	mongoc_client_t *client = NULL;
	mongoc_cursor_t *cursor = NULL;
	bson_t *query = NULL;
	const bson_t *doc = NULL;
	bson_error_t error;
	char *data = NULL;
	char *str;
	size_t len;

	query = bson_new();
	if (!bson_init_from_json(query, query_data, strlen(query_data), &error)) {
		ereport(ERROR, (errmsg("%s", error.message)));
		goto end;
	}

	client = mongoc_client_new(uri);
	collection = mongoc_client_get_collection(client, dbname, collectionname);

	cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
	if (mongoc_cursor_next(cursor, &doc)) {
		str = bson_as_json(doc, &len);
		data = palloc0(len * sizeof(char) + 1);
		strncpy(data, str, len);
		bson_free(str);
	}

end:
    if (query)
        bson_destroy(query);

    if (cursor)
		mongoc_cursor_destroy(cursor);
    if (collection)
		mongoc_collection_destroy(collection);
    if (client)
		mongoc_client_destroy(client);

	if (data) {
		PG_RETURN_TEXT_P(SET_STR(data));
	} else {
		PG_RETURN_NULL();
	}
}

Datum
mongo_save(PG_FUNCTION_ARGS)
{
	char const* uri = GET_STR(PG_GETARG_CSTRING(0));
	char const* dbname = GET_STR(PG_GETARG_CSTRING(1));
	char const* collectionname = GET_STR(PG_GETARG_CSTRING(2));
	char const* data = GET_STR(PG_GETARG_CSTRING(3));
	char const* query_data = GET_STR(PG_GETARG_CSTRING(4));
	mongoc_collection_t *collection = NULL;
	mongoc_client_t *client = NULL;
	mongoc_cursor_t *cursor = NULL;
	bson_t *document = NULL;
	bson_t *update = NULL;
	bson_t *query = NULL;
	const bson_t *doc = NULL;
	bson_error_t error;
	bool ret = false;

	document = bson_new_from_json((const uint8_t *)data, strlen(data), &error);
	if (!document) {
		ereport(ERROR, (errmsg("%s", error.message)));
		goto end;
	}

	query = bson_new_from_json((const uint8_t *)query_data, strlen(query_data), &error);
	if (!query) {
		ereport(ERROR, (errmsg("%s", error.message)));
		goto end;
	}

	client = mongoc_client_new (uri);
	collection = mongoc_client_get_collection (client, dbname, collectionname);

	if (!bson_empty(query)) {
		cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
		if (!mongoc_cursor_next(cursor, &doc)) {
			if (mongoc_cursor_error(cursor, &error)) {
				ereport(WARNING, (errmsg("%s", error.message)));
				goto end;
			}
		} else {
			update = bson_new();
			BSON_APPEND_DOCUMENT(update, "$set", document);
			if (!mongoc_collection_update(collection, MONGOC_UPDATE_NONE, query, update, NULL, &error)) {
				ereport(ERROR, (errmsg("%s", error.message)));
			} else {
				ret = true;
			}

			goto end;
		}
	}

	if (!mongoc_collection_insert(collection, MONGOC_INSERT_NONE, document, NULL, &error)) {
		ereport(ERROR, (errmsg("%s", error.message)));
	} else {
		ret = true;
	}

end:
    if (document)
        bson_destroy(document);
    if (query)
        bson_destroy(query);
    if (update)
        bson_destroy(update);

    if (cursor)
		mongoc_cursor_destroy(cursor);
    if (collection)
		mongoc_collection_destroy(collection);
    if (client)
		mongoc_client_destroy(client);

    PG_RETURN_BOOL(ret);
}

Datum
mongo_find_all(PG_FUNCTION_ARGS)
{
    FuncCallContext *funcctx;
	Datum result;
	Datum values[2];
	bool nulls[2];
	HeapTuple tuple;
	MemoryContext oldcontext;
	TupleDesc tupledesc;
	MongoContext *mongo_context = NULL;
	mongoc_collection_t *collection = NULL;
	mongoc_client_t *client = NULL;
	mongoc_cursor_t *cursor = NULL;
	bson_t *query = NULL;
	const bson_t *doc = NULL;
	bson_iter_t iter;
	bson_error_t error;
	char const *uri, *dbname, *collectionname, *query_data;
	char *data = NULL, *str;
	size_t len;

    if (SRF_IS_FIRSTCALL()) {
        funcctx = SRF_FIRSTCALL_INIT();
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
		uri = GET_STR(PG_GETARG_CSTRING(0));
		dbname = GET_STR(PG_GETARG_CSTRING(1));
		collectionname = GET_STR(PG_GETARG_CSTRING(2));
		query_data = GET_STR(PG_GETARG_CSTRING(3));


		query = bson_new();
		if (!bson_init_from_json(query, query_data, strlen(query_data), &error)) {
			ereport(ERROR, (errmsg("%s", error.message)));
		}

		client = mongoc_client_new(uri);
		collection = mongoc_client_get_collection(client, dbname, collectionname);

		cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

		mongo_context = (MongoContext *) palloc0(sizeof(MongoContext));
		mongo_context->client = client;
		mongo_context->collection = collection;
		mongo_context->query = query;
		mongo_context->cursor = cursor;

		if (get_call_result_type(fcinfo, NULL, &tupledesc) != TYPEFUNC_COMPOSITE) {
			ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("function returning record called in context that cannot accept type record")));
		}

		mongo_context->tupdesc = BlessTupleDesc(tupledesc);

		funcctx->user_fctx = mongo_context;

		MemoryContextSwitchTo(oldcontext);
    }

	funcctx = SRF_PERCALL_SETUP();
	mongo_context = funcctx->user_fctx;

    if (mongoc_cursor_next(mongo_context->cursor, &doc)) {
		str = bson_as_json(doc, &len);
		data = palloc0(len * sizeof(char) + 1);
		strncpy(data, str, len);
		bson_free(str);

		values[0] = PointerGetDatum(0);
		nulls[0] = true;

		if (bson_iter_init (&iter, doc) && bson_iter_find(&iter, "id")) {
			if (bson_iter_type(&iter) == BSON_TYPE_INT32) {
				values[0] = PointerGetDatum(bson_iter_int32(&iter));
				nulls[0] = false;
			}
		}

		values[1] = PointerGetDatum(cstring_to_text(data));
		nulls[1] = false;

		tuple = heap_form_tuple(mongo_context->tupdesc, values, nulls);                
        result = HeapTupleGetDatum(tuple);                

        SRF_RETURN_NEXT(funcctx, result);
    } else {
		if (mongo_context) {
			bson_destroy(mongo_context->query);
			mongoc_collection_destroy(mongo_context->collection);
			mongoc_client_destroy(mongo_context->client);

			pfree(mongo_context);
		}

        SRF_RETURN_DONE(funcctx);
    }
}