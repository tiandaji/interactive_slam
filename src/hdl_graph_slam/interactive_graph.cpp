#include <hdl_graph_slam/interactive_graph.hpp>

#include <chrono>
#include <boost/filesystem.hpp>
#include <hdl_graph_slam/graph_slam.hpp>
#include <hdl_graph_slam/information_matrix_calculator.hpp>


namespace hdl_graph_slam {

  InteractiveGraph::InteractiveGraph()
  : GraphSLAM("lm_var"),
  iterations(0),
  chi2_before(0.0),
  chi2_after(0.0),
  elapsed_time_msec(0.0)
  {
    inf_calclator.reset(new InformationMatrixCalculator());
    inf_calclator->load(params);
  }

  InteractiveGraph::~InteractiveGraph() {

  }

  bool InteractiveGraph::load_map_data(const std::string& directory) {
    if(!load(directory + "/graph.g2o")) {
      return false;
    }

    if(!load_keyframes(directory)) {
      return false;
    }

    return true;
  }

  bool InteractiveGraph::load_keyframes(const std::string& directory) {
    for(int i = 0;; i++) {
      std::string keyframe_dir = (boost::format("%s/%06d") % directory % i).str();
      if(!boost::filesystem::is_directory(keyframe_dir)) {
        break;
      }

      KeyFrame::Ptr keyframe = std::make_shared<KeyFrame>(keyframe_dir, graph.get());
      if(!keyframe->node) {
        std::cerr << "error : failed to load keyframe!!" << std::endl;
        std::cerr << "      : " << keyframe_dir << std::endl;
        return false;
      }

      keyframes[keyframe->id()] = keyframe;
    }

    return true;
  }

  g2o::EdgeSE3* InteractiveGraph::add_edge(const KeyFrame::Ptr& key1, const KeyFrame::Ptr& key2, const Eigen::Isometry3d& relative_pose) {
    Eigen::MatrixXd inf = inf_calclator->calc_information_matrix(key1->cloud, key2->cloud, relative_pose);
    return add_se3_edge(key1->node, key2->node, relative_pose, inf);
  }

  void InteractiveGraph::optimize() {
    auto t1 = std::chrono::high_resolution_clock::now();

    chi2_before = graph->chi2();
    iterations = GraphSLAM::optimize(params.param<int>("g2o_solver_num_iterations", 64));
    chi2_after = graph->chi2();

    auto t2 = std::chrono::high_resolution_clock::now();
    elapsed_time_msec = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
  }
}