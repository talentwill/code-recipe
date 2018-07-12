#include <stdio.h>
#include <memory>
#include <functional>

struct Rate{
    int value; // 被用于计算的值
};

namespace A{
struct Data{
    Data() { info = std::make_shared<Info>();}
    struct Info
    {
        int info;
        Rate rate = {1};
    };
    
    std::shared_ptr<Info> info;
    int aa, ab, ac; //不会被User 类用到的值
};}

namespace B{
struct Data{
    Rate rate = {2};
    int ba, bb, bc; //不会被User 类用到的值
};} 


class User1
{
public:
    User1(std::shared_ptr<A::Data> a, std::shared_ptr<B::Data> b){
        a_ = a;
        b_ = b;
    }

    int calculate(){
        int result = 0;
        if (a_) {
            result = a_->info->rate.value * 10;
        }else if(b_){
            result = b_->rate.value * 10;
        }
        printf("calc=%d\n", result);
        return result;
    }

    std::shared_ptr<A::Data> a_;
    std::shared_ptr<B::Data> b_;
};

template<class T=B::Data>
class User2
{
public:
    User2(std::shared_ptr<T> data){
        data_ = data;
    }

    int calculate()
    {
      int result = data_->rate.value * 10;
      printf("calc = %d\n", result);
      return result;
    }

    std::shared_ptr<T> data_;
};

using getRateFunc = std::function<int()>;
class User3
{
  public:
    User3(getRateFunc getRate)
    {
        getRate_ = getRate;
    }

    int calculate()
    {
        int result = getRate_() * 10;
        printf("calc = %d\n", result);
        return result;
    }

    getRateFunc getRate_;
};

void user1()
{
    User1 *u1a = new User1(std::make_shared<A::Data>(), nullptr);
    u1a->calculate();
    User1 *u1b = new User1(nullptr, std::make_shared<B::Data>());
    u1b->calculate();
}

void user2()
{
    // DataA 就传入 A::Data::Info作为模板参数
    auto u2a = new User2<A::Data::Info>(std::make_shared<A::Data::Info>());
    u2a->calculate();
    // DataB 就传入 B::Data作为模板参数
    auto u2b = new User2<B::Data>(std::make_shared<B::Data>());
    u2b->calculate();
}

void user3()
{
    auto a = std::make_shared<A::Data>();
    auto u3a = new User3([a](){
        return a->info->rate.value;
    });
    u3a->calculate();

    auto b = std::make_shared<B::Data>();
    auto u3b = new User3([b]() {
        return b->rate.value;
    });
    u3b->calculate();
}
int main()
{
    user1();
    user2();
    user3();
    return 0;
}
