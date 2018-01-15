#ifndef _solution_hpp
#define _solution_hpp

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

/**
   Classe pour représenter une solution
   
   Chaine d'entiers positifs auquel est ajoutée la valeur de la qualité (fitness)
*/
class Solution : public std::vector<unsigned int> {
public:
  Solution() : std::vector<unsigned int>() {
  }

  Solution(const Solution & _s) : std::vector<unsigned int>(_s) {
    _fitness = _s.fitness();
  }

  Solution& operator=(const Solution & _s) {
    this->resize(_s.size());
    for(unsigned int i = 0; i < _s.size(); i++)
      this->operator[](i) = _s[i];

    _fitness = _s.fitness();

    return *this;
  }

  /**
   * set the fitness
   */
  void fitness(unsigned int _fit) {
    _fitness = _fit;
  }

  /**
   * get the fitness
   */
  int fitness() const { 
    return _fitness; 
  }

  /**
   * print the solution
   */
  void print() {
    std::cout << this->fitness() << " " << this->size() ;

    for(unsigned int xi : *this)
      std::cout << " " << xi ;
  }

  /**
   * convert the solution into string
   */
  std::string to_string() const {
    std::stringstream sstr;

    sstr << this->fitness() << " " << this->size();

    for(unsigned int xi : *this)
      sstr << " " << xi ;

    return(sstr.str());
  }

private:
  // quality of the solution
  int _fitness;
};

/*
std::ostream& operator<<(std::ostream& o, const Solution& s)
{
    o << s.to_string();
    return o;
}
*/



#endif
