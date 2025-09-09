.. _multicontext_top:

=============================
Multi-Context Support in CGRA
=============================

Multi-context support allows time-multiplexing the CGRA. In multi-context, multiple configuration
bitstreams are loaded into a CGRA instead of just one.  The CGRA cycles through the bitstreams according
to a predefined pattern. Multi-context thus allows a small CGRA to realize a large application that could
not otherwise "fit", by trading off space for time.
A CGRA can support certain number of contexts (supported initiation intervals - SII) and the CGRA can
operate in any number of contexts 1<=II<=SII (where II is initiation interval).
Multi-context is implemented here using binary counters that drives multiplexer select inputs to select
configuration bits based on the current context. Identical binary counters are located in each core CGRA
tile: processing elements (PE), register file (RF), I/O, and memory port. Each of these counters is
ceil(log2(SII)) bits wide. The counters count in tandem: each counter in each core CGRA tile is outputting
the same code in a given cycle.  If SII=4 and II=4 (all contexts are used), then the counters count through
the sequence: 00, 01, 10, 11, 00, ... .  The counters are also capable of working in a ``modulo'' style if
II<SII, where, if II, the counter would count through the sequence: 00, 01, 10, 00, ... .  An extra register
is incorporated into the CGRA top module that indicates the number of used contexts (II).  The output of this
register is fed to one-hot counters throughout the CGRA fabric, so they cycle through only those one-hot codes
consistent with the number of used contexts. The one-hot codes produced by counters drive AND/OR logic to
select the configuration bit for the current context.


Generating Verilog
------------------

To generate verilog for the multi-context CGRA architecture, the only additional parameter to be provided is SII.
So the following command can be used to generate the verilog for multi-contect CGRA:

$ ``"$CGRA_MAPPER" --SII 4 -c adres --arch-opts "rows=2 cols=2" --gen-verilog .``

The above command generated the CGRA verilog file with the filename cgrame.v

Generating Configuration Bitstream
----------------------------------

To generate configuration bitstream for the multi-context CGRA architecture, the SII and the initiation interval
to be used (II) needs to be provided as parameters.
The following command generates the configuration bitstream for multi-contect CGRA:

$ ``"$CGRA_MAPPER" --SII 4 --II 3 -g graph_loop.dot -c adres --arch-opts "rows=2 cols=2" --gen-testbench .``

A verilog file named testbench.v is generated with the configuration bitstream for mapping the application onto
the multi-context CGRA.

ModelSim Simulation
-------------------

For ModelSim simulation using the tool generated CGRA and configuration bitstream verilog files, a wrapper verilog
file is also additionally required which instantiates both the CGRA verilog and configures it using the
configuration bitstream verilog file (testbench.v). Examples of such wrapper verilog files can be found in the
verilog-verif folder (verilog-verif/adres2x2-sum/multi-context/tb_src/tb_master.sv)