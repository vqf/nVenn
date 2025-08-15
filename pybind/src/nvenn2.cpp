#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "topol.h"

namespace py = pybind11;

PYBIND11_MODULE(nvenn2, m) {
  py::class_<borderLine>(m, "diagram")
    .def(py::init<const std::string &, unsigned int>())
    .def("simulate", &borderLine::simulate, py::arg("verbose") = false)
    .def("error_message", &borderLine::errorMsg)
    .def("error", &borderLine::err)
    .def("set_step", &borderLine::setStep)
    .def("set_cycle", &borderLine::setCycle)
    .def("is_step_finished", &borderLine::isStepFinished)
    .def("refresh", &borderLine::refresh)
    .def("rotate_diagram", &borderLine::rotateScene, py::arg("alpha"))
    .def("palette", &borderLine::loadPalette)
    .def("set_color", &borderLine::setRGBColor, py::arg("setNumber"), py::arg("red"), py::arg("green"), py::arg("blue"))
    .def("set_opacity", &borderLine::setSVGOpacity, py::arg("opacity") = 0.4)
    .def("set_line_width", &borderLine::setSVGLineWidth, py::arg("line_width") = 1)
    .def("show_region_number", &borderLine::showCircleNumbers, py::arg("show") = true)
    .def("show_region_identifier", &borderLine::showRegionNumbers, py::arg("show") = true)
    .def("set_font_size", &borderLine::setFontSize, py::arg("font_size") = 12)
    .def("get_venn_region", &borderLine::getVennRegionVectorL, py::arg("region"))
    .def("to_html", &borderLine::tohtml)
    .def("tosvg", &borderLine::tosvg);
}
