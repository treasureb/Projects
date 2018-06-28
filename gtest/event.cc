#include<cstdio>
#include<map>
#include<gtest/gtest.h>



/*
 * 事件机制的三种级别
 * 1.全局的
 * 2.test_case
 * 3.test
 *
 */

class GlobalTest:public testing::Environment{
public:
    void SetUp(){
        printf("Environment:: Set\n");
    }

    void TearDown(){
        printf("Environment:: TearDown\n");
    }
};



//自定义类,然后继承gtest框架中的testing::Test基类
class TestMap:public testing::Test{
public:

    //重写两个虚函数,触发事件机制
    void SetUp(){
        printf("begin test\n");
        data.insert(std::make_pair(11,20));
        data.insert(std::make_pair(12,20));
        data.insert(std::make_pair(13,20));
        data.insert(std::make_pair(14,20));
    }

    void TearDown(){
        printf("end test\n");
        data.clear();
    }

    static void SetUpTestCase(){
        printf("SetUpTestCase\n");
    }
    
    static void TearDownTestCase(){
        printf("TearDownTestCase\n");
    }

    std::map<int,int> data;
};


// 1.测试map的size接口
TEST_F(TestMap,TestSize){
    //数据准备阶段 

    //插入一个pair类型

    ASSERT_EQ(data.size(),4);
}

// 2.测试map的empt接口
TEST_F(TestMap,TestEmpty){
    //数据准备阶段
    ASSERT_FALSE(data.empty());
}

// 3.测试map []接口
TEST_F(TestMap,TestBracket){

    //数据准备阶段

    //MakeTestData(data);
    ASSERT_EQ(data[11],20);
    data[11] = 100;
    ASSERT_EQ(data[11],100);
    data[100] = 1000;
    ASSERT_EQ(data[100],1000);
}


int main( int argc,char* argv[] ){
    testing::InitGoogleTest(&argc,argv);
    testing::Environment* env = new GlobalTest();
    testing::AddGlobalTestEnvironment(env);
    return 0;
}
