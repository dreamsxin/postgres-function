#ifndef PG_MONGO_H
#define PG_MONGO_H

#include "bson.h"
#include "mongoc.h"

#include "fmgr.h"

typedef struct MongoContext {
	mongoc_client_t *client;
	mongoc_collection_t *collection;
	mongoc_cursor_t *cursor;
	bson_t *query;
    TupleDesc tupdesc;
} MongoContext;

extern Datum mongo_find(PG_FUNCTION_ARGS);
extern Datum mongo_save(PG_FUNCTION_ARGS);
extern Datum mongo_find_all(PG_FUNCTION_ARGS);

#define GET_STR(v)  \
	DatumGetCString(DirectFunctionCall1(varcharout, PointerGetDatum(v)))

#define SET_STR(v) \
	DatumGetTextP(DirectFunctionCall1(varcharin, CStringGetDatum(v)))

#endif   /* PG_MONGO_H */