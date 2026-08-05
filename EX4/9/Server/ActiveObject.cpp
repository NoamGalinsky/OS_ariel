#include "ActiveObject.h"
#include "GraphAlgorithm.h"

ActiveObject::ActiveObject(GraphAlgorithm* algo)
{
    this->algo = algo;
    next = nullptr;
}

void ActiveObject::activate(ActiveObjectArgs& args)
{
    args.ret += algo->activate(*args.graph) + "\n";
    if (next != nullptr)
    {
        next->activate(args);
    }
}

void ActiveObject::setNext(ActiveObject* next)
{
    this->next = next;
}

ActiveObject* ActiveObject::getNext() const
{
    return next;
}

