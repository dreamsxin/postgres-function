#ifndef PG_MONGO_H
#define PG_MONGO_H

#include "bson.h"
#include "mongoc.h"

#include "fmgr.h"

extern Datum mongo_find(PG_FUNCTION_ARGS);
extern Datum mongo_save(PG_FUNCTION_ARGS);

#endif   /* PG_MONGO_H */