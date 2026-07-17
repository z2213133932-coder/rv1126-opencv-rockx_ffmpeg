#include "sqlite3_operation.h"

sqlite3* db = NULL;
sqlite3_stmt* stmt = NULL;
int sqlite_ret;
int ConnectToSqilte3()
{
    sqlite_ret = sqlite3_open("/userdata/face.db",&db);
    if(sqlite_ret != SQLITE_OK)
    {
        printf("Connect to sqlite error\n");
        return -1;
    }
    else
    {
        printf("Connect to sqlite sucess\n");
        return 0;
    }
}

int InsertInToSqlite3(rockx_face_feature_t face_feature,char* name)
{
    const char *sql = "insert into face_data_table(name, face_feature) values(?, ?);";
    sqlite3_prepare(db,sql,-1,&stmt,NULL);
    sqlite3_bind_text(stmt,1,name,strlen(name),NULL);
    sqlite3_bind_blob(stmt,2,face_feature.feature,face_feature.len,NULL);
    sqlite_ret = sqlite3_step(stmt);
    if(sqlite_ret == SQLITE_DONE)
    {
        printf("insert done\n");
        return 0;
    }
    else
    {
        return -1;
    }
}

map<string,rockx_face_feature_t> GetSquliteData()
{
    char* name = NULL;
    rockx_face_feature_t sqlite_feature = {0};
    const char* sql = "select name,face_feature from face_data_table;"; 
    sqlite_ret = sqlite3_prepare(db,sql,-1,&stmt,NULL);
    if(sqlite_ret == SQLITE_OK)
    {
        while(SQLITE_ROW == sqlite3_step(stmt))
        {
            name = (char*)sqlite3_column_text(stmt,0);
            printf("name = %s\n",name);
            const void* feature = sqlite3_column_blob(stmt,1);
            int blob_size = sqlite3_column_bytes(stmt, 1);
            memcpy(sqlite_feature.feature,feature,blob_size);
            sqlite_feature.len = blob_size;
            printf("get sucess\n");
        }
    }

    map<string,rockx_face_feature_t> people_feature;
    people_feature.insert(make_pair((string)name,sqlite_feature));
    return people_feature;
}
