
#include <evalCC.hpp>

namespace cc {

void EvalCC::operator()(Solution & solution) {
    GlobalParameters p;

    p.preferences = solution;

    artis::common::RootCoordinator <
        DoubleTime, artis::pdevs::Coordinator <
            DoubleTime,
            RootGraphManager,
            GlobalParameters >
        > rc(0, 4800, "root", p, artis::common::NoParameters());

    rc.attachView("CC", new ::MyView());

    rc.run();

    const ::MyView::Values& values = rc.observer().view("CC").get("Crane:moveNumber");
    int move_number;

    values.back().second(move_number);

    solution.fitness(move_number);
}

} // namespace cc
