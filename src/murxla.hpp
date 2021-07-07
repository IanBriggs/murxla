#ifndef __MURXLA__MURXLA_H
#define __MURXLA__MURXLA_H

#include <cstdint>
#include <string>

#include "options.hpp"
#include "solver_option.hpp"
#include "theory.hpp"

namespace murxla {

namespace statistics {
class Statistics;
};

class Murxla
{
 public:
  using ErrorMap =
      std::unordered_map<std::string,
                         std::pair<std::string, std::vector<uint32_t>>>;

  enum Result
  {
    RESULT_ERROR,
    RESULT_ERROR_CONFIG,
    RESULT_OK,
    RESULT_TIMEOUT,
    RESULT_UNKNOWN,
  };

  inline static const std::string DD_PREFIX    = "murxla-dd-";
  inline static const std::string SOLVER_BTOR  = "btor";
  inline static const std::string SOLVER_BZLA  = "bzla";
  inline static const std::string SOLVER_CVC5  = "cvc5";
  inline static const std::string SOLVER_SMT2  = "smt2";
  inline static const std::string SOLVER_YICES = "yices";

  inline static const std::string API_TRACE = "tmp-api.trace";
  inline static const std::string SMT2_FILE = "tmp-smt2.smt2";

  static constexpr int32_t SMT2_READ_END  = 0;
  static constexpr int32_t SMT2_WRITE_END = 1;

  Murxla(statistics::Statistics* stats,
         const Options& options,
         SolverOptions* solver_options,
         ErrorMap* error_map,
         const std::string& tmp_dir);

  Result run(const std::string& file_out,
             const std::string& file_err,
             const std::string& untrace_file_name,
             bool run_forked,
             bool trace_file);

  void test();

  void dd();

  Options d_options;
  SolverOptions* d_solver_options;
  std::string d_tmp_dir;

 private:
  Result run_aux(const std::string& file_out,
                 const std::string& file_err,
                 const std::string& untrace_file_name,
                 bool run_forked,
                 bool trace_file);

  Result replay(const std::string& out_file_name,
                const std::string& err_file_name,
                const std::string& untrace_file_name);

  bool add_error(const std::string& err, uint32_t seed);

  statistics::Statistics* d_stats;

  /** Map normalized error message to pair (original error message, seeds). */
  ErrorMap* d_errors;
};

std::ostream& operator<<(std::ostream& out, const Murxla::Result& res);

}  // namespace murxla
#endif
