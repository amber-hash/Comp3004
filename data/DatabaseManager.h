#pragma once
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

/**
 * DatabaseManager
 *
 * Owns the SQLite connection and schema.
 * Responsible for:
 *   - Opening/closing the database
 *   - Creating all tables (CREATE TABLE IF NOT EXISTS)
 *   - Seeding initial data if the DB is empty
 *   - Providing a single access point for QSqlQuery execution
 *
 * DataStore calls DatabaseManager for all persistence.
 * No UI or business logic lives here.
 */
class DatabaseManager {
public:
    static DatabaseManager& instance();

    // Opens the SQLite file, creates schema, seeds data if empty.
    // Call once from DataStore::initialize().
    bool open(const QString& path = "hintonmarket.db");

    void close();

    bool isOpen() const;

    // Returns the underlying database (for QSqlQuery construction in DataStore)
    QSqlDatabase& db() { return m_db; }

    // Schema helpers — called during open()
    bool createTables();
    bool seedIfEmpty();

private:
    DatabaseManager() = default;
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool seedUsers();
    bool seedMarketDates();

    bool execQuery(const QString& sql);

    QSqlDatabase m_db;
};
