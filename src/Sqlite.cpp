#include "Sqlite.h"
#include <iostream>

#define print(s) {  std::cout << s; }
#define println(s) {  std::cout << s << std::endl; }
#define printError(s) {  std::cout << s; }
#define printlnError(s) {  std::cerr << s << std::endl; }

#define UNUSED(variable) { (void)(variable); }


gre90r::Sqlite::Sqlite(const char* filename)
: m_db(NULL), m_connected(false), m_name(filename)
{
	// connect to DB
	int rc = sqlite3_open(filename, &this->m_db);
	if (rc == SQLITE_OK) {
		this->m_connected = true;
		println("connected to DB");
	}
	else {
		this->close(); // free resources if there have been reserved any
		printError("[ERROR] failed to connect to DB. rc = " << rc << ".");
		printlnError("filename was: " << filename);
		printlnError("sqlite error message: " << sqlite3_errmsg(this->m_db));
	}
}


gre90r::Sqlite::~Sqlite() {
	// force closing db
	if (this->m_db != NULL) {
		int rc = sqlite3_close(this->m_db);
		if (rc == SQLITE_OK) {
			std::cout << "disconnected from DB" << std::endl;
		}
		else {
			printlnError("[ERROR] failed to force disconnect from DB: rc = " << rc << ". "
									 << "sqlite error message: " << sqlite3_errmsg(this->m_db));
		}
	}
}


bool gre90r::Sqlite::isConnected() const {
	return this->m_connected;
}


const char* gre90r::Sqlite::getName() const {
	return this->m_name;
}


void gre90r::Sqlite::close() {
	// check if db connection exists
	if (this->m_db == NULL) {
		println("[INFO] DB has already been closed.");
		return;
	}

	// check if a transaction is currently open
	// sqlite3_get_autocommit returns 0 if autocommit is disabled, which
	// means that a transaction has been started with "BEGIN TRANSACTION".
	// autocommit is automatically enabled with a "COMMIT" or "ROLLBACK".
	if (sqlite3_get_autocommit(this->m_db) == 0) {
		println("[INFO] will not close DB. please finish your transaction.");
		return;
	}

	int rc = sqlite3_close(this->m_db);
	if (rc == SQLITE_OK) {
		this->m_connected = false;
		this->m_db = NULL;
		std::cout << "disconnected from DB" << std::endl;
	}
	else {
		printError("[ERROR] failed to disconnect from DB: rc = " << rc << ". ");
		printlnError("sqlite error message: " << sqlite3_errmsg(this->m_db));
	}
}


int gre90r::Sqlite::callbackPrintQueryResults(
		void* data,
		int argc,
		char** argv,
		char** colNames)
{
	// check invalid arguments
	if (argc < 0) {
		return 1; // RC 1: invalid number of rows
	}
	if (argv == NULL) {
		return 2; // RC 2: no rows to process
	}
	if (colNames == NULL) {
		return 3; // RC 3: column names missing
	}

	// ignore 4th argument which is passed by sqlite3_exec.
	// there are no arguments needed for this call
	UNUSED(data);

	// prints every column of row. if attribute is not set
	// then it will be marked as NULL.
	for (int i = 0; i < argc; i++) {
		println(colNames[i] << " -> " << (argv[i] ? argv[i] : "NULL"));
	}

	return 0;
}


int gre90r::Sqlite::execute(const char* query) {
	int rc = -1;

	if (query == NULL) {
		return -2;
	}

	if (this->isConnected()) {
		char* errmsg = 0;
		/**
		 *  sqlite3_exec(
		 * 		sqlite3*,
		 * 		const char *sql,
		 * 		sqlite_callback,
		 * 		void *data,				// ! only pass (const char*)
		 * 		char **errmsg
		 * 	)
		 */
		// 4th arg = NULL -> no data is passed to the callback
		rc = sqlite3_exec(this->m_db, query, callbackPrintQueryResults, &this->m_resultSet, &errmsg);

		if (errmsg) {
			printlnError("[ERROR] query execution returned: " << errmsg << ". rc = " << rc << ".");
		}
	}
	else {
		printlnError("[ERROR] cannot execute query. not connected to DB.");
		return -3;
	}

	return rc; // sql error code
}


int gre90r::Sqlite::callbackSaveQueryResults(void* resultsetBuffer, int argc,
                                             char** argv, char** colNames)
{
	// check invalid arguments
	if (argc < 0) {
		return 1; // RC 1: invalid number of rows
	}
	if (argv == NULL) {
		return 2; // RC 2: no rows to process
	}
	if (colNames == NULL) {
		return 3; // RC 3: column names missing
	}
	if (resultsetBuffer == NULL) {
		return 4; // RC 4: no resultset buffer
	}

	gre90r::SqlResult* results = static_cast<SqlResult*>(resultsetBuffer);

	// prints every column of row. if attribute is not set
	// it will be marked as NULL. each row is buffered in this->resultSet
	println("<ResultSet>");
	for (int i = 0; i < argc; i++) {
		println("  " << colNames[i] << " -> " << (argv[i] ? argv[i] : "NULL"));
		results->insert( std::pair<const char*,const char*>(colNames[i], argv[i]) );
	}
	println("</ResultSet>");

	return 0;
}


gre90r::SqlResult gre90r::Sqlite::select(const char* query) {

	if (this->isConnected()) {
		char* errmsg = 0;
		/**
		 *  sqlite3_exec(
		 * 		sqlite3*,
		 * 		const char *sql,
		 * 		sqlite_callback,
		 * 		void *data,					// !!! only pass SqlResult objects !!!
		 * 		char **errmsg
		 * 	)
		 */
		// &this->m_resultSet : give a resultset object where the results will be written to.
		// callbackSaveQueryResults writes to this->m_resultSet.
		int rc = sqlite3_exec(this->m_db, query, callbackSaveQueryResults, &this->m_resultSet, &errmsg);

		if (errmsg) {
			printlnError("[ERROR] query execution returned: " << errmsg << ". rc = " << rc);
		}
	}
	else {
		printlnError("[ERROR] cannot execute query. not connected to DB.");
		return gre90r::SqlResult(); // empty resultset
	}

	return this->m_resultSet;
}
