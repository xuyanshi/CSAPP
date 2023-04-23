#if !defined(UTIL_H)
#define UTIL_H

#include <iostream>

#define COUT_THIS(this) std::cout << this << std::endl;
#define COUT_VAR(this) std::cout << #this << ": " << this << std::endl;

#define CERR_THIS(this) std::cerr << this << std::endl;
#define CERR_VAR(this) std::cerr << #this << ": " << this << std::endl;

#define INVARIANT(cond)                                              \
  if (!(cond)) {                                                     \
    COUT_THIS(#cond << "failed at " << __FILE__ << ":" << __LINE__); \
    exit(-1);                                                        \
  }

#endif  // UTIL_H