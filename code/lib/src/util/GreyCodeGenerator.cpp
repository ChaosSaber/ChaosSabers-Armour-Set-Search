#include "util/GreyCodeGenerator.hpp"
#include <cassert>
#include <iostream>
#include <string>

util::GreyCodeGenerator::GreyCodeGenerator(std::vector<size_t>&& maxValues, size_t start)
    : maxValues_(std::move(maxValues))
{
    init(start);
}

util::GreyCodeGenerator::GreyCodeGenerator(const std::vector<size_t>& maxValues, size_t start)
    : maxValues_(maxValues)
{
    init(start);
}

bool util::GreyCodeGenerator::generateNext(std::vector<size_t>& next)
{
    auto result = generateNext();
    if (result)
        fillVector(next);
    return result;
}

bool util::GreyCodeGenerator::generateNext()
{
    size_t coloumn = 0;
    while (coloumn < maxValues_.size())
    {
        if (coloumnHasMaxValue(coloumn))
        {
            ++coloumn;
            continue;
        }
        if (countOrders_[coloumn] == ADDITIVE)
        {
            ++indexes_[coloumn];
        }
        else
        {
            --indexes_[coloumn];
        }
        if (coloumn > 0)
        {
            for (size_t i = 0; i < coloumn; ++i)
                reverseCountOrder(i);
        }
        return true;
    }
    return false;
}

const std::vector<size_t>& util::GreyCodeGenerator::currentGreyCode() const { return indexes_; }

void util::GreyCodeGenerator::init(size_t start)
{
    countOrders_.resize(maxValues_.size());
    for (auto& order : countOrders_)
        order = ADDITIVE;
    indexes_.resize(maxValues_.size());
    for (auto& index : indexes_)
        index = 0;
    while (start > 0)
    {
        size_t coloumn = 0;
        size_t val = 1;
        while (coloumn < maxValues_.size())
        {
            auto newVal = val * (maxValues_[coloumn] + 1);
            if (newVal - 1 >= start)
            {
                for (size_t i = 0; i <= maxValues_[coloumn]; ++i)
                {
                    auto tmp = val * (i + 1);
                    if (tmp == start && i != maxValues_[coloumn])
                    {
                        indexes_[coloumn] = i + 1;
                        start = 0;
                        break;
                    }
                    else if (tmp > start)
                    {
                        indexes_[coloumn] = i;
                        start -= val * i;
                        break;
                    }
                }
                break;
            }
            val = newVal;
            ++coloumn;
        }
        if (coloumn >= maxValues_.size())
            throw std::logic_error("grey code can not generate that much elements");
    }
    for (size_t coloumn = maxValues_.size() - 1; coloumn > 0; --coloumn)
    {
        if (indexes_[coloumn] % 2 == 1)
        {
            // reverse previous coloumns
            for (size_t i = 0; i < coloumn; ++i)
            {
                reverseCountOrder(i);
                indexes_[i] = maxValues_[i] - indexes_[i];
            }
        }
    }
}

bool util::GreyCodeGenerator::coloumnHasMaxValue(size_t coloumn)
{
    if (countOrders_[coloumn] == ADDITIVE)
    {
        return indexes_[coloumn] == maxValues_[coloumn];
    }
    else
    {
        return indexes_[coloumn] == 0;
    }
}

void util::GreyCodeGenerator::reverseCountOrder(size_t coloumn)
{
    if (countOrders_[coloumn] == ADDITIVE)
        countOrders_[coloumn] = SUBTRACTIVE;
    else
        countOrders_[coloumn] = ADDITIVE;
}

void util::GreyCodeGenerator::fillVector(std::vector<size_t>& next) const
{
    if (next.size() != maxValues_.size())
        next.resize(maxValues_.size());
    for (size_t i = 0; i < maxValues_.size(); ++i)
        next[i] = indexes_[i];
}

size_t countChanges(const std::vector<size_t>& lhs, const std::vector<size_t>& rhs)
{
    assert(lhs.size() == rhs.size());
    size_t changes = 0;
    for (size_t i = 0; i < lhs.size(); ++i)
    {
        if (lhs[i] != rhs[i])
            ++changes;
    }
    return changes;
}

void util::GreyCodeTest()
{
    auto printVector = [](const std::vector<size_t>& vector) {
        for (auto& elem : vector)
            std::cout << elem << " ";
    };
    auto countChanges = [](const std::vector<size_t>& lhs, const std::vector<size_t>& rhs) {
        assert(lhs.size() == rhs.size());
        size_t changes = 0;
        for (size_t i = 0; i < lhs.size(); ++i)
        {
            if (lhs[i] != rhs[i])
                ++changes;
        }
        return changes;
    };
    auto compareVector = [printVector, countChanges](const std::vector<size_t>& lhs,
                                                     const std::vector<size_t>& rhs,
                                                     const std::string& currentNext) {
        if (countChanges(lhs, rhs) != 0)
        {
            std::cerr << currentNext << " grey code (";
            printVector(lhs);
            std::cerr << ") differs from initiated one (";
            printVector(rhs);
            std::cerr << ")" << std::endl;
            return false;
        }
        return true;
    };

    auto test = [countChanges, compareVector, printVector](const std::vector<size_t>& maxValues) {
        GreyCodeGenerator gen(maxValues);
        size_t count = 0;
        std::vector<size_t> current, next;
        current.resize(maxValues.size());
        for (auto& coloumn : current)
            coloumn = 0;
        while (gen.generateNext(next))
        {
            ++count;
            size_t changes = countChanges(current, next);
            if (changes != 1)
            {
                std::cerr << "FAILED: found different amount then 1 change: " << changes
                          << std::endl;
                printVector(current);
                std::cout << "\t\t\t";
                printVector(next);
                std::cout << std::endl;
                return;
            }

            // test initiated generator
            GreyCodeGenerator genInit(maxValues, count - 1);

            if (!compareVector(current, genInit.currentGreyCode(), "current"))
                return;

            std::vector<size_t> nextInit;
            if (!genInit.generateNext(nextInit))
            {
                std::cerr << "could not generate next grey code with initiated generator"
                          << std::endl;
                return;
            }
            if (!compareVector(next, nextInit, "next"))
                return;
            current = next;
        }
        size_t expectedCount = 1;
        for (auto& max : maxValues)
            expectedCount *= (max + 1); // Values from 0 to max
        if (expectedCount != (count + 1))
        {
            std::cerr << "FAILED: found " << count << " grey codes, but expected " << expectedCount
                      << std::endl;
            return;
        }
        std::cout << "PASSED" << std::endl;
    };

    test({3, 3, 3});
    test({2, 3, 4});
    test({1, 2, 3, 4});
    test({5, 2, 6, 4, 7});
    test({15, 10, 11, 13, 9});
}
