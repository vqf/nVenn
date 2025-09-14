#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "topol.h"

namespace py = pybind11;


PYBIND11_MODULE(nvenn2, m) {
  py::class_<borderLine>(m, "diagram")
    .def(py::init<const std::string &, unsigned int>(), "Reads table containing a Venn diagram.\n\nArgs:\n\ttable (string): text table containing the sets\n\tbycol (int): whether the sets \
are in columns (1) or rows (2)")
    .def(py::init<const std::string &>(), "Reads table containing a Venn diagram. The algorithm will attempt to guess whether sets are in rows or columns.\n\nArgs:\n\ttable (string): \
    text table containing the sets")
    .def("simulate", &borderLine::simulate, py::arg("verbose") = false, "Generates Venn diagram figure")
    .def("error_message", &borderLine::errorMsg, "Description of error if error() is true")
    .def("error", &borderLine::err, "True if there has been an error at some point. The function error_message() returns a description of the error.")
    .def("set_step", &borderLine::setStep, "Part of the simulation() API. See Github project documentation if you want to use it.")
    .def("set_cycle", &borderLine::setCycle, "Part of the simulation() API. See Github project documentation if you want to use it.")
    .def("is_step_finished", &borderLine::isStepFinished, "Part of the simulation() API. See Github project documentation if you want to use it.")
    .def("refresh", &borderLine::refresh, "Part of the simulation() API. See Github project documentation if you want to use it.")
    .def("rotate_diagram", &borderLine::rotateScene, py::arg("alpha"), "Rotate result figure.\n\nArgs:\n\talpha (float): angle of rotation in sexagesimal degrees, counterclockwise.")
    .def("palette", &borderLine::loadPalette, "Change the color palette of the figure.\n\nArgs:\n\tpalette (int): palette number (1 to 4)")
    .def("set_color", &borderLine::setRGBColor, py::arg("setNumber"), py::arg("red"), py::arg("green"), py::arg("blue"), "Change the color of a set in the result figure. \
         \n\nArgs:\n\tsetNumber (int): set number in the same order as the input.\n\tred (float): red component (0 - 1).\n\tgreem (float): green component (0 - 1). \
         \n\tblue (float): blue component (0 - 1).")
    .def("set_opacity", &borderLine::setSVGOpacity, py::arg("opacity") = 0.4, "Opacity of the colors in the result figure.\n\nArgs:\n\topacity (float): opacity as a float between \
         0 (transparent) and 1 (opaque).")
    .def("set_line_width", &borderLine::setSVGLineWidth, py::arg("line_width") = 1, "Set the width of set lines.\n\nArgs:\n\tline_width (float): line width in svg units.")
    .def("show_region_number", &borderLine::showCircleNumbers, py::arg("show") = true, "Show or hide the labels with the number of elements in each region.")
    .def("show_region_identifier", &borderLine::showRegionNumbers, py::arg("show") = true, "Show or hide the labels with the region identifiers.")
    .def("set_font_size", &borderLine::setFontSize, py::arg("font_size") = 12, "Set font size for the labels in the figure in svg units.")
    .def("get_venn_region", &borderLine::getVennRegionVectorL, py::arg("region"), "Get a vector with the elements of a region.\n\nArgs:\n\tregion: description of \
         the region as a vector of the sets it belongs to.")
    .def("to_html", &borderLine::tohtml, "HTML code for a web page that contains the result figure. The figure can be clicked to explore the elements in each region.")
    .def("to_svg", &borderLine::tosvg, "SVG code for the result figure.")
    .def("restore_from_file", &borderLine::restoreFromFile, "Read previous result from svg file.")
    .def("write_svg", &borderLine::writeSVG, py::arg("file_name"), "Write diagram to svg file")
    .def("write_html", &borderLine::writeHTML,  py::arg("file_name"), "Write diagram to html file");
}
