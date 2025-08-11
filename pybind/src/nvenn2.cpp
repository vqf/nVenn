#include <pybind11/pybind11.h>
#include "../../topol.h"

namespace py = pybind11;

PYBIND11_MODULE(nvenn2, m) {
  py::class_<borderLine>(m, "borderLine")
    .def(py::init<const std::string &>())
    .def("simulate", &borderLine::simulate)
    .def("error_message", &borderLine::errorMsg)
    .def("error", &borderLine::err)
    .def("set_step", &borderLine::setStep)
    .def("set_cycle", &borderLine::setCycle)
    .def("is_step_finished", &borderLine::isStepFinished)
    .def("refresh", &borderLine::refresh)
    .def("rotate", &borderLine::rotateScene, py::arg("alpha"))
    .def("palette", &borderLine::loadPalette)
    .def("set_color", &borderLine::setRGBColor, py::arg("setNumber"), py::arg("red"), py::arg("green"), py::arg("blue"))
    .def("to_html", &borderLine::tohtml)
    .def("tosvg", &borderLine::tosvg);
}
