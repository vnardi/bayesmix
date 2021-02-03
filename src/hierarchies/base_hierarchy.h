#ifndef BAYESMIX_HIERARCHIES_BASE_HIERARCHY_H_
#define BAYESMIX_HIERARCHIES_BASE_HIERARCHY_H_

#include <google/protobuf/message.h>

#include <Eigen/Dense>
#include <memory>
#include <random>
#include <set>
#include <stan/math/prim.hpp>

#include "hierarchy_id.pb.h"
#include "marginal_state.pb.h"
#include "src/utils/rng.h"

//! Abstract base template class for a hierarchy object.

//! This template class represents a hierarchy object in a generic iterative
//! BNP algorithm, that is, a single set of unique values with their own prior
//! distribution attached to it. These values are part of the Markov chain's
//! state chain (which includes multiple hierarchies) and are simply referred
//! to as the state of the hierarchy. This object also corresponds to a single
//! cluster in the algorithm, in the sense that its state is the set of
//! parameters for the distribution of the data points that belong to it. Since
//! the prior distribution for the state is often the same across multiple
//! different hierarchies, the hyperparameters object is accessed via a shared
//! pointer. Lastly, any hierarchy that inherits from this class contains
//! multiple ways of updating the state, either via prior or posterior
//! distributions, and of evaluating the distribution of the data, either its
//! likelihood (whose parameters are the state) or its marginal distribution.

class AbstractHierarchy {
 protected:
  std::set<int> cluster_data_idx;
  int card = 0;
  double log_card = stan::math::NEGATIVE_INFTY;
  std::shared_ptr<google::protobuf::Message> prior;

  virtual void update_summary_statistics(const Eigen::VectorXd &datum,
                                         bool add) = 0;
  virtual void initialize_hypers() = 0;

 public:
  virtual ~AbstractHierarchy() = default;
  virtual std::shared_ptr<AbstractHierarchy> clone() const = 0;
  virtual bayesmix::HierarchyId get_id() const = 0;

  virtual void initialize() = 0;

  virtual bool is_multivariate() const = 0;
  virtual bool is_dependent() const { return false; }
  virtual bool is_conjugate() const { return true; }
  virtual void update_hypers(
      const std::vector<bayesmix::MarginalState::ClusterState> &states) = 0;

  virtual double like_lpdf(const Eigen::RowVectorXd &datum) const = 0;
  //! Evaluates the log-likelihood of data in the given points
  virtual Eigen::VectorXd like_lpdf_grid(const Eigen::MatrixXd &data) const;
  //! Evaluates the log-marginal distribution of data in a single point
  virtual double marg_lpdf(const Eigen::RowVectorXd &datum) const = 0;
  //! Evaluates the log-marginal distribution of data in the given points
  virtual Eigen::VectorXd marg_lpdf_grid(const Eigen::MatrixXd &data) const;

  // SAMPLING FUNCTIONS
  //! Generates new values for state from the centering prior distribution
  virtual void draw() = 0;
  //! Generates new values for state from the centering posterior distribution
  virtual void sample_given_data() = 0;
  virtual void sample_given_data(const Eigen::MatrixXd &data) = 0;
  virtual void write_state_to_proto(google::protobuf::Message *out) const = 0;
  virtual void write_hypers_to_proto(google::protobuf::Message *out) const = 0;
  virtual void set_state_from_proto(
      const google::protobuf::Message &state_) = 0;

  virtual void create_empty_prior() = 0;

  void check_prior_is_set();
  int get_card() const { return card; }
  double get_log_card() const { return log_card; }
  std::set<int> get_data_idx() { return cluster_data_idx; }
  google::protobuf::Message *prior_proto() {
    if (prior == nullptr) create_empty_prior();
    
    return prior.get();
  }

  virtual void clear_data() = 0;
  virtual void add_datum(const int id, const Eigen::VectorXd &datum);
  virtual void remove_datum(const int id, const Eigen::VectorXd &datum);
};

template <class Derived, typename State, typename Hyperparams, typename Prior>
class BaseHierarchy : public AbstractHierarchy {
 protected:
  State state;
  // HYPERPARAMETERS
  std::shared_ptr<Hyperparams> hypers;

  virtual Hyperparams get_posterior_parameters() = 0;
  void create_empty_prior() { prior.reset(new Prior); }

 public:
  // DESTRUCTOR AND CONSTRUCTORS
  ~BaseHierarchy() = default;
  BaseHierarchy() = default;
  virtual std::shared_ptr<AbstractHierarchy> clone() const override {
    auto out = std::make_shared<Derived>(static_cast<Derived const &>(*this));
    out->clear_data();
    return out;
  }

  State get_state() const { return state; }
  Hyperparams get_hypers() const { return *hypers; }
};

#endif  // BAYESMIX_HIERARCHIES_BASE_HIERARCHY_H_
