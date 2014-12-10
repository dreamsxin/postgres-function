/* mongo/mongo--1.0.sql */

-- Copyright (c) 2012-2014 Citus Data, Inc.

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION mongo" to load this file. \quit

CREATE FUNCTION mongo_save()
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;
