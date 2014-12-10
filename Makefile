MODULES = mongo
EXTENSION = mongo
DATA = mongo--1.0.sql

PG_CPPFLAGS = -I/usr/include/libmongoc-1.0 -I/usr/include/libbson-1.0
PG_LIBS = -llibmongoc-1.0 -llibbson-1.0

OBJS = mongo.o

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
