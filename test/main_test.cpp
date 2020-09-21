#include "gtest/gtest.h"
#include "../src/Sqlite.h"
#include "util.cpp"
#include "queries.cpp"
#include <chrono>
#include <thread>

/**
 * this time is used to keep
 * the db in simulateBusyDbForOneSecond() busy
 * for the specified amount of milliseconds.
 */
const int TIME_MILLIS_BUSY_DB = 100;


/******************/
/* Test Databases */
/******************/
/**
 * test.db is used for testcases which want to create their
 * own db. the testcases have to explicitly delete the db afterwards.
 */
const char* TEST_DB_FILENAMENAME = "test.db";

// general purpose DB which everyone can access 
const char* GENERAL_PURPOSE_DB_FILENAME = "sharedTest.db";
static gre90r::Sqlite* db = NULL;


/********************/
/* prepare testcase */
/********************/
/**
 * reset test db to initial state
 */
static void testDbFreshStart() {
  // delete previous employees from table
  db->execute(QUERY_DELETE_TABLE_EMPLOYEE);

  // create table Employee
  if (db->execute(QUERY_CREATE_TABLE_EMPLOYEE) != SQLITE_OK) {
    FAIL() << "[TEST_ERROR] testDbFreshStart: could not create table Employee.";
  }
}


/**********/
/* helper */
/**********/
/**
 * keep the database busy for one second.
 * this is done by locking the database with
 * an open transaction.
 */
void simulateBusyDbForOneSecond(gre90r::Sqlite* _db) {
  // validate function call
  if (_db == NULL) {
    FAIL() << "[TEST_ERROR] simulateBusyDb: null db" << std::endl;
  }
  if (!_db->isConnected()) {
    FAIL() << "[TEST_ERROR] simulateBusyDb: db not connected" << std::endl;
  }

  /////////////
  // lock db //
  /////////////
  _db->execute("BEGIN TRANSACTION;"); // locks db in our sqlite implementation

  // // lock will be acquired when the first SELECT statement is executed
  // // @see https://www.sqlite.org/lockingv3.html - 7.0 Transaction Control At The SQL Level
  _db->execute(QUERY_CREATE_TABLE_EMPLOYEE);
  _db->execute(QUERY_INSERT_INTO_EMPLOYEE_JOHN);
  _db->execute(QUERY_SELECT_NAME_FROM_EMPLOYEE);
  println("[DEBUG] DB locked");

  sleepMilliseconds(TIME_MILLIS_BUSY_DB);

  // // prepare timers
  // std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
  // startTime = std::chrono::system_clock::now();
  // int elapsedSeconds = 0;
  // int executedStatements = 0;

  // // execute write statements to lock db
  // do {
  //   if (_db != NULL && _db->isConnected()) { // in case parent thread closes db in the meanwhile
  //     _db->execute(QUERY_DELETE_TABLE_EMPLOYEE);
  //     _db->execute(QUERY_CREATE_TABLE_EMPLOYEE);
  //     _db->execute(QUERY_INSERT_INTO_EMPLOYEE_JOHN);
  //     _db->execute("COMMIT;");
  //     _db->execute(QUERY_SELECT_NAME_FROM_EMPLOYEE);
  //     println("query execution lap #" << ++executedStatements);
  //   }
  //   elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
  // } while(elapsedSeconds < SLEEP_TIME_MILLISECONDS_BUSY_DB);

  ///////////////
  // unlock db //
  ///////////////
  if (_db != NULL && _db->isConnected()) {
    _db->execute("COMMIT;");
    println("[DEBUG] transaction committed");
    _db->execute("END TRANSACTION;");
  }
  println("[DEBUG] DB unlocked");
}


/********************/
/* SetUp & TearDown */
/********************/
class Environment : public ::testing::Environment {
  public:
    ~Environment() override {}

    /**
     * equivalent to the Java @BeforeAll. this is called
     * once before the tests start.
     */
    void SetUp() override {
      // create general purpose test db
      db = new gre90r::Sqlite(GENERAL_PURPOSE_DB_FILENAME);
    }

    /**
     * deletes test files.
     * 
     * equivalent to the Java @AfterAll. this is called
     * after all tests have been run.
     */
    void TearDown() override {
      // delete general purpose test db
      db->close();
      delete db;

      // delete db files
      deleteFile(TEST_DB_FILENAMENAME);
      deleteFile(GENERAL_PURPOSE_DB_FILENAME);
    }
};


/**************************/
/* Test Suite: DB connect */
/**************************/
/**
 * connect to db file
 */
TEST(dbConnect, connectToExistingFile) {
  createFile(TEST_DB_FILENAMENAME);
  if (!fileExists(TEST_DB_FILENAMENAME)) {
    FAIL() << "[ERROR] file was not created, so cannot read db file." << std::endl;
  }
  gre90r::Sqlite sqlite(TEST_DB_FILENAMENAME);
  ASSERT_EQ(true, sqlite.isConnected());
  ASSERT_STREQ(TEST_DB_FILENAMENAME, sqlite.getName());
}
/**
 * connect to db without existing file.
 * that is the case on first program start.
 * in that case it will create a file.
 */
TEST(dbConnect, connectWithoutFile) {
  deleteFile(TEST_DB_FILENAMENAME);
  if (fileExists(TEST_DB_FILENAMENAME)) {
    FAIL() << "[ERROR] db file should not exist in this test case." << std::endl;
  }
  gre90r::Sqlite sqlite(TEST_DB_FILENAMENAME);
  ASSERT_EQ(true, sqlite.isConnected());
}
/**
 * connect to db with empty filename
 * 
 * shall return isConnected true because DB
 * is held in memory then
 */
TEST(dbConnect, connectWithoutDbName) {
  gre90r::Sqlite sqlite("");
  ASSERT_EQ(true, sqlite.isConnected());
}
/**
 * connect to db when filename is null.
 * in that case the db is held in memory
 * 
 * shall return isConnected true because DB
 * is held in memory then
 */
TEST(dbConnect, connectNameIsNull) {
  gre90r::Sqlite sqlite(NULL);
  ASSERT_EQ(true, sqlite.isConnected());
}
/**
 * when no db file can be created (due to permission denied)
 */
TEST(dbConnect, createDbFilePermissionDenied) {
  gre90r::Sqlite sqlite("/root/test.db");
  ASSERT_EQ(false, sqlite.isConnected());
}


/************************/
/* Test Suite: DB close */
/************************/
/**
 * close a regulare sqlite connection
 */
TEST(dbClose, currentlyConnected) {
  gre90r::Sqlite sqlite(TEST_DB_FILENAMENAME);
  ASSERT_EQ(true, sqlite.isConnected());
  sqlite.close();
  ASSERT_EQ(false, sqlite.isConnected());
}
/**
 * close an already closed connection
 */
TEST(dbClose, currentlyNotConnected) {
  gre90r::Sqlite sqlite(TEST_DB_FILENAMENAME);
  ASSERT_EQ(true, sqlite.isConnected());
  sqlite.close();
  ASSERT_EQ(false, sqlite.isConnected());
  sqlite.close();
  ASSERT_EQ(false, sqlite.isConnected());
}
/**
 * close when db file has been deleted meanwhile
 */
TEST(dbClose, dbFileDoesNotExistAnymore) {
  // db file has been created
  gre90r::Sqlite sqlite(TEST_DB_FILENAMENAME);

  deleteFile(TEST_DB_FILENAMENAME);
  sqlite.close();

  ASSERT_FALSE(sqlite.isConnected());
}
/**
 * close db when db is currently working.
 * cannot close when db is busy.
 */
TEST(dbClose, busyDb) {
  // connect to new db
  gre90r::Sqlite busyDb(TEST_DB_FILENAMENAME);
  // run busyDb in own thread. automatically starts.
  std::thread threadBusyDb(simulateBusyDbForOneSecond, &busyDb);
  // wait until db is surely locked
  unsigned int timeWaitForLock = TIME_MILLIS_BUSY_DB / 2;
  sleepMilliseconds(timeWaitForLock);
  // while db is locked, try to close it
  busyDb.close();
  // wait until db is unlocked
  sleepMilliseconds(TIME_MILLIS_BUSY_DB);
  // db should not be closed while it is busy
  ASSERT_TRUE(busyDb.isConnected());
  // wait for thread to finish
  threadBusyDb.join();
}


/***********************/
/* Test Suite: execute */
/***********************/
/**
 * create a valid sql table
 */
TEST(dbExecute, createTable) {
  // reset test db to initial state
  testDbFreshStart();

  // check row size is 0 & table exists
  gre90r::SqlResult result = db->select(QUERY_SELECT_NAME_FROM_EMPLOYEE);
  ASSERT_EQ(0, result.size());
}
/**
 * try to create a table with invalid sql
 */
TEST(dbExecute, invalidSql) {
  ASSERT_TRUE(SQLITE_OK != db->execute("creete taple employee(id int primary key not null, name char[50] not null);"));
}
/**
 * run a valid sql insert statement
 */
TEST(dbExecute, insert) {
  // reset test db to initial state
  testDbFreshStart();

  // insert into table Employee
  ASSERT_EQ(SQLITE_OK, db->execute(QUERY_INSERT_INTO_EMPLOYEE_JEFF));
}
/**
 * execute a query which also uses the callback to print results
 */
TEST(dbExecute, queryWithPrintResults) {
  // reset test db to initial state
  testDbFreshStart();

  // insert employees
  db->execute(QUERY_INSERT_INTO_EMPLOYEE_JOHN);
  db->execute(QUERY_INSERT_INTO_EMPLOYEE_JEFF);

  // select all employees
  int rc = db->execute(QUERY_SELECT_NAME_FROM_EMPLOYEE);

  ASSERT_EQ(SQLITE_OK, rc);
}
/**
 * execute null
 */
TEST(dbExecute, nullInput) {
  int rc = db->execute(NULL);
  ASSERT_EQ(-2, rc);
}
/**
 * execute on closed connection
 */
TEST(dbExecute, closedConnection) {
  gre90r::Sqlite sqlite(TEST_DB_FILENAMENAME);
  sqlite.close();
  int rc = sqlite.execute(QUERY_CREATE_TABLE_EMPLOYEE);
  ASSERT_EQ(-3, rc);
}


/**********************/
/* Test Suite: select */
/**********************/
/**
 * run a valid sql select statement
 */
TEST(dbSelect, legitSelects) {
  // reset test db to initial state
  testDbFreshStart();

  // select from empty table Employee
  gre90r::SqlResult resultSet;
  resultSet = db->select(QUERY_SELECT_NAME_FROM_EMPLOYEE);
  ASSERT_EQ(0, resultSet.size());

  // insert row & select it
  db->execute(QUERY_INSERT_INTO_EMPLOYEE_JOHN);
  resultSet = db->select(QUERY_SELECT_NAME_FROM_EMPLOYEE);
  ASSERT_EQ(1, resultSet.size());
  
  for (auto result : resultSet) {
    // reads only first element and we only added one, so it is the inserted employee.
    ASSERT_EQ(EMPLOYEE_JOHN, result.second );
    break;
  }
}
/**
 * call a select statement when not connected
 * to database.
 */
TEST(dbSelect, notConnected) {
  gre90r::Sqlite sqlite(TEST_DB_FILENAMENAME);
  sqlite.close();
  gre90r::SqlResult resultSet = sqlite.select(QUERY_SELECT_NAME_FROM_EMPLOYEE);
  ASSERT_EQ(0, resultSet.size()); // expect empty resultset
}


/********/
/* main */
/********/
int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);

  // adding setup & teardown
  ::testing::AddGlobalTestEnvironment(new Environment);

  return RUN_ALL_TESTS();
}
