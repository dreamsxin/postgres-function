#include "postgres.h"
#include "utils/builtins.h"
#include "mongo.h"

#if PG_VERSION_NUM >= 90300
	#include "access/htup_details.h"
#endif

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

#define GET_STR(v)  \
	DatumGetCString(DirectFunctionCall1(varcharout, PointerGetDatum(v)))

#define SET_STR(v) \
	DatumGetTextP(DirectFunctionCall1(varcharin, CStringGetDatum(v)))

PG_FUNCTION_INFO_V1(mongo_find);
PG_FUNCTION_INFO_V1(mongo_save);

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

	client = mongoc_client_new (uri);
	collection = mongoc_client_get_collection (client, dbname, collectionname);

	cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
	if (mongoc_cursor_next(cursor, &doc)) {
		str = bson_as_json(doc, &len);
		data = palloc0(len * sizeof(char) + 1);
		strncpy(data, str, len);
		bson_free(str);
	}

end:
    if (query)
        bson_destroy (query);

    if (cursor)
		mongoc_cursor_destroy (cursor);
    if (collection)
		mongoc_collection_destroy (collection);
    if (client)
		mongoc_client_destroy (client);

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
        bson_destroy (document);
    if (query)
        bson_destroy (query);
    if (update)
        bson_destroy (update);

    if (cursor)
		mongoc_cursor_destroy (cursor);
    if (collection)
		mongoc_collection_destroy (collection);
    if (client)
		mongoc_client_destroy (client);

    PG_RETURN_BOOL(ret);
}