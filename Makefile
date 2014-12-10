# MODULES = mongo
MODULE_big = mongo
EXTENSION = mongo
DATA = mongo--1.0.sql

OBJS = mongo.o

PG_CPPFLAGS = `pkg-config --cflags --libs libmongoc-1.0`
PG_LIBS = `pkg-config --cflags --libs libmongoc-1.0`
SHLIB_LINK = `pkg-config --cflags --libs libmongoc-1.0`


PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
