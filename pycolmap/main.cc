#include <glog/logging.h>
#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "pycolmap/estimators/bindings.cc"
#include "pycolmap/feature/sift.cc"
#include "pycolmap/geometry/bindings.cc"
#include "pycolmap/helpers.h"
#include "pycolmap/optim/bindings.cc"
#include "pycolmap/pipeline/bindings.cc"
#include "pycolmap/scene/bindings.cc"
#include "pycolmap/sfm/incremental_triangulator.cc"
#include "pycolmap/utils.h"

namespace py = pybind11;

struct Logging {
  enum class Level {
    INFO = google::GLOG_INFO,
    WARNING = google::GLOG_WARNING,
    ERROR = google::GLOG_ERROR,
    FATAL = google::GLOG_FATAL,
  };
};  // dummy class

PYBIND11_MODULE(pycolmap, m) {
  m.doc() = "COLMAP plugin";
#ifdef VERSION_INFO
  m.attr("__version__") = py::str(VERSION_INFO);
#else
  m.attr("__version__") = py::str("dev");
#endif
  m.attr("has_cuda") = IsGPU(Device::AUTO);

  auto PyLogging =
      py::class_<Logging>(m, "logging")
          .def_readwrite_static("minloglevel", &FLAGS_minloglevel)
          .def_readwrite_static("stderrthreshold", &FLAGS_stderrthreshold)
          .def_readwrite_static("log_dir", &FLAGS_log_dir)
          .def_readwrite_static("logtostderr", &FLAGS_logtostderr)
          .def_readwrite_static("alsologtostderr", &FLAGS_alsologtostderr)
          .def_static("info", [](std::string msg) { LOG(INFO) << msg; })
          .def_static("warning", [](std::string msg) { LOG(WARNING) << msg; })
          .def_static("error", [](std::string msg) { LOG(ERROR) << msg; })
          .def_static("fatal", [](std::string msg) { LOG(FATAL) << msg; });
  py::enum_<Logging::Level>(PyLogging, "Level")
      .value("INFO", Logging::Level::INFO)
      .value("WARNING", Logging::Level::WARNING)
      .value("ERROR", Logging::Level::ERROR)
      .value("FATAL", Logging::Level::FATAL)
      .export_values();
  google::InitGoogleLogging("");
  google::InstallFailureSignalHandler();
  FLAGS_logtostderr = true;

  auto PyDevice = py::enum_<Device>(m, "Device")
                      .value("auto", Device::AUTO)
                      .value("cpu", Device::CPU)
                      .value("cuda", Device::CUDA);
  AddStringToEnumConstructor(PyDevice);

  BindGeometry(m);
  BindOptim(m);
  BindScene(m);
  BindEstimators(m);
  BindIncrementalTriangulator(m);
  BindSift(m);
  BindPipeline(m);

  py::add_ostream_redirect(m, "ostream");
}