#pragma once
#include <vector>

namespace util
{
class GreyCodeGenerator
{
  public:
    /**
     * creates a new grey code generator with the given max values(inclusive) The grey code will
     * consist of n numbers. Where n ist the size of the given vector with the max values. each
     * number can go from 0 to the given max value.
     * @param maxValues A vector with the max values for each number
     */
    GreyCodeGenerator(std::vector<size_t>&& maxValues, size_t start = 0);
    GreyCodeGenerator(const std::vector<size_t>& maxValues, size_t start = 0);
    GreyCodeGenerator(const GreyCodeGenerator& other) = delete;
    GreyCodeGenerator(GreyCodeGenerator&& other) = delete;
    const GreyCodeGenerator& operator=(const GreyCodeGenerator& other) = delete;
    const GreyCodeGenerator& operator=(GreyCodeGenerator&& other) = delete;

    /**
     * generates the next grey code for the data
     * @param next A reference to an vector which will hold the next grey code after an succesfull
     * call
     * @return Returns true if a next GreyCode could be found and false if no more Grey Codes are
     * available
     */
    bool generateNext(std::vector<size_t>& next);

    /**
     * generates the next grey code for the data
     * @return Returns true if a next GreyCode could be found and false if no more Grey Codes are
     * available
     */
    bool generateNext();

    /**
     * @returns Returns the current grey code as vector
     */
    const std::vector<size_t>& currentGreyCode() const;

  private:
    /**
     * initialise the generator to the given value
     * @parameter start The index the generator shall be initialized to
     */
    void init(size_t start);
    /**
     * checks if the given coloumn is at its maximum value
     * @param coloumn The index of the coloumn which shall be checked
     * @return Returns true if the coloumn is at the max value, otherwise false
     */
    bool coloumnHasMaxValue(size_t coloumn);
    /**
     * reverse the count order of the specified coloumn
     * @param coloumn The index of the coloumn which shall be reversed
     */
    void reverseCountOrder(size_t coloumn);
    /**
     * fills the given Vector with the current index
     * @param next A reference to the vector which shall be filled
     */
    void fillVector(std::vector<size_t>& next) const;
    enum CountOrder { ADDITIVE, SUBTRACTIVE };
    std::vector<size_t> maxValues_;
    std::vector<CountOrder> countOrders_;
    std::vector<size_t> indexes_;
};

void GreyCodeTest();

} // namespace util