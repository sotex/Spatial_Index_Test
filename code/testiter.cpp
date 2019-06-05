#include "test.hpp"

const int inserseed = 9999;
const int queryseed = 8888;

class TestIter :public TestBase {
    std::vector<Node> data;
public:
    void init(size_t count)
    {
        data.reserve(count);
    }
    void insert(size_t count)
    {
        resetRand(inserseed);

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
            data.emplace_back(n);
            t.Stop();
            zsj += t.elapsed();
        }
        printf("插入 %llu 总耗时:%lf 毫秒\n", count, zsj);
    }
    void query(size_t count)
    {
        resetRand(queryseed);

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
            for (size_t ii = 0; ii < data.size(); ++ii) {
                Node& n = data[ii];
                if (Intersect(r, n.box)) {
                    result.push_back(n.id);
                }
            }
            t.Stop();
            zsj += t.elapsed();

            // 输出中间那次的查询条件和结果
            if (i == count / 2) {
                printf("查询条件:box[(%lf,%lf)-(%lf,%lf)],%lld<=dtime<=%lld,sat=%d\n查询结果(仅输出ID)：",
                    r.x0, r.y0, r.x1, r.y1, t0, t1, sat);
                for (auto& id : result) {
                    printf("%llu,", id);
                }
                puts("\n");
            }
        }
        printf("查询(仅范围) %llu 总耗时:%lf 毫秒\n", count, zsj);

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
            for (size_t ii = 0; ii < data.size(); ++ii) {
                Node& n = data[ii];
                if (Intersect(r, n.box) && 
                    (n.dtime <= t1 && n.dtime >= t0) &&
                    n.sat == sat) {
                    result.push_back(n.id);
                }
            }
            t.Stop();
            zsj += t.elapsed();

            // 输出中间那次的查询条件和结果
            if (i == count / 2) {
                printf("查询条件:box[(%lf,%lf)-(%lf,%lf)],%lld<=dtime<=%lld,sat=%d\n查询结果(仅输出ID)：",
                    r.x0, r.y0, r.x1, r.y1, t0, t1, sat);
                for (auto& id : result) {
                    printf("%llu,", id);
                }
                puts("\n");
            }
        }
        printf("查询(全条件) %llu 总耗时:%lf 毫秒\n", count, zsj);
    }
};



int main()
{
    TestRun<TestIter>(10000);
    TestRun<TestIter>(50000);
    TestRun<TestIter>(100000);
    TestRun<TestIter>(200000);
    // TestRun<TestIter>(1000000);
    return 0;
}
