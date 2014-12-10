#include "postgres.h"
#include <string.h>
#include "fmgr.h"

#include "bson.h"
#include "mongoc.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(mongo_save);

Datum
mongo_save(PG_FUNCTION_ARGS)
{
	char const* uri = PG_GETARG_CSTRING(0);

    PG_RETURN_BOOL(true);
}