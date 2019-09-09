#ifndef __SMTMBT__SOLVER_MANAGER_H
#define __SMTMBT__SOLVER_MANAGER_H

#include <cassert>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "solver.hpp"
#include "sort.hpp"
#include "theory.hpp"
#include "util.hpp"

namespace smtmbt {

/* -------------------------------------------------------------------------- */

class SolverManager
{
 public:
  using TermMap = std::unordered_map<Term, size_t, HashTerm>;
  using SortMap = std::unordered_map<Sort, TermMap, HashSort>;
  using SortSet = std::unordered_set<Sort, HashSort>;

  /* Statistics. */
  struct Stats
  {
    uint32_t inputs = 0; /* constants, variables */
    uint32_t terms  = 0; /* all terms, including inputs */
  };

  SolverManager(Solver* solver, RNGenerator& rng);
  ~SolverManager() = default;

  void clear();

  Solver& get_solver();
#if 0
  OpKindMap& get_op_kinds();
  SortKindMap& get_sort_kinds();
#endif
  SortKinds& get_theory_to_sort_kinds();
  OpKinds& get_theory_to_op_kinds();

  void set_rng(RNGenerator& rng);
  RNGenerator& get_rng() const;

  const TheoryIdSet& get_enabled_theories() const;

  void add_input(Term term, Sort sort, TheoryId theory);
  void add_term(Term term, Sort sort, TheoryId theory);
  void add_sort(Sort sort, TheoryId theory);

#if 1
  SortKind pick_sort_kind(SortKindVector& kinds);
  OpKind pick_op_kind(OpKindVector& kinds);
  OpKind pick_op_kind(OpKindVector& kinds1, OpKindVector& kinds2);
#endif

  SortKindData& pick_sort_kind_data();
  OpKindData& pick_op_kind_data();

#if 1
  SortKindData& pick_sort_kind_data(SortKindVector& kinds);
  OpKindData& pick_op_kind_data(OpKindVector& kinds);
  OpKindData& pick_op_kind_data(OpKindVector& kinds1, OpKindVector& kinds2);

  TheoryId pick_theory();
  TheoryId pick_theory_with_sorts();
  TheoryId pick_theory_with_terms();

  TheoryId get_theory(Sort sort);

  Term pick_term(TheoryId theory);
  Term pick_term(Sort sort);

  bool has_term();
  bool has_term(TheoryId theory);
  bool has_term(Sort sort);

  Sort pick_sort();
  Sort pick_sort(TheoryId theory);
  Sort pick_sort_with_terms(TheoryId theory);

  bool has_sort() const;
  bool has_sort(Sort sort) const;
  bool has_sort(TheoryId theory) const;

  Sort get_sort(Term term);
#endif

  Stats d_stats;

  // protected:
  // TODO: move that to class AbsTerm AbsSort
  /* Solver specific implementations. */
  //  virtual Term copy_term(Term term);
  //  virtual Sort copy_sort(Sort sort);

 private:
  void add_enabled_theories();
  void add_sort_kinds();
  void add_op_kinds();

#if 1
  template <typename TKind,
            typename TKindData,
            typename TKindMap,
            typename TKindVector>
  TKindData& pick_kind(TKindMap& map,
                       TKindVector* kinds1,
                       TKindVector* kinds2 = nullptr);
#endif

  template <typename TKind,
            typename TKindData,
            typename TKindMap>
  TKindData& pick_kind(TKindMap& map);

  std::unique_ptr<Solver> d_solver;
  RNGenerator& d_rng;

  OpKindMap d_op_kinds;
  SortKindMap d_sort_kinds;

  TheoryIdSet d_enabled_theories;

  /* Map theory -> sort kinds. */
  SortKinds d_theory_to_sort_kinds;
  /* Map theory of term arguments -> op kinds. */
  OpKinds d_theory_to_op_kinds;
  /* Map theory -> sorts. */
  std::unordered_map<TheoryId, SortSet> d_theory_to_sorts;
  /* Map sort -> theory. */
  std::unordered_map<Sort, TheoryId, HashSort> d_sort_to_theory;
  /* Map theory -> (sort -> terms). */
  std::unordered_map<TheoryId, SortMap> d_terms;
  /* Map term -> sort. */
  std::unordered_map<Term, Sort, HashTerm> d_term_to_sort;
};

/* -------------------------------------------------------------------------- */

#if 0

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
class SolverManager
{
 public:
  using TermMap = std::unordered_map<TTerm, size_t, THashTerm>;
  using SortMap = std::unordered_map<TSort, TermMap, THashSort>;
  using SortSet = std::unordered_set<TSort, THashSort>;

  /* Statistics. */
  struct Stats
  {
    uint32_t inputs = 0; /* constants, variables */
    uint32_t terms  = 0; /* all terms, including inputs */
  };

  SolverManager(RNGenerator& rng) : d_fsm(rng), d_solver(nullptr), d_rng(rng) {}
  // TODO: copy/assignment constructors?
  ~SolverManager() = default;

  void set_solver(TSolver s);
  TSolver get_solver();

  void set_rng(RNGenerator& rng);
  RNGenerator& get_rng();

  void run();

  void add_input(TTerm term, TheoryId theory);
  void add_term(TTerm term, TheoryId theory);
  void add_sort(TSort sort, TheoryId theory);

  TTerm pick_term();
  TTerm pick_term(TheoryId theory);
  TTerm pick_term(TTerm term);
  TTerm pick_term(TSort sort);

  bool has_term();
  bool has_term(TheoryId theory);
  bool has_term(TSort sort);

  TSort pick_sort();
  TSort pick_sort(TheoryId theory);
  TSort pick_sort_with_terms();
  TSort pick_sort_with_terms(TheoryId theory);

  bool has_sort();
  bool has_sort(TSort sort);
  bool has_sort(TheoryId theory);

  TheoryId pick_theory_with_sorts();
  TheoryId pick_theory_with_terms();

  TheoryId get_theory(TSort sort);

  virtual TSort get_sort(TTerm term) = 0;

  Stats d_stats;

 protected:
  virtual void configure() = 0;

  template <class T>
  T* new_action();

  /* Solver specific implementations. */
  virtual TTerm copy_term(TTerm term);
  virtual TSort copy_sort(TSort sort);

  FSM d_fsm;
  TSolver d_solver;
  RNGenerator& d_rng;

  /* Map theory -> sorts. */
  std::unordered_map<TheoryId, SortSet> d_theory_to_sorts;
  /* Map sort -> theory. */
  std::unordered_map<TSort, TheoryId, THashSort> d_sort_to_theory;
  /* Map theory -> (sort -> terms). */
  std::unordered_map<TheoryId, SortMap> d_terms;

 private:
  std::unordered_map<std::string, std::unique_ptr<Action>> d_actions;
};

/* ========================================================================== */

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
void
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::set_solver(
    TSolver s)
{
  d_solver = s;
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
TSolver
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::get_solver()
{
  return d_solver;
}

/* -------------------------------------------------------------------------- */

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
void
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::set_rng(
    RNGenerator& rng)
{
  d_rng = rng;
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
RNGenerator&
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::get_rng()
{
  return d_rng;
}

/* -------------------------------------------------------------------------- */

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
void
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::run()
{
  d_fsm.run();
}

/* -------------------------------------------------------------------------- */

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
void
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::add_input(
    TTerm term, TheoryId theory)
{
  d_stats.inputs += 1;
  add_term(term, theory);
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
void
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::add_term(
    TTerm term, TheoryId theory)
{
  d_stats.terms += 1;

  TSort sort = get_sort(term);
  add_sort(sort, theory);

  if (d_terms.find(theory) == d_terms.end())
  {
    d_terms.emplace(theory, SortMap());
  }

  assert(d_terms.find(theory) != d_terms.end());

  SortMap& map = d_terms[theory];
  map.emplace(sort, TermMap());

  assert(map.find(sort) != map.end());
  if (map[sort].find(term) == map[sort].end())
  {
    map[sort].emplace(copy_term(term), 0);
  }
  else
  {
    map[sort][term] += 1;
  }
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
void
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::add_sort(
    TSort sort, TheoryId theory)
{
  if (d_sort_to_theory.find(sort) == d_sort_to_theory.end())
  {
    d_sort_to_theory.emplace(copy_sort(sort), theory);
  }

  if (d_theory_to_sorts.find(theory) == d_theory_to_sorts.end())
  {
    d_theory_to_sorts.emplace(theory, std::unordered_set<TSort, THashSort>());
  }
  d_theory_to_sorts[theory].emplace(sort);
}

/* -------------------------------------------------------------------------- */

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
TTerm
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::pick_term()
{
  TheoryId theory;
  theory = pick_theory_with_terms();
  return pick_term(theory);
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
TTerm
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::pick_term(
    TheoryId theory)
{
  assert(d_terms.find(theory) != d_terms.end());
  if (theory == THEORY_ALL)
  {
    theory = pick_theory_with_terms();
  }
  TSort sort;
  sort = pick_sort_with_terms(theory);
  assert(get_theory(sort) == theory);
  TTerm res = pick_term(sort);
  assert(get_sort(res) == sort);
  assert(get_theory(get_sort(res)) == theory);
  return res;
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
TTerm
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::pick_term(
    TTerm term)
{
  TSort sort = get_sort(term);
  return pick_term(sort);
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
TTerm
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::pick_term(
    TSort sort)
{
  TheoryId theory = get_theory(sort);
  assert(d_terms.find(theory) != d_terms.end());
  assert(d_terms[theory].find(sort) != d_terms[theory].end());
  TermMap& map = d_terms[theory][sort];
  assert(!map.empty());
  auto it = map.begin();
  if (map.size() > 1)
  {
    std::advance(it, d_rng.pick_uint32() % map.size());
  }
  // TODO: increment ref counter
  return it->first;
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
bool
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::has_term()
{
  for (const auto t : d_terms)
  {
    for (const auto s : t.second)
    {
      if (!s.second.empty()) return true;
    }
  }
  return false;
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
bool
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::has_term(
    TheoryId theory)
{
  if (theory == THEORY_ALL) return has_term();

  if (d_terms.find(theory) == d_terms.end()) return false;

  for (const auto s : d_terms[theory])
  {
    if (!s.second.empty()) return true;
  }
  return false;
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
bool
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::has_term(TSort sort)
{
  return !d_terms[get_theory(sort)][sort].empty();
}

/* -------------------------------------------------------------------------- */

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
TSort
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::pick_sort()
{
  TheoryId theory = pick_theory_with_sorts();
  return pick_sort(theory);
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
TSort
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::pick_sort(
    TheoryId theory)
{
  assert(d_theory_to_sorts.find(theory) != d_theory_to_sorts.end());
  assert(!d_theory_to_sorts[theory].empty());

  SortSet& set = d_theory_to_sorts[theory];
  auto it      = set.begin();
  std::advance(it, d_rng.pick_uint32() % set.size());
  return *it;
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
TSort
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::
    pick_sort_with_terms()
{
  TheoryId theory = pick_theory_with_terms();
  return pick_sort_with_terms(theory);
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
TSort
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::
    pick_sort_with_terms(TheoryId theory)
{
  SortMap& map = d_terms[theory];
  assert(!map.empty());

  if (theory == THEORY_ALL) theory = pick_theory_with_terms();

  auto it = map.begin();
  if (map.size() > 1)
  {
    std::advance(it, d_rng.pick_uint32() % map.size());
  }
  return it->first;
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
bool
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::has_sort()
{
  return !d_sort_to_theory.empty();
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
bool
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::has_sort(TSort sort)
{
  return d_sort_to_theory.find(sort) != d_sort_to_theory.end();
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
bool
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::has_sort(
    TheoryId theory)
{
  if (d_theory_to_sorts.find(theory) == d_theory_to_sorts.end()) return false;
  return !d_theory_to_sorts[theory].empty();
}

/* -------------------------------------------------------------------------- */

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
TheoryId
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::
    pick_theory_with_sorts()
{
  assert(d_theory_to_sorts.size());
  auto it = d_theory_to_sorts.begin();
  std::advance(it, d_rng.pick_uint32() % d_theory_to_sorts.size());
  return it->first;
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
TheoryId
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::
    pick_theory_with_terms()
{
  assert(d_terms.size());
  auto it = d_terms.begin();
  std::advance(it, d_rng.pick_uint32() % d_terms.size());
  assert(!it->second.empty());
  return it->first;
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
TheoryId
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::get_theory(
    TSort sort)
{
  assert(has_sort(sort));
  return d_sort_to_theory[sort];
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
template <class T>
T*
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::new_action()
{
  static_assert(std::is_base_of<Action, T>::value,
                "expected class (derived from) Action");
  T* action             = new T(this);
  const std::string& id = action->get_id();
  if (d_actions.find(id) == d_actions.end())
  {
    d_actions[id].reset(action);
  }
  else
  {
    delete action;
  }
  return static_cast<T*>(d_actions[id].get());
}

/* -------------------------------------------------------------------------- */

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
TTerm
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::copy_term(
    TTerm term)
{
  return term;
}

template <typename TSolver,
          typename TTerm,
          typename TSort,
          typename THashTerm,
          typename THashSort>
TSort
SolverManager<TSolver, TTerm, TSort, THashTerm, THashSort>::copy_sort(
    TSort sort)
{
  return sort;
}
#endif

/* -------------------------------------------------------------------------- */

}  // namespace smtmbt
#endif
