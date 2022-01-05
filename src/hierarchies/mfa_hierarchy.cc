#include "mfa_hierarchy.h"

#include <google/protobuf/stubs/casts.h>

#include <Eigen/Dense>
#include <cassert>
#include <iostream>
#include <stan/math/prim/prob.hpp>
#include <vector>

#include "algorithm_state.pb.h"
#include "hierarchy_prior.pb.h"
#include "ls_state.pb.h"
#include "src/utils/proto_utils.h"
#include "src/utils/rng.h"

double MFAHierarchy::like_lpdf(const Eigen::RowVectorXd& datum) const {
  return stan::math::multi_normal_lpdf(
      datum, state.mu,
      state.Lambda * state.Lambda.transpose() +
          Eigen::MatrixXd(state.psi.asDiagonal()));
}

MFA::State MFAHierarchy::draw(const MFA::Hyperparams& params) {
  auto& rng = bayesmix::Rng::Instance().get();
  MFA::State out;
  out.mu = params.mutilde;
  out.psi = params.beta / (params.alpha0 + 1.);
  out.Eta = Eigen::MatrixXd::Zero(card, params.q);
  out.Lambda = Eigen::MatrixXd::Zero(p, params.q);
  for (size_t j = 0; j < p; j++) {
    out.mu[j] =
        stan::math::normal_rng(params.mutilde[j], sqrt(params.phi), rng);

    out.psi[j] = stan::math::inv_gamma_rng(params.alpha0, params.beta[j], rng);

    for (size_t i = 0; i < params.q; i++) {
      out.Lambda(j, i) = stan::math::normal_rng(0, 1, rng);
    }
  }

  for (size_t i = 0; i < card; i++) {
    for (size_t j = 0; j < params.q; j++) {
      out.Eta(i, j) = stan::math::normal_rng(0, 1, rng);
    }
  }

  return out;
}

void MFAHierarchy::initialize_state() {
  state.mu = hypers->mutilde;
  state.psi = hypers->beta / (hypers->alpha0 + 1.);
  state.Eta = Eigen::MatrixXd::Zero(card, hypers->q);
  state.Lambda = Eigen::MatrixXd::Zero(p, hypers->q);
}

void MFAHierarchy::initialize_hypers() {
  if (prior->has_fixed_values()) {
    // Set values
    hypers->mutilde = bayesmix::to_eigen(prior->fixed_values().mutilde());
    p = hypers->mutilde.size();
    hypers->beta = bayesmix::to_eigen(prior->fixed_values().beta());
    hypers->phi = prior->fixed_values().phi();
    hypers->alpha0 = prior->fixed_values().alpha0();
    hypers->q = prior->fixed_values().q();

    // Check validity
    if (p != hypers->beta.rows()) {
      throw std::invalid_argument(
          "Hyperparameters dimensions are not consistent");
    }
    for (size_t j = 0; j < p; j++) {
      if (hypers->beta[j] <= 0) {
        throw std::invalid_argument("Shape parameter must be > 0");
      }
    }
    if (hypers->alpha0 <= 0) {
      throw std::invalid_argument("Scale parameter must be > 0");
    }
    if (hypers->phi <= 0) {  // TODO check
      throw std::invalid_argument("Diffusion parameter must be > 0");
    }
    if (hypers->q <= 0) {
      throw std::invalid_argument("Number of factors must be > 0");
    }
  }

  else {
    throw std::invalid_argument("Unrecognized hierarchy prior");
  }
}

void MFAHierarchy::update_hypers(
    const std::vector<bayesmix::AlgorithmState::ClusterState>& states) {
  auto& rng = bayesmix::Rng::Instance().get();
  if (prior->has_fixed_values()) {
    return;
  }

  else {
    throw std::invalid_argument("Unrecognized hierarchy prior");
  }
}

void MFAHierarchy::update_summary_statistics(const Eigen::RowVectorXd& datum,
                                             bool add) {
  if (add) {
    data_sum += datum;
    data.push_back(datum);
  } else {
    Eigen::VectorXd datumtemp(datum);
    data_sum -= datum;
    for (size_t i = 0; i < data.size(); ++i) {
      if ((data[i] - datumtemp).squaredNorm() < DBL_EPSILON) {
        data.erase(data.begin() + i);
      }
    }
  }
}

void MFAHierarchy::clear_summary_statistics() {
  data_sum = Eigen::VectorXd::Zero(p);
  data.clear();
}

void MFAHierarchy::set_state_from_proto(
    const google::protobuf::Message& state_) {
  auto& statecast = downcast_state(state_);
  state.mu = bayesmix::to_eigen(statecast.mfa_state().mu());
  state.psi = bayesmix::to_eigen(statecast.mfa_state().psi());
  state.Eta = bayesmix::to_eigen(statecast.mfa_state().eta());
  state.Lambda = bayesmix::to_eigen(statecast.mfa_state().lambda());
  set_card(statecast.cardinality());
}

std::shared_ptr<bayesmix::AlgorithmState::ClusterState>
MFAHierarchy::get_state_proto() const {
  bayesmix::MFAState state_;
  bayesmix::to_proto(state.mu, state_.mutable_mu());
  bayesmix::to_proto(state.psi, state_.mutable_psi());
  bayesmix::to_proto(state.Eta, state_.mutable_eta());
  bayesmix::to_proto(state.Lambda, state_.mutable_lambda());

  auto out = std::make_shared<bayesmix::AlgorithmState::ClusterState>();
  out->mutable_mfa_state()->CopyFrom(state_);
  return out;
}

void MFAHierarchy::set_hypers_from_proto(
    const google::protobuf::Message& hypers_) {
  auto& hyperscast = downcast_hypers(hypers_).mfa_state();
  hypers->mutilde = bayesmix::to_eigen(hyperscast.mutilde());
  hypers->alpha0 = hyperscast.alpha0();
  hypers->beta = bayesmix::to_eigen(hyperscast.beta());
  hypers->phi = hyperscast.phi();
  hypers->q = hyperscast.q();
}

std::shared_ptr<bayesmix::AlgorithmState::HierarchyHypers>
MFAHierarchy::get_hypers_proto() const {
  bayesmix::MFAPriorDistribution hypers_;
  bayesmix::to_proto(hypers->mutilde, hypers_.mutable_mutilde());
  bayesmix::to_proto(hypers->beta, hypers_.mutable_beta());
  hypers_.set_alpha0(hypers->alpha0);
  hypers_.set_phi(hypers->phi);
  hypers_.set_q(hypers->q);

  auto out = std::make_shared<bayesmix::AlgorithmState::HierarchyHypers>();
  out->mutable_mfa_state()->CopyFrom(hypers_);
  return out;
}

void MFAHierarchy::sample_full_cond(bool update_params) {
  if (this->card == 0) {
    // No posterior update possible
    sample_prior();
  } else {
    sample_Eta();
    sample_mu();
    sample_psi();
    sample_Lambda();
  }
}

void MFAHierarchy::sample_Eta() {
  auto& rng = bayesmix::Rng::Instance().get();

  Eigen::MatrixXd Sigmaeta(
      (Eigen::MatrixXd::Identity(hypers->q, hypers->q) +
       state.Lambda.transpose() *
           Eigen::MatrixXd(state.psi.cwiseInverse().asDiagonal()) *
           state.Lambda)
          .inverse());

  Eigen::MatrixXd temp(state.psi.cwiseInverse().asDiagonal());
  if (state.Eta.rows() != card) {
    state.Eta.resize(card, state.Eta.cols());
    state.Eta = Eigen::MatrixXd::Zero(card, state.Eta.cols());
  }
  for (size_t i = 0; i < card; i++) {
    state.Eta.row(i) = (stan::math::multi_normal_rng(
        Sigmaeta * (state.Lambda.transpose()) * temp * (data[i] - state.mu),
        Sigmaeta, rng));
  }
}

void MFAHierarchy::sample_mu() {
  auto& rng = bayesmix::Rng::Instance().get();

  Eigen::MatrixXd Sigmamu =
      (hypers->phi * Eigen::MatrixXd::Identity(p, p) +
       card * Eigen::MatrixXd(state.psi.cwiseInverse().asDiagonal()))
          .inverse();

  Eigen::VectorXd Somma = Eigen::VectorXd::Zero(p);

  for (size_t i = 0; i < card; i++) {
    Eigen::VectorXd riga = state.Eta.row(i);
    Somma += state.Lambda * riga;
  }
  Eigen::VectorXd mumean =
      Sigmamu * (hypers->phi * hypers->mutilde +
                 Eigen::MatrixXd(state.psi.cwiseInverse().asDiagonal()) *
                     (data_sum - Somma));

  state.mu = stan::math::multi_normal_rng(mumean, Sigmamu, rng);
}

void MFAHierarchy::sample_Lambda() {
  auto& rng = bayesmix::Rng::Instance().get();
  Eigen::MatrixXd datamatrix(p, card);
  for (size_t i = 0; i < card; i++) {
    datamatrix.col(i) = data[i];
  }
  for (size_t j = 0; j < p; j++) {
    Eigen::MatrixXd Sigmalambda =
        (Eigen::MatrixXd::Identity(hypers->q, hypers->q) +
         state.Eta.transpose() / state.psi[j] * state.Eta)
            .inverse();
    Eigen::VectorXd vectortemp(datamatrix.row(j));
    state.Lambda.row(j) = stan::math::multi_normal_rng(
        Sigmalambda * state.Eta.transpose() *
            (vectortemp - Eigen::VectorXd::Constant(card, state.mu[j])) /
            state.psi[j],
        Sigmalambda, rng);
  }
}

void MFAHierarchy::sample_psi() {
  auto& rng = bayesmix::Rng::Instance().get();

  for (size_t j = 0; j < p; j++) {
    double S = 0;
    for (size_t i = 0; i < card; i++) {
      S += std::pow((data[i][j] - state.mu[j] -

                     state.Lambda.row(j).dot(state.Eta.row(i))),
                    2);
    }
    state.psi[j] = stan::math::inv_gamma_rng(hypers->alpha0 + card / 2,
                                             hypers->beta[j] + S / 2, rng);
  }
}
