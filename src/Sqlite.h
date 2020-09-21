#ifndef SQLITEWRAPPER_H
#define SQLITEWRAPPER_H

#include <sqlite3.h>
#include <string>
#include <map>


namespace gre90r {

	/**
	 * the rows an sql statement returns
	 * std::map<column,value>
	 */
	typedef std::map<std::string,std::string> SqlResult;

	/**
	 * sqlite3 wrapper
	 */
	class Sqlite {
	public:
		/**
		 * forbid standard constructor
		 */
		Sqlite() = delete;

		/**
		 * create or open sqlite database by filename.
		 * if database with filename does not exist, it
		 * will create a new database
		 * @param filename filename of the database. if NULL it will create
		 * 				a database only in memory
		 */
		Sqlite(const char* filename);

		/**
		 * forbid copy constructor
		 */
		Sqlite(const Sqlite&) = delete;

		/**
		 * disconnect from database & cleanup
		 */
		virtual ~Sqlite();

		/**
		 * forbid assignment operator
		 */
		Sqlite& operator=(const Sqlite&) = delete;

		/**
		 * check if a database connection is open
		 * @return true: database is connected and ready to execute statements.
		 * 				 false: database failed to connect or has been closed.
		 */
		bool isConnected() const;

		/**
		 * @return db name, which is the filename
		 */
		const char* getName() const;

		/**
		 * close db connection.
		 * 
		 * does not close on open transaction. COMMIT or ROLLBACK
		 * your transaction first.
		 */
		void close();

		/**
		 * run a query on db
		 * @param query any sql statement
		 * @return sql error code. 0 is ok. != 0 is failure. refer to
		 *         http://www.sqlite.org/c3ref/c_abort.html for sql error codes.
		 * 				 if it's a negative error code, then it's not an sql error:
		 * 				 -1: unknown error
		 * 				 -2: no query supplied, query is null.
		 *				 -3: not connected to database
		 */
		int execute(const char* query);

		/**
		 * an sql select statement which returns rows
		 * @param query an sql select statement
		 * @return sql result set. an SqlResult array
		 */
		SqlResult select(const char* query);

	private:
		/**************/
		/* Attributes */
		/**************/
		sqlite3* m_db; // the database
		bool m_connected; // status if this application is currently connected to a database 
		SqlResult m_resultSet; // saves the last query result
		const char* m_name;

		/*******************/
		/* private Methods */
		/*******************/
		/**
		 * used as callback for sqlite query executions.
		 * prints each row returned by the query.
		 * runs once for each line returned.
		 * @param data Data provided in the 4th argument of sqlite3_exec().
		 * 				this is not used.
		 * @param argc the number of columns in row
		 * @param argv value of column
		 * @param colNames an array of strings representing column names
		 * @return error code. 0 is OK. everything != 0 is an error.
		 * 				 RC 1: invalid number of rows
		 * 				 RC 2: no rows to process
		 * 				 RC 3: column names missing
		 */
		static int callbackPrintQueryResults(void* data, int argc, char** argv, char** colNames);

		/**
		 * saves each row from the query result in the class attribute 'm_resultSet', one
		 * element the 'm_resultSet' array represents one row.
		 * @param resultsetBuffer the result set of the query will be written in here.
		 * 				this parameter equals the data provided in the 4th argument of sqlite3_exec().
		 * @param argc the number of columns in row
		 * @param argv values of columns. argv[i] is the value of one column
		 * @param colNames an array of strings representing column names
		 * @return error code. 0 is OK. everything != 0 is an error.
		 * 				 RC 1: invalid number of rows
		 * 				 RC 2: no rows to process
		 * 				 RC 3: column names missing
		 *				 RC 4: no resultset buffer
		 */
		static int callbackSaveQueryResults(void* resultsetBuffer, int argc, char** argv, char** colNames);
	};

}

#endif
