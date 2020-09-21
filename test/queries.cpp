#include <string>

/**
 * create table Employee
 */
const char* QUERY_CREATE_TABLE_EMPLOYEE = 
  "create table employee(" \
    "id     int       primary key not null," \
    "name   char(50)  not null" \
  ");";

/*************/
/* Employees */
/*************/
// IDs have to match the inserts below
const char* EMPLOYEE_JOHN_ID = "1";
const char* EMPLOYEE_JOHN = "John Paul";
const char* EMPLOYEE_JEFF_ID = "2";
const char* EMPLOYEE_JEFF = "Jeff Beck";

/********************/
/* Employee inserts */
/********************/
const char* QUERY_INSERT_INTO_EMPLOYEE_JOHN = 
  "insert into employee (id, name) values (1, 'John Paul')";
const char* QUERY_INSERT_INTO_EMPLOYEE_JEFF =
  "insert into employee (id, name) values (2, 'Jeff Beck')";

/********************/
/* Employee selects */
/********************/
// get all employee names
const char* QUERY_SELECT_NAME_FROM_EMPLOYEE =
  "select name from employee";

/**********************/
/* Employee deletions */
/**********************/
// clear table employee
const char* QUERY_CLEAR_TABLE_EMPLOYEE =
  "delete from employee";

// delete table employee
const char* QUERY_DELETE_TABLE_EMPLOYEE =
  "drop table employee";