// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the Apache 2.0 License.
#pragma once

#include "endpoint_registry.h"
#include "json_handler.h"
#include "metrics.h"

namespace metrics
{
  class Tracker
  {
  private:
    metrics::Metrics metrics;

  public:
    ccf::CommandEndpointFunction get_endpoint_handler()
    {
      auto get_metrics = [this](auto&, nlohmann::json&&) {
        auto result = metrics.get_metrics_report();
        return ccf::make_success(result);
      };

      return ccf::json_command_adapter(get_metrics);
    }

    void install_endpoint(ccf::EndpointRegistry& reg)
    {
      reg
        .make_command_endpoint(
          "metrics", HTTP_GET, get_endpoint_handler(), ccf::no_auth_required)
        .set_auto_schema<void, metrics::Report>()
        .set_execute_outside_consensus(
          ccf::endpoints::ExecuteOutsideConsensus::Locally)
        .install();
    }

    void tick(
      std::chrono::milliseconds elapsed, kv::Consensus::Statistics stats)
    {
      metrics.track_tx_rates(elapsed, stats);
    }
  };
}