#ifndef LOOP_CLOSE_MODAL_HPP
#define LOOP_CLOSE_MODAL_HPP

#include <memory>
#include <boost/optional.hpp>

#include <imgui.h>
#include <guik/gl_canvas.hpp>
#include <hdl_graph_slam/view/keyframe_view.hpp>
#include <hdl_graph_slam/view/interactive_graph_view.hpp>

namespace hdl_graph_slam {

class LoopCloseModal {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  LoopCloseModal(InteractiveGraphView& graph, const std::string& data_directory);
  ~LoopCloseModal();

  bool set_begin_keyframe(int keyframe_id);

  bool set_end_keyframe(int keyframe_id);

  bool run();

  void draw_gl(glk::GLSLShader& shader);

  void draw_canvas();

private:
  void update_fitness_score();

  void auto_align();

  void scan_matching();

private:
  InteractiveGraphView& graph;

  double fitness_score;
  KeyFrameView::Ptr begin_keyframe;
  KeyFrameView::Ptr end_keyframe;

  Eigen::Isometry3d begin_keyframe_pose;
  Eigen::Isometry3d end_keyframe_pose_init;
  Eigen::Isometry3d end_keyframe_pose;

  std::unique_ptr<guik::GLCanvas> canvas;

  int registration_method;

  int scan_matching_method;
  float scan_matching_resolution;
};
}

#endif
