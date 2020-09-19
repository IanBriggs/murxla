#ifndef __SMTMBT__SOLVER_MANAGER_H
#define __SMTMBT__SOLVER_MANAGER_H

#include <cassert>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "solver.hpp"
#include "solver_option.hpp"
#include "sort.hpp"
#include "term_db.hpp"
#include "theory.hpp"
#include "util.hpp"

namespace smtmbt {

namespace statistics {
struct Statistics;
}

/* -------------------------------------------------------------------------- */

class SolverManager
{
 public:
  using SortSet = std::unordered_set<Sort, HashSort>;

  /* Statistics. */
  struct Stats
  {
    uint32_t inputs = 0; /* values, constants */
    uint32_t vars   = 0; /* variables */
    uint32_t terms  = 0; /* all terms, including inputs */
    uint32_t sorts  = 0; /* all sorts */
  };

  SolverManager(Solver* solver,
                RNGenerator& rng,
                std::ostream& trace,
                SolverOptions& options,
                bool arith_linear,
                bool trace_seeds,
                bool cross_check,
                bool simple_symbols,
                statistics::Statistics* stats,
                TheoryIdVector& enabled_theories);
  ~SolverManager() = default;

  statistics::Statistics* d_mbt_stats;

  /** Clear all data. */
  void clear();

  /** Get solver. */
  Solver& get_solver();

  /** Set random number generator. */
  void set_rng(RNGenerator& rng);
  /** Get random number generator. */
  RNGenerator& get_rng() const;

  /** Get the list of terms for which tracing with get-sort is pending. */
  std::vector<Term>& get_pending_get_sorts();

  /** Get the trace line for the current seed ("set-seed <seed>"). */
  std::string trace_seed() const;

  /** True if current run is a cross check run. */
  bool is_cross_check() const;

  /** Get set of enabled theories. */
  const TheoryIdSet& get_enabled_theories() const;

  std::ostream& get_trace();

  /** Get the number of created terms. */
  uint64_t get_n_terms() const;
  /** Get the number of created terms of given sort kind. */
  uint64_t get_n_terms(SortKind sort_kind);

  /**
   * Add operator kind to operator kinds database.
   * kind           : the operator kind
   * arity          : the arity of the operator
   * nparams        : the number of parameters of the operator
   * sort_kind      : the sort kind of the operator
   * sort_kind_args : a vector of sorts of the operators' arguments, if all or
   *                  the remaining kinds are the same, it's sufficient to only
   *                  list it once
   */
  void add_op_kind(const OpKind& kind,
                   int32_t arity,
                   uint32_t nparams,
                   SortKind sort_kind,
                   const std::vector<SortKind>& sort_kind_args,
                   TheoryId theory);
  /** Add sort to sort databse. */
  void add_sort(Sort& sort, SortKind sort_kind);
  /** Add value to term database. */
  void add_value(Term& term, Sort& sort, SortKind sort_kind);
  /** Add string value of lenght 1. */
  void add_string_char_value(Term& term);
  /** Add input to term database. */
  void add_input(Term& term, Sort& sort, SortKind sort_kind);
  /** Add var to term database. */
  void add_var(Term& term, Sort& sort, SortKind sort_kind);
  /** Add non-input term to term database. */
  void add_term(Term& term,
                SortKind sort_kind,
                const std::vector<Term>& args = {});

  /** Pick arbitrary symbol (simple or piped). */
  std::string pick_symbol();

  /**
   * Pick sort kind of existing (= created) sort.
   * Optionally restrict selection to sort kinds with terms only if
   * 'with_terms' is true.
   */
  SortKind pick_sort_kind(bool with_terms = true);

  /**
   * Pick enabled sort kind (and get its data).
   * Only sort kinds of enabled theories are picked.
   * This function does not guarantee that a sort of this kind alreay exists.
   */
  SortKindData& pick_sort_kind_data();
  /**
   * Pick enabled operator kind (and get its data).
   * Only operator kinds of enabled theories are picked.
   */
  OpKind pick_op_kind(bool with_terms = true);

  Op& get_op(const OpKind& kind);

  /**
   * Return true if
   *  - with_terms = true : Any terms in any enabled theory have been created
   *                        such that an operator of that theory applies.
   *  - with_terms = false: Any theory is enabled.
   */
  bool has_theory(bool with_terms = true);
  /** Pick any of the enabled theories. */
  TheoryId pick_theory(bool with_terms = true);

  /**
   * Pick a value of given sort.
   * Requires that a value of given sort exists.
   */
  Term pick_value(Sort sort);

  /**
   * Pick string value with lenght 1.
   */
  Term pick_string_char_value();

  /**
   * Pick a term of given sort.
   * Requires that terms of this sort exist.
   */
  Term pick_term(Sort sort);
  /**
   * Pick term of given sort kind and scope level.
   * Requires that terms of this sort kind exist.
   */
  Term pick_term(SortKind sort_kind, size_t level);
  /**
   * Pick term of given sort kind.
   * Requires that terms of this sort kind exist.
   */
  Term pick_term(SortKind sort_kind);
  /**
   * Pick any term.
   * Requires that terms of any sort kind exist.
   */
  Term pick_term();

  /**
   * Pick variable from current scope level.
   */
  Term pick_var();

  /**
   * Remove variable from current scope level.
   */
  void remove_var(Term& var);

  /**
   * Pick Boolean term from current scope level.
   */
  Term pick_quant_body();

  /**
   * Pick term of Bool SortKind SORT_BOOL and add it to asssumptions list.
   * Requires that terms of SortKind SORT_BOOL exist.
   */
  Term pick_assumption();
  /**
   * Pick assumption out of the assumed assumptions list.
   * Requires that d_assumptions is not empty.
   */
  Term pick_assumed_assumption();

  /**
   * Reset solver manager state into assert mode.
   *
   * After this call, calling
   *   - get_model()
   *   - get_unsat_assumptions()
   *   - get_unsat_core() and
   *   - get_proof()
   * is not possible until after the next SAT call.
   */
  void reset_sat();

  /** Return true if term database contains any value of given sort. */
  bool has_value(Sort sort) const;

  /** Return true if we already created string values with lenght 1. */
  bool has_string_char_value() const;

  /** Return true if term database contains any term. */
  bool has_term() const;
  /**
   * Return true if term database contains any term of given sort kind at given
   * level.
   */
  bool has_term(SortKind sort_kind, size_t level) const;
  /** Return true if term database contains any term of given sort kind. */
  bool has_term(SortKind sort_kind) const;
  /** Return true if term database contains any term of given sort. */
  bool has_term(Sort sort) const;
  /** Return true if d_assumptions is not empty. */
  bool has_assumed() const;
  /** Return true if term database contains a variable. */
  bool has_var() const;
  /**
   * Return true if term database contains a Boolean term in the current scope
   * level.
   */
  bool has_quant_body() const;

  /** Return true if given term has been previously assumed. */
  bool is_assumed(Term term) const;

  /**
   * Return the Term in the Term database that wraps the same solver term
   * with the given sort and sort kind.
   * Returns a nullptr if given Term is not in the term database.
   *
   * Note: We need this for Terms returned by the solver that are only wrapped
   *       solver terms without sort information.
   */
  Term find_term(Term term, Sort sort, SortKind sort_kind);

  /**
   * Return the term with the given id.
   * Note: We only use this for untracing.
   */
  Term get_term(uint64_t id) const;

  /**
   * Map an id from a trace to an actual term ID.
   * Note: Only used for untracing.
   */
  void register_term(uint64_t untraced_id, uint64_t term_id);

  /**
   * Map an id from a trace to an actual sort ID.
   * Note: Only used for untracing.
   */
  void register_sort(uint64_t untraced_id, uint64_t sort_id);

  /**
   * Pick sort.
   * It is not guaranteed that there exist terms of the returned sort.
   */
  Sort pick_sort();
  /**
   * Pick sort of given sort kind. Optionally restrict selection to sorts
   * with terms only if 'with_terms' is true.
   */
  Sort pick_sort(SortKind sort_kind, bool with_terms = true);
  /**
   * Pick sort, but exclude some of them.
   * It is not guaranteed that there exist terms of the returned sort.
   */
  Sort pick_sort(const SortKindSet& exclude_sorts, bool with_terms = true);
  /**
   * Pick bit-vector sort with given bit-width.  Optionally restrict
   * selection to sorts with terms only if 'with_terms' is true.
   */
  Sort pick_sort_bv(uint32_t bw, bool with_terms = true);
  /**
   * Pick bit-vector sort with given maximum bit-width.  Optionally restrict
   * selection to sorts with terms only if 'with_terms' is true.
   */
  Sort pick_sort_bv_max(uint32_t bw_max, bool with_terms = true);

  /**
   * Return true if any sort has been created.
   * This does not guarantee that any terms have been created.
   */
  bool has_sort() const;
  /**
   * Return true if a sort of given kind exists.
   * This does not guarantee that any terms of this sort have been created.
   */
  bool has_sort(SortKind sort_kind) const;
  /**
   * Return true if given sort already exists.
   * This does not guarantee that any terms of this sort have been created.
   */
  bool has_sort(Sort sort) const;
  /**
   * Return true if sorts other than exclude_sorts have been created.
   * This does not guarantee that any terms have been created.
   */
  bool has_sort(const std::unordered_set<SortKind>& exclude_sorts) const;

  /**
   * Return true if a bit-vector sort with given bit-width exists.
   * Optionally restrict selection to sorts with terms only if 'with_terms' is
   * true.
   */
  bool has_sort_bv(uint32_t bw, bool with_terms = true) const;
  /**
   * Return true if a bit-vector sort up to given maximum bit-width exists.
   * Optionally restrict selection to sorts with terms only if 'with_terms' is
   * true.
   */
  bool has_sort_bv_max(uint32_t bw_max, bool with_terms = true) const;

  /**
   * Return the sort with the given id.
   * Note: We only use this for untracing.
   */
  Sort get_sort(uint64_t id) const;

  /**
   * Set d_n_sorts to id.
   * Note: Only used for untracing.
   */
  void set_n_sorts(uint64_t id);

  /**
   * Lookup sort in d_sorts. If a no matching sort is found the given sort is
   * returned.
   */
  Sort find_sort(Sort sort) const;

  /**
   * Pick an option and an option value.
   */
  std::pair<std::string, std::string> pick_option();

  /**
   * True if incremental solving is enabled.
   * (SMT-LIB: option :incremental).
   */
  bool d_incremental = false;
  /**
   * True if model generation is enabled.
   * (SMT-LIB: option :produce-models).
   */
  bool d_model_gen = false;
  /**
   * True if producing unsat assumptions is enabled.
   * (SMT-LIB: option :produce-unsat-assumptions).
   */
  bool d_unsat_assumptions = false;

  /** The number of scope levels previously pushed. */
  uint32_t d_n_push_levels = 0;

  /**
   * True if a previous check-sat call is still 'active', i.e., if no formulas
   * have been asserted or assumed since.
   * While true it is save to check failed assumptions and query model values.
   */
  bool d_sat_called = false;

  /** The result of the previous sat call. */
  Solver::Result d_sat_result = Solver::Result::UNKNOWN;

  /** The number of check-sat calls issued. */
  uint32_t d_n_sat_calls = 0;

  /** Statistics. */
  Stats d_stats;

  /** True to restrict arithmetic operators to linear fragment. */
  bool d_arith_linear = false;

  /**
   * True if every non-return trace call should be preceded by a
   * 'set-seed <seed>' line. We need to provide this option in the solver
   * manager for actions to have access to it.
   */
  bool d_trace_seeds = false;

  /**
   * True if cross checking is enabled. We need to provide this option in the
   * solver manager for the actions to have access to it.
   * */
  bool d_cross_check = false;

  /**
   * True if all symbols for terms should be of the form '_sX' rather than
   * a random string.
   */
  bool d_simple_symbols = false;

 private:
  /**
   * Determine and populate set of enabled theories.
   * All theories supported by a solvers are by default enabled and can
   * optionally be disabled (the latter is still TODO).
   */
  void add_enabled_theories(TheoryIdVector& enabled_theories);

  /**
   * Populate sort kinds database with enabled sort kinds.
   * Sort kinds are enabled based on the set of enabled theories.
   */
  void add_sort_kinds();
  /**
   * Populate operator kinds database with enabled operator kinds.
   * Operator kinds are enabled based on the set of enabled theories.
   */
  void add_op_kinds();

  /** Clear set of assumptions. */
  void clear_assumptions();

#if 0
  template <typename TKind,
            typename TKindData,
            typename TKindMap,
            typename TKindVector>
  TKindData& pick_kind(TKindMap& map,
                       TKindVector* kinds1,
                       TKindVector* kinds2 = nullptr);
#endif

  /** Generalized helper to pick a sort or operator kind. */
  template <typename TKind,
            typename TKindData,
            typename TKindMap>
  TKindData& pick_kind(TKindMap& map);

  /**
   * The activated solver.
   * No calls to the API of the underlying solver are issued from the solver
   * manager, only calls to the API of the smtmbt::Solver object.
   */
  std::unique_ptr<Solver> d_solver;

  /** The random number generator. */
  RNGenerator& d_rng;

  /** The stream to capture the API trace. */
  std::ostream& d_trace;

  /** Term id counter. */
  uint64_t d_n_terms = 0;
  /** Sort id counter. */
  uint64_t d_n_sorts = 0;

  /** Map SortKind to number of created terms of that SortKind. */
  std::unordered_map<SortKind, uint64_t> d_n_sort_terms;

  /** The set of enabled sort kinds. Maps SortKind to SortKindData. */
  SortKindMap d_sort_kinds;
  /** The set of enabled operator kinds. Maps OpKind to Op. */
  OpKindMap d_op_kinds;

  /** The set of enabled theories. */
  TheoryIdSet d_enabled_theories;

  /** Maintain all created sorts. */
  SortSet d_sorts;

  /** Map sort kind -> sorts. */
  std::unordered_map<SortKind, SortSet> d_sort_kind_to_sorts;

  /** The set of already assumed formulas. */
  std::unordered_set<Term, HashTerm> d_assumptions;

  /** Vector of available solver options */
  SolverOptions& d_solver_options;

  std::unordered_set<std::string> d_used_solver_options;

  /** Counter to create simple symbol names when option is enabled. */
  uint32_t d_n_symbols = 0;

  /** Term database */
  TermDb d_term_db;

  /** Map untraced ids to corresponding Terms. */
  std::unordered_map<uint64_t, Term> d_untraced_terms;

  /** Map untraced ids to corresponding Terms. */
  std::unordered_map<uint64_t, Sort> d_untraced_sorts;

  /** Set of currently created string values with length 1. */
  std::unordered_set<Term, HashTerm> d_string_char_values;

  /**
   * List of terms for which we have to trace a ("phantom") action
   * 'term-get-sort'.
   *
   * When adding terms of parameterized sort, e.g., bit-vectors or
   * floating-points, or when creating terms with a Real operator, that is
   * actually of sort Int, it can happen that the resulting term has yet unknown
   * sort, i.e., a sort that has not previously been created via ActionMksort.
   * In order to ensure that the untracer can map such sorts back correctly,
   * we have to trace a "phantom" action (= an action, that is only executed
   * when untracing) for new sorts.
   *
   * This vector holds all terms that have been created while executing the
   * previous action with yet unseen sort.
   */
  std::vector<Term> d_pending_get_sorts;
};

/* -------------------------------------------------------------------------- */

}  // namespace smtmbt
#endif
