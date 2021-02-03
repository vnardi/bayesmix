#ifndef BAYESMIX_HIERARCHIES_NNIG_HIERARCHY_H_
#define BAYESMIX_HIERARCHIES_NNIG_HIERARCHY_H_

#include <google/protobuf/stubs/casts.h>

#include <Eigen/Dense>
#include <memory>

#include "base_hierarchy.h"
#include "hierarchy_id.pb.h"
#include "hierarchy_prior.pb.h"
#include "marginal_state.pb.h"

//! Normal Normal-InverseGamma hierarchy for univariate data.

//! This class represents a hierarchy, i.e. a cluster, whose univariate data
//! are distributed according to a normal likelihood, the parameters of which
//! have a Normal-InverseGamma centering distribution. That is:
//!           phi = (mu,sig)     (state);
//! f(x_i|mu,sig) = N(mu,sig^2)  (data likelihood);
//!    (mu,sig^2) ~ G            (unique values distribution);
//!             G ~ MM           (mixture model);
//!            G0 = N-IG         (centering distribution).
//! state[0] = mu is called location, and state[1] = sig is called scale. The
//! state hyperparameters, contained in the Hypers object, are (mu_0, lambda0,
//! alpha0, beta0), all scalar values. Note that this hierarchy is conjugate,
//! thus the marginal and the posterior distribution are available in closed
//! form and Neal's algorithm 2 may be used with it.

namespace NNIG {
struct State {
  double mean, var;
};

struct Hyperparams {
  double mean, var_scaling, shape, scale;
};

};  // namespace NNIG

class NNIGHierarchy
    : public BaseHierarchy<NNIGHierarchy, NNIG::State, NNIG::Hyperparams,
                           bayesmix::NNIGPrior> {
 public:
  void clear_data() override;

 protected:
  double data_sum = 0;
  double data_sum_squares = 0;
  // STATE


  void update_summary_statistics(const Eigen::VectorXd &datum,
                                 bool add) override;

  void initialize_hypers() override;

  // AUXILIARY TOOLS
  //! Returns updated values of the prior hyperparameters via their posterior
  NNIG::Hyperparams get_posterior_parameters() override;

  std::shared_ptr<bayesmix::NNIGPrior> cast_prior_proto() {
    return std::dynamic_pointer_cast<bayesmix::NNIGPrior>(prior);
  }

 public:
  void initialize() override;

  //! Returns true if the hierarchy models multivariate data (here, false)
  bool is_multivariate() const override { return false; }

  void update_hypers(const std::vector<bayesmix::MarginalState::ClusterState>
                         &states) override;

  // DESTRUCTOR AND CONSTRUCTORS
  ~NNIGHierarchy() = default;
  NNIGHierarchy() = default;

  // EVALUATION FUNCTIONS
  //! Evaluates the log-likelihood of data in a single point
  double like_lpdf(const Eigen::RowVectorXd &datum) const override;
  //! Evaluates the log-marginal distribution of data in a single point
  double marg_lpdf(const Eigen::RowVectorXd &datum) const override;

  // SAMPLING FUNCTIONS
  //! Generates new values for state from the centering prior distribution
  void draw() override;
  //! Generates new values for state from the centering posterior distribution
  void sample_given_data() override;
  void sample_given_data(const Eigen::MatrixXd &data) override;

  // GETTERS AND SETTERS

  void set_state_from_proto(const google::protobuf::Message &state_) override;
  void write_state_to_proto(google::protobuf::Message *out) const override;
  void write_hypers_to_proto(google::protobuf::Message *out) const override;

  bayesmix::HierarchyId get_id() const override {
    return bayesmix::HierarchyId::NNIG;
  }
};

#endif  // BAYESMIX_HIERARCHIES_NNIG_HIERARCHY_H_
