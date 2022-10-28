#include "jdbc/mysql_driver.h"
#include "jdbc/mysql_connection.h"
#include "jdbc/mysql_error.h"
#include "jdbc/cppconn/statement.h"
#include "jdbc/cppconn/prepared_statement.h"
#include "jdbc/cppconn/resultset.h"

#include <iostream>

// Initialize cpp-mysql connector
// Try to connect to our database (MYSQL80)
// - ip:		127.0.0.1
// - port:		3306
// - username:	root
// - password:	****
// - schema:	gameworld

class GameDB {
public:
	GameDB();
	~GameDB();

	bool Connect();
	void Disconnect();

	void AddMonster(const char* name, const char* type, int hp, int speed);
	void PrintMonsterData();

private:
	sql::Driver* m_pDriver;
	sql::Connection* m_pConnection;
	sql::ResultSet* m_pResultSet;
	sql::Statement* m_pStatement;
	sql::PreparedStatement* m_pInsertMonsterStatement;
};

GameDB::GameDB() 
	: m_pDriver(nullptr)
	, m_pConnection(nullptr)
	, m_pResultSet(nullptr) 
	, m_pStatement(nullptr) {}
GameDB::~GameDB() {}


bool GameDB::Connect() {
	// Connection call here
	try {
		m_pDriver = sql::mysql::get_driver_instance();
	}
	catch (sql::SQLException e) {
		printf("Failed to get_driver_instance: %s\n", e.what());
		return false;
	}
	printf("Successfully retrieved our ccp-conn-sql driver!\n");

	try {
		sql::SQLString hostName("127.0.0.1:3306");
		sql::SQLString username("root");
		sql::SQLString password("root");
		m_pConnection = m_pDriver->connect(hostName, username, password);
		m_pConnection->setSchema("gameworld");
	}
	catch (sql::SQLException e) {
		printf("Failed to connect to our database: %s\n", e.what());
		return false;
	}
	printf("Successfully connected to our Database & Schema!\n");

	try {
		m_pStatement = m_pConnection->createStatement();
		m_pInsertMonsterStatement = m_pConnection->prepareStatement(
			"INSERT INTO monster_data (name, type, max_hp, walk_speed) VALUES (?, ?, ?, ?);");
	}
	catch (sql::SQLException e) {
		printf("Failed to create statements: %s\n", e.what());
		return false;
	}

	return true;
}

void GameDB::AddMonster(const char* name, const char* type, int hp, int speed)
{
	// Input sanitation is very important.
	// We don't want to put ANYTHING from the user into a command
	// that is not sanitized
	m_pInsertMonsterStatement->setString(1, name);
	m_pInsertMonsterStatement->setString(2, type);
	m_pInsertMonsterStatement->setInt(3, hp);
	m_pInsertMonsterStatement->setInt(4, speed);
	
	try {
		m_pInsertMonsterStatement->execute();
	}
	catch (sql::SQLException e) {
		printf("Failed to add a monster to our database: %s\n", e.what());
		return;
	}
	printf("Successfully added a monster to our Database & Schema!\n");
}

void GameDB::PrintMonsterData() {

	try {
		m_pResultSet = m_pStatement->executeQuery("SELECT * FROM monster_data;");
	}
	catch (sql::SQLException e) {
		printf("Failed to query our database: %s\n", e.what());
		return;
	}
	printf("Successfully retrieved %d rows from the database!\n", (int)m_pResultSet->rowsCount());

	while (m_pResultSet->next()) {
		// We are at a valid row of data
		// We know the data we should have, because we should know the query we made
		// id, name, type, max_hp, max_speed
		int id = m_pResultSet->getInt("id"); // m_pResulSet->getInt(1);
		sql::SQLString name = m_pResultSet->getString("name");
		sql::SQLString type = m_pResultSet->getString("type");
		int max_hp = m_pResultSet->getInt("max_hp");
		int walk_speed = m_pResultSet->getInt("walk_speed");

		printf("%d | %s (%s) %d, %d\n", id, name.c_str(), type.c_str(), max_hp, walk_speed);
	}
}

void GameDB::Disconnect() {
	// Disconnect
	try {
		m_pConnection->close();
	}
	catch (sql::SQLException e) {
		printf("Failed to close the connection to our database: %s\n", e.what());
		return;
	}
	printf("Successfully closed the connection to our Database!\n");

	delete m_pStatement;
	delete m_pResultSet;
	delete m_pInsertMonsterStatement;
}


int main(int argc, char** argv) {
	GameDB db;

	if (db.Connect() == false)
		return 1;

	db.PrintMonsterData();
	db.AddMonster("Goblin Wizard", "Goblin", 100, 3);
	db.PrintMonsterData();
	db.Disconnect();

	return 0;
}