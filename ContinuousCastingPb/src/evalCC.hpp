#ifndef _evalCC_hpp
#define _evalCC_hpp

#include <graph_manager.hpp>
#include <models.hpp>

#include <artis-star/common/RootCoordinator.hpp>

#include <solution.hpp>

using namespace cc;
using namespace artis::common;

namespace cc {

class MyView : public cc::View
{
public:
    MyView()
    {
        selector("Stack:height", { RootGraphManager::CC,
                    SubGraphManager::STACK, cc::View::ALL, Stack::HEIGHT });
        selector("Crane:moveNumber", { RootGraphManager::CC,
                    SubGraphManager::CRANE, Crane::MOVE_NUMBER });
        selector("Crane:slabNumber", { RootGraphManager::CC,
                    SubGraphManager::CRANE, Crane::SLAB_NUMBER });
    }
};

class EvalCC
{
public:

    void operator()(Solution & /* solution */) ;

};

} // namespace cc

#endif