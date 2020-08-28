#include "theory.hpp"

namespace smtmbt {

std::ostream&
operator<<(std::ostream& out, TheoryId theory)
{
  switch (theory)
  {
    case THEORY_ARRAY: out << "THEORY_ARRAY"; break;
    case THEORY_BOOL: out << "THEORY_BOOL"; break;
    case THEORY_BV: out << "THEORY_BV"; break;
    case THEORY_FP: out << "THEORY_FP"; break;
    case THEORY_INT: out << "THEORY_INT"; break;
    case THEORY_QUANT: out << "THEORY_QUANT"; break;
    case THEORY_REAL: out << "THEORY_REAL"; break;
    case THEORY_STRING: out << "THEORY_STRING"; break;
    default: out << "UNKNOWN THEORY!" << int(theory); break;
  }
  return out;
}
}  // namespace smtmbt
