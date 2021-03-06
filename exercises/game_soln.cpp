// Copyright David Abrahams 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define _GLIBCXX_DEBUG 1

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <set>
#include <iostream>

struct GameObject : boost::enable_shared_from_this<GameObject>
{
    GameObject(int points) : points(points), id(count++) {}
protected:
    virtual ~GameObject()
    {
        std::cout << "bye bye " << *this << std::endl;
    }
public:
    virtual void time_slice() = 0;
    void damage(int d);

    struct deleter
    {
        void operator()(GameObject* p) { delete p; }
    };

    friend std::ostream& operator<<(std::ostream& os, GameObject const& self)
    {
        return os << "object " << self.id;
    }
 private:
    int points;

    // Just a way of identifying things.
    int id;
    static int count;
};
int GameObject::count = 0;

std::set< boost::shared_ptr<GameObject> > all_targets;

void GameObject::damage(int d)
{
    std::cout << *this << " sustains " << d << " points of damage";
    if ( d > this->points )
    {
        std::cout << "...and is destroyed";
        all_targets.erase(shared_from_this());
    }
    else
    {
        this->points -= d;
    }
    std::cout << std::endl;
}

struct Tank : GameObject
{
    Tank() : GameObject(3000) {}

    ~Tank() {}
    virtual void time_slice()
    {
        boost::shared_ptr<GameObject> p = this->target.lock();

        if (p)
            p->damage(500);
        else
            this->target = this->select_target();
    }

    boost::shared_ptr<GameObject> select_target()
    {
        // Choose a random target.  Hint: if you shoot yourself that
        // makes the problem harder.  You might want to avoid that.
        std::set< boost::shared_ptr<GameObject> >::iterator targets
            = all_targets.begin(), t;
        do
        {
            t = targets;
            std::advance(t, rand() % all_targets.size() );
        }
        while(t->get() == this);
        return *t;
    }

 private:
    boost::weak_ptr<GameObject> target;
};


int main()
{
    for (int i = 0; i < 200; ++i)
    {
        boost::shared_ptr<GameObject> p(new Tank, Tank::deleter());
        all_targets.insert(p);
    }

    while (all_targets.size() > 1)
    {
        std::for_each(
            all_targets.begin(), all_targets.end()
          , boost::bind(&GameObject::time_slice,_1));
    }
    std::cout << **all_targets.begin() << " is the winner!" << std::endl;
}
