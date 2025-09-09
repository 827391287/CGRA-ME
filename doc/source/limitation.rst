.. _limitation-label:

***********
Limitations
***********

There are some limitation to the software, which will generally vary between versions.

.. _dfg-generation-limitation-label:

DFG Limitations
===============

#. You can only have one basic block within the loop body.
   Any branching or loops that remain after optimization will result in generation errors.

#. Similar to the above, any function calls that remain after optimization are not supported and will result in generation errors.

#. The latency of back edges is assumed to be one, and latency of all froward edges is unconstrained.
   This can result in incorrect mappings if loop-carried dependencies from earlier than the previous iteration exist.

#. Initial values of variables (other than constants) are not preserved, and assumed to be 0.
   Also, all registers are initialized to 0.
   For DFGs where this matters, output from simulations of the CGRA Verilog will be incorrect.

#. DFGs that contain control will have the control portions stripped out, and only the datapath will be mapped.

Mapper Limitations
==================

Capabilities and correctness depend on which mapper is being used.

BothSmallTimeThenHeurFullTime
-----------------------------

(this is the default mapper)

#. May not find a mapping -- it may rely on `ILPHeuristicMapper` for a correct mapping.

ILPHeuristicMapper
------------------

#. May not find a mapping -- it relies on certain heuristics to improve runtime significantly.

ILPMapper
---------

#. Often takes a very long time.
#. By default, it does not constrain latency to be balanced among re-convergent paths in the DFG. Adding this makes it even slower.
   CGRA-ME will validate that re-convergent paths in the mapping have equal latency, and that all cycles have latency of at least one.


------------

#. Not tested as well as other mappers, and currently has trouble mapping non-trivial DFGs
#. Needs a slower schedule for DFGs that have back-edges.

