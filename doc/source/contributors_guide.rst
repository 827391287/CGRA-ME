.. _contributors_guide_top:

===================
Contributor's Guide
===================

*This guide is for those who want to understand the general organization of the code, and possibly contribute.
See the* :ref:`gettingstarted_top` *guide for how to get the code building.*

Framework Overview
==================

.. image:: _static/CGRA-ME-framework-chart.png
    :alt: flow chart showing components of CGRA-ME
    :align: center

Some correspondences to source files & data structures:
 - [1] *Benchmark* corresponds to a C file in a subdirectory of ``benchmarks``.
 - [2] *CGRA Architecture Description* corresponds to a XML file in ``architectures``.
 - [3] *CGRA Architecture Interpreter* is implemented by either ``src/adl_0`` or ``src/adl_1``, depending on the command line argument (1 is the default).
    - Additionally, the modules that are instantiated by the XML correspond to subclasses of ``Module`` in ``src/core/Module{,RoutingStructures}.cpp``
 - [4] *Device Model of Architecture* is set of ``Modules`` connected together to each other by ports, and in a tree stored in ``class CGRA``.
    - Can also be constructed directly by the C++ API, see ``inc/CGRA/user-inc/UserArch.h`` for how to register a generator.
 - [5] *LLVM* is the C-to-DFG pass, source is in ``llvm-passes/DFG``.
    - produces a file in the DOT format that describes the DFG, read into main ``cgrame`` executable as a ``class OpGraph``.
 - [6] *Mapper* is one of three mappers: (Xander's) ``ILPMapper``,  or (Matthew's) ``ILPHeuristicMapper``, depending on command-line arguments
    - takes a ``CGRA`` and a ``OpGraph`` and produces a ``class Mapping``.
    - At map time, device model (``CGRA``) is converted to ``class MRRG``. Each module produces a fragment and they're stitched together by the connections between ports.
    - Mappers are in ``core/{ILPMapper,HeuristicMapper}.cpp``
 - [7] *Verilog RTL for CGRA* is the Verilog produced by calling ``CGRA::genVerilog``. Uses CoreIR based approach by default.
 - [8] *Verilog Simulation* can be done with the "configurator" module produced by calling ``printTestbench`` on ``CGRA::genBitStream``. See ``verilog-verif`` folder for an automated approach.
 - [9] & [A] are not directly part of the repository, and done by feeding the generated Verilog into the relevant ASIC or FPGA tool.
 - [B] & [C] are files in ``performance_models``.
 - [D] *Performance/Power/Area Estimation engine* is implemented by ``src/perfengine``.

.. NOTE::
	If you want to understand the flows, a good place to start reading is ``cgrame``'s ``main`` function in ``src/run/cgrame.cpp``.

Repository Layout
=================

A freshly cloned repository contains the following directories:
 * ``arch`` -- XML architecture description language (ADL) files
    * ``simple/*.xml`` -- ADL v0 architectures
    * ``simple/archfiles/*.xml`` -- ADL v1 architectures
 * ``benchmarks`` -- benchmark C code and Makefiles to build them
 * ``cmake`` -- some files usid by the CMake when the code is built
 * ``doc`` -- documentation
 * ``inc`` -- header files that are part of the API
 * ``performance_models`` -- models/configuration for the performance engine
 * ``src`` -- source files and internal header files (`src details`_)
 * ``thirdparty`` -- any code that we didn't write (`thirdparty details`_)
 * ``verilog-verif`` -- files for automatic generation and testing of generated Verilog implementations

After building, the following will be present
 * ``builds`` -- directory for builds. Subdirectories include (depending on builds done) ``release`` and ``debug``
 * ``build`` -- symlink to the most recently built subdirectory of ``builds``

After running, the following may be present
 * ``output`` -- location of the visualization.

Src Details
-----------

 * ``adl_0`` -- the first version of the ADL
 * ``adl_1`` -- the second version of the ADL
 * ``archs`` -- C++ architecture creation code, with architecture-specific modules (eg. HyCUBE, ADRES, and their special PEs)
 * ``core`` -- common types, algorithms and the mappers
 * ``dotparser`` -- `DOT <https://www.graphviz.org/doc/info/lang.html>`_ parser, used for reading OpGraphs and mapping
 * ``llvm-passes`` -- LLVM passes, including the C-to-DFG pass
 * ``modules`` -- common modules; modules that aren't architecture specific
 * ``perfengine`` -- implementation of the performance engine
 * ``run`` -- ``main`` functions, and associated files
 * ``visual`` -- visualization and related files

Thirdparty Details
------------------

 * ``catch2`` -- the testing library, `Catch2 <https://github.com/catchorg/Catch2>`_
 * ``coreir`` -- our fork of the `CoreIR <https://github.com/rdaly525/coreir>`_ project, used for Verilog generation (submodule)
 * ``cxxopts`` -- `CXXOpts <https://github.com/jarro2783/cxxopts>`_, a command line option parser
 * ``google-or-tools`` -- `Google's operation research tools library <https://developers.google.com/optimization/>`_ (submodule)
 * ``mINI`` -- `An INI parser <https://github.com/r-lyeh-archived/mINI>`_, used to read ``mapper_config.ini``
 * ``pugixml`` -- `pugixml XML parser <https://pugixml.org/>`_, used to read the XML architectures
 * ``scipoptsuite`` -- `Solving Constraint Integer Programs <https://scip.zib.de/>`_, an ILP solver
 * ``tatum`` -- the `Tatum <https://github.com/kmurray/tatum>`_ static timing analysis library, used by the performance engine. (submodule)
 * ``vis`` -- `vis.js <https://visjs.org/>`_, used for the visualization
