#include "test.hpp"
#include <geos/geos.h>
#include <geos/index/ItemVisitor.h>
#include <geos/index/quadtree/Quadtree.h>
#include <set>

const int inserseed = 9999;
const int queryseed = 8888;

using namespace geos;
using namespace geos::index;

class NodeVisitor: public ItemVisitor {
	std::vector<void*>& bf;
	Rect                rt;
	public:
	NodeVisitor(std::vector<void*>& v,const Rect& r):bf(v),rt(r)
	{
	}
	void visitItem(void* item)
	{
		Node* n = (Node*)item;
		if(Intersect(n->box,rt)){
			bf.push_back(item);
		}
	}

};

class NodeVisitor2: public ItemVisitor {
	std::vector<void*>& bf;
	Rect                rt;
	time_t    _t0,_t1;
	int _sat;
	public:
	NodeVisitor2(std::vector<void*>& v,const Rect& r,time_t t0,time_t t1,int sat)
		:bf(v),rt(r),_t0(t0),_t1(t1),_sat(sat)
	{
	}
	void visitItem(void* item)
	{
		Node* n = (Node*)item;
		if(Intersect(n->box,rt) && (n->dtime >= _t0 && n->dtime <= _t1) && n->sat == _sat ){
			bf.push_back(item);
		}
	}

};



class TestQuadtree :public TestBase {
	index::quadtree::Quadtree* mIndex;

public:
	~TestQuadtree()
	{
		delete mIndex;
	}
    void init(size_t count)
    {
		mIndex = new quadtree::Quadtree;
    }
    void insert(size_t count)
    {
        resetRand(inserseed);

        Timer t;
        double zsj = 0;
        for (size_t i = 0; i < count; ++i) {
            // 获取节点数据
            Node* n = new Node;
            getNextData(n->box, n->dtime, n->sat);
            n->url = std::to_string(i);
            n->id = i;

            // 插入计时
            t.Start();
			mIndex->insert(new Envelope(n->box.x0,n->box.x1,n->box.y0,n->box.y1),n);
            t.Stop();
            zsj += t.elapsed();
        }
        printf("    插入 %llu 总耗时:%lf 毫秒\n", count, zsj);
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
            std::vector<void*> result;
			Envelope querybox(r.x0,r.x1,r.y0,r.y1);
			NodeVisitor visit(result,r);
			mIndex->query(&querybox,visit);
            t.Stop();
            zsj += t.elapsed();

            // 输出中间那次的查询条件和结果
            if (i == count / 2) {
                printf("    查询条件:box[(%lf,%lf)-(%lf,%lf)],%lld<=dtime<=%lld,sat=%d\n    查询结果(仅输出ID)：",
                    r.x0, r.y0, r.x1, r.y1, t0, t1, sat);
                // 排序输出
                std::set<uint64_t> idset;
                for (auto& p : result) {
					Node* n = (Node*)p;
					idset.insert(n->id);
                }
				for(auto& id:idset){
					printf("%llu,", id);
				}
                puts("\n");
            }
        }
        printf("    查询(仅范围) %llu 总耗时:%lf 毫秒\n", count, zsj);

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
            std::vector<void*> result;
			Envelope querybox(r.x0,r.x1,r.y0,r.y1);
			NodeVisitor2 visit(result,r,t0,t1,sat);
			mIndex->query(&querybox,visit);
            t.Stop();
            zsj += t.elapsed();

            // 输出中间那次的查询条件和结果
            if (i == count / 2) {
                printf("    查询条件:box[(%lf,%lf)-(%lf,%lf)],%lld<=dtime<=%lld,sat=%d\n    查询结果(仅输出ID)：",
                    r.x0, r.y0, r.x1, r.y1, t0, t1, sat);
                // 排序输出
                std::set<uint64_t> idset;
                for (auto& p : result) {
					Node* n = (Node*)p;
					idset.insert(n->id);
                }
				for(auto& id:idset){
					printf("%llu,", id);
				}
                puts("\n");
            }
        }
        printf("    查询(全条件) %llu 总耗时:%lf 毫秒\n", count, zsj);
    }
};



int main()
{
    TestRun<TestQuadtree>(10000);
    TestRun<TestQuadtree>(50000);
    TestRun<TestQuadtree>(100000);
    TestRun<TestQuadtree>(200000);
    // TestRun<TestQuadtree>(1000000);
    return 0;
}
