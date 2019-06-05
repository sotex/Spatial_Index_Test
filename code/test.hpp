#ifndef __TEST_HPP__
#define __TEST_HPP__

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <random>

class Timer {
    clock_t _s;
    clock_t _e;
public:
    Timer()
        :_s(clock()), _e(0) {}
    inline clock_t Start() {
        return _s = clock();
    }
    inline clock_t Stop() {
        return _e = clock();
    }
    inline double elapsed() const {
        return double(_e - _s) * 1000.0 / CLOCKS_PER_SEC;
    }
    inline double elapsed_sec() const {
        return double(_e - _s) / CLOCKS_PER_SEC;
    }
};

struct Rect {
    double x0, x1;
    double y0, y1;
};
inline bool Intersect(const Rect& r1, const Rect& r2)
{
    return !((r1.x0 > r2.x1) ||
        (r1.x1 < r2.x0) ||
        (r1.y0 > r2.y1) ||
        (r1.y1 < r2.y0));
}
struct Node {
    uint64_t    id;        // 节点ID
    std::string url;       // 影像路径
    Rect        box;       // 外包框
    time_t      dtime;     // 数据时间
    int         sat;       // 卫星
};

class TestBase {
	std::mt19937 mt;
	inline double rand_real()
	{
		return double(mt()) / (mt.max() - mt.min());
	}
	inline uint32_t rand_uint()
	{
		return mt();
	}
protected:
    void resetRand(int seed)
    {
        mt.seed(seed);
    }
    void getNextData(Rect& r, time_t& dtime, int& sat)
    {
        // 生成大小在0.002~0.2度大小的矩形(绝大部分影像的大小)
        // 0.002度约为222米，0.2度 22.26千米
        // 高与宽的长度差距在上下70%以内
        Node n;
        r.x0 = -180 + rand_real() * 359.8;
        r.y0 = -90 + rand_real() * 179.8;
        double w = 0.002 + rand_real() * 0.1998;
        double h = w * (1.0 + double(0.01 * (70 - rand_uint() % 141)));
        r.x1 = r.x0 + w;
        r.y1 = r.y0 + h;
        // 时间控制在1990年1月1日到2019年1月1日
        dtime = 631123200 + rand_uint() % 915148800;
        // 卫星限制在0-5
        sat = rand_uint() % 6;
    }

    void getQueryArg(Rect& r,time_t& t0,time_t& t1,int& sat)
    {
        // 查询框在 0.0023~5.625度（约为度网格5~16级）
        double w = 0.0023 + rand_real() * 5.625;
        r.x0 = -180 + rand_real() * (360.0 - w);
        r.y0 = -90 + rand_real() * (180.0 - w);
        r.x1 = r.x0 + w;
        r.y1 = r.y0 + w;
        // 时间控制在1个月(2592000)~5年(155520000)之间
        time_t el = 2592000 + (rand_uint() % 152928000);
        t0 = 631123200 + (rand_uint() % (915148800-el));
        t1 = t1 + el;
        // 卫星
        sat = rand_uint() % 6;
    }
public:
    virtual void init(size_t count) = 0;
    virtual void insert(size_t count) = 0;
    virtual void query(size_t count) = 0;
};



template<typename T>
void TestRun(size_t count)
{
    TestBase* p = new T;
    Timer t;

    p->init(count);

    printf("\n\nSSSSSSSSSSSSSSSS----TestRun(insert(%llu))----SSSSSSSSSSSSSSSS\n", count);
    t.Start();
    p->insert(count);
    t.Stop();
    printf("\n-----TestRun(insert(%llu)) 耗时%lf-----\n", count, t.elapsed());

    printf("\n------------TestRun(query(%llu))-----------\n", count);
    t.Start();
    p->query(count);
    t.Stop();
    printf("\nEEEEEEEEEEEEEEEE----TestRun(query(%llu)) 耗时%lf----EEEEEEEE\n", count, t.elapsed());
}

#endif //!__TEST_HPP__

