#include "test.hpp"
#include "sqlite3.h"
#include <algorithm>

const int inserseed = 9999;
const int queryseed = 8888;

class TestSqlite :public TestBase {
	sqlite3* db;
public:
	~TestSqlite()
	{
		sqlite3_close(db);
	}
    void init(size_t count)
    {
		sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
		int rc = sqlite3_open(":memory:",&db);
		// int rc = sqlite3_open("./test.sqlite",&db);
		if(rc != SQLITE_OK){
			puts("open sqlite error");
			exit(0);
		}
		char* errmsg;
		rc = sqlite3_exec(db,
                      "CREATE VIRTUAL TABLE test USING rtree(id,minX,maxX,minY,maxY, +dtime INTEGER NOT NULL,+sat INTEGER NOT NULL)",
                      NULL, NULL, &errmsg);
		if (rc != SQLITE_OK) 
		{
			printf("%4d Error:%s\n", __LINE__, errmsg);
			exit(0);
		}
    }
    void insert(size_t count)
    {
        resetRand(inserseed);

		char* errmsg;
		if (sqlite3_exec(db, "begin", NULL, NULL, &errmsg) != SQLITE_OK) {
			printf("%4d Error:%sn", __LINE__, errmsg);
			exit(0);
		}
	
		sqlite3_stmt *pStmt = NULL;
		if(sqlite3_prepare_v2(db,
							  "INSERT INTO test VALUES(?,?,?,?,?,?,?)",
							  -1, &pStmt, NULL) != SQLITE_OK) {
			printf("%4d Error:%s\n", __LINE__, errmsg);
			exit(0);
		}
        Timer t;
        double zsj = 0;
        for (size_t i = 0; i < count; ++i) {
            // 获取节点数据
            Node n;
            getNextData(n.box, n.dtime, n.sat);
            n.url = std::to_string(i);
            n.id = i;

            // 插入计时
            t.Start();
			sqlite3_bind_int64(pStmt, 1, n.id);
			sqlite3_bind_double(pStmt, 2, n.box.x0);
			sqlite3_bind_double(pStmt, 3, n.box.x1);
			sqlite3_bind_double(pStmt, 4, n.box.y0);
			sqlite3_bind_double(pStmt, 5, n.box.y1);
			sqlite3_bind_int64(pStmt, 6, n.dtime);
			sqlite3_bind_int(pStmt, 7, n.sat);
			
			sqlite3_step(pStmt);
            t.Stop();
            zsj += t.elapsed();
			sqlite3_reset(pStmt);
        }
		sqlite3_finalize(pStmt); 
		if (sqlite3_exec(db, "commit", NULL, NULL, &errmsg) != SQLITE_OK){
			printf("%4d Error:%s\n", __LINE__, errmsg);
			exit(0);
		}
	
        printf("插入 %llu 总耗时:%lf 毫秒\n", count, zsj);
    }
    void query(size_t count)
    {
        resetRand(queryseed);
		
		sqlite3_stmt* pStmt = NULL;
		if (sqlite3_prepare_v2(db,
				"SELECT id,dtime,sat FROM test WHERE NOT(maxX<? OR minX>? OR maxY<? OR minY>?);",
				-1, &pStmt, NULL) != SQLITE_OK) {
			printf("%4d Error\n", __LINE__);
			exit(0);
		}

        Timer t;
        double zsj = 0;
        for (size_t i = 0; i < count; ++i) {
            // 获取查询条件
            Rect r;
            time_t t0, t1;
            int sat;
            getQueryArg(r, t0, t1, sat);

            // 查询计时
            t.Start();
            std::vector<size_t> result;
			sqlite3_bind_double(pStmt, 1, r.x0);
			sqlite3_bind_double(pStmt, 2, r.x1);
			sqlite3_bind_double(pStmt, 3, r.y0);
			sqlite3_bind_double(pStmt, 4, r.y1);
			while (sqlite3_step(pStmt) == SQLITE_ROW) {
				int64_t id    = sqlite3_column_int64(pStmt, 0);
				time_t  dtime = sqlite3_column_int64(pStmt, 1);
				int     sat   = sqlite3_column_int(pStmt, 2);
				result.push_back(id);
			}
            t.Stop();
            zsj += t.elapsed();
			sqlite3_reset(pStmt);

            // 输出中间那次的查询条件和结果
            if (i == count / 2) {
                printf("    查询条件:box[(%lf,%lf)-(%lf,%lf)],%lld<=dtime<=%lld,sat=%d\n    查询结果(仅输出ID)：",
                    r.x0, r.y0, r.x1, r.y1, t0, t1, sat);
				std::sort(result.begin(),result.end());
                for (auto& id : result) {
                    printf("%llu,", id);
                }
                puts("\n");
            }
        }
		sqlite3_finalize(pStmt);
        printf("    查询(仅范围) %llu 总耗时:%lf 毫秒\n", count, zsj);


		pStmt = NULL;
		if (sqlite3_prepare_v2(db,
				"SELECT id,dtime,sat FROM test WHERE NOT(maxX<? OR minX>? OR maxY<? OR minY>?) AND dtime>=? AND dtime<=? AND sat==?;",
				-1, &pStmt, NULL) != SQLITE_OK) {
			printf("%4d Error\n", __LINE__);
			exit(0);
		}
        resetRand(queryseed);
        zsj = 0;
        for (size_t i = 0; i < count; ++i) {
            // 获取查询条件
            Rect r;
            time_t t0, t1;
            int sat;
            getQueryArg(r, t0, t1, sat);

            // 查询计时
            t.Start();
            std::vector<size_t> result;
            sqlite3_bind_double(pStmt, 1, r.x0);
			sqlite3_bind_double(pStmt, 2, r.x1);
			sqlite3_bind_double(pStmt, 3, r.y0);
			sqlite3_bind_double(pStmt, 4, r.y1);
			sqlite3_bind_int64(pStmt, 5, t0);
			sqlite3_bind_int64(pStmt, 6, t1);
			sqlite3_bind_int(pStmt, 7, sat);
			while (sqlite3_step(pStmt) == SQLITE_ROW) {
				int64_t id    = sqlite3_column_int64(pStmt, 0);
				time_t  dtime = sqlite3_column_int64(pStmt, 1);
				int     sat   = sqlite3_column_int(pStmt, 2);
				result.push_back(id);
			}
            t.Stop();
            zsj += t.elapsed();
			sqlite3_reset(pStmt);

            // 输出中间那次的查询条件和结果
            if (i == count / 2) {
                printf("    查询条件:box[(%lf,%lf)-(%lf,%lf)],%lld<=dtime<=%lld,sat=%d\n    查询结果(仅输出ID)：",
                    r.x0, r.y0, r.x1, r.y1, t0, t1, sat);
					std::sort(result.begin(),result.end());
                for (auto& id : result) {
                    printf("%llu,", id);
                }
                puts("\n");
            }
        }
		sqlite3_finalize(pStmt);
        printf("    查询(全条件) %llu 总耗时:%lf 毫秒\n", count, zsj);
    }
};



int main()
{
    TestRun<TestSqlite>(10000);
    TestRun<TestSqlite>(50000);
  //  TestRun<TestSqlite>(100000);
 //   TestRun<TestSqlite>(200000);
    // TestRun<TestSqlite>(1000000);
    return 0;
}
