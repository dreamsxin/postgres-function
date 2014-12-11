/* mongo/mongo--1.0.sql */

-- Copyright (c) 2012-2014 Citus Data, Inc.

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION mongo" to load this file. \quit

CREATE FUNCTION mongo_find(_uri character varying, _database character varying, _collection character varying, _query json)
RETURNS json
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FUNCTION mongo_save(_uri character varying, _database character varying, _collection character varying, _data json, _query json)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FUNCTION mongo_find_all(_uri character varying, _database character varying, _collection character varying, _query json)
RETURNS SETOF json
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;
