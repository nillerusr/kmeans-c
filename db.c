//
// Created by nillerusr on 06.02.20.
//

#include "db.h"
#include "sqlite3.h"
#include "images.h"
#include "cjni.h"
#include "stdbool.h"

sqlite3 *db;
static int rcmain = -1;
struct point_s dbPoint;
static bool bCalled = false;

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    bCalled = true;
    LogPrintf("callback");
    dbPoint.color[0] = atoi(argv[1]);
    dbPoint.color[1] = atoi(argv[2]);
    dbPoint.color[2] = atoi(argv[3]);
    return 0;
}

bool openColorsDatabase()
{
    char dbPath[512];
    const char *sqlcmd = "CREATE TABLE IF NOT EXISTS words (word TEXT, R BYTE, G BYTE, B BYTE)";

    if( rcmain == SQLITE_OK )
        return true;

    snprintf(dbPath, sizeof(dbPath), "%s/colors.sqlite", datapath);
    rcmain = sqlite3_open( dbPath ,&db);
    LogPrintf("database opened!");

    LogPrintf("%s\n", dbPath);

    if( rcmain != SQLITE_OK )
    {
        ELogPrintf("Cannot open database: %s\n", sqlite3_errmsg(db));
        return false;
    }

    sqlite3_exec(db, sqlcmd, NULL, 0, NULL);

    return true;
};

point_t getColorFromDatabase(const char *word)
{
    char sqlcmd[512];
    int rc;
    snprintf(sqlcmd, sizeof sqlcmd, "SELECT * FROM words WHERE word='%s'", word );
    bCalled = false;
    rc = sqlite3_exec(db, sqlcmd, callback, 0, 0 );
    if( rc != SQLITE_OK || !bCalled )
        dbPoint.color[0] = -2;

    return dbPoint;
}

void addColorToDatabase(const char *word, int r, int g, int b)
{
    char sqlcmd[512];
    snprintf(sqlcmd, sizeof sqlcmd, "INSERT INTO words VALUES ('%s',%d,%d,%d)", word, r, g, b);
    sqlite3_exec(db, sqlcmd, 0, 0, 0 );
}