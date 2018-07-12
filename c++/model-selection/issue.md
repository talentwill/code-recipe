有个User类，其方法calculate()的内部需要使用到外部数据类的实例中的某一个成员值作为输入源。在User构造函数需要传入不同数据实例作为参数，根据传入不同的数据实例分别创建不同的 User 类。

**先来看下A和B两个数据类，结构和层次不同，但是都有 Rate 这个值**

```
struct Rate{
    int value; // 被用于计算的值
};

namespace A{
struct Data{
    struct Info{
        int i1,i2;
        Rate rate = {1};
    };
    Data() { info = std::make_shared<Info>();}
    std::shared_ptr<Info> info;
    int aa, ab, ac; //不会被User 类用到的值
};}

namespace B{
struct Data{
    Rate rate = {2};
    int ba, bb, bc; //不会被User 类用到的值
};} 
```

**下面是创建User类，传入不同的 Data 类型构建出不同的 User 实例**

**User类 example 1:**

因为 User 每次只会接受一种Data类作为计算，所以可以在构造函数中传入 不同Data类的指针来判断，使用指针不为空的类实例去做计算。在calculate()中，需要编写不同的代码来访问 不同Data 类中的 同一个rate 值。
这个代码看上去会非常恶心，因为根据不同的类型需要不同的代码，如果有3~4种不同的 Data 类，calculate()就需要不同的 if 分支。
```
class User1
{
public:
    User1(std::shared_ptr<A::Data> a, std::shared_ptr<B::Data> b){
        a_ = a; b_ = b;
    }

    int calculate(){
        int result = 0;
        if (a_) {
            result = a_->info->rate.value * 10;
        }else if(b_){
            result = b_->rate.value * 10;
        }
        return result;
    }

    std::shared_ptr<A::Data> a_;
    std::shared_ptr<B::Data> b_;
};

int main()
{
    User1 *u1a = new User1(std::make_shared<A::Data>(), nullptr);
    u1a->calculate();
    User1 *u1b = new User1(nullptr, std::make_shared<B::Data>());
    u1b->calculate();
}
```

**User类 example 2:**

能不能用C++模板的方式传入不同的 Data 呢？
思路：不管哪一种Data类，都会有 rate 这边成员变量，只是调用的层次不一样，那可不可以把前面的类型作为模板参数呢？
尝试了一下，代码如下，但是感觉还是不够好。

```
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
      return result;
    }

    std::shared_ptr<T> data_;
};

int main()
{
    // DataA 就传入 A::Data::Info作为模板参数
    auto u2a = new User2<A::Data::Info>(std::make_shared<A::Data::Info>());
    u2a->calculate();
    // DataB 就传入 B::Data作为模板参数
    auto u2b = new User2<B::Data>(std::make_shared<B::Data>());
    u2b->calculate();
}
```

**User类 example 3:**
既然 User 类是需要一个 rate 值，那可不可以在构造函数的时候传入一个函数对象呢，在calculate()中调用这个函数对象，获取到不同 Data 实例的 rate 值。

```
//把user需要的输入值用一个函数对象传入，user就不需要关心这个值从哪里来的了
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
        return result;
    }

    getRateFunc getRate_;
};

int main()
{
    //用匿名函数传入User类需要的值
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
```
### 最后的问题
注意的是Data类因为是纯数据类，是不可以继承接口的。
有没有比较好的编程范式去解决这种场景下的问题呢？
另外3个例子中哪个比较可取呢？