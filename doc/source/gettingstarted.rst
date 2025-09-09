.. _gettingstarted_top:

===============
Getting Started
===============

Requirements
------------

====================    ================   ====
Package                 Version            Note
====================    ================   ====
**gcc**                 >=4.9.0            If you want to build llvm with gcc too, please follow llvm's `requirement <http://llvm.org/docs/GettingStarted.html#software>`_ for gcc.
**clang**               >=3.8.0            Tested with various versions up to v10.0.0. Be sure to install the 'development' package.
**llvm**                >=3.8.0            Tested with various versions up to v10.0.0. Be sure to install the 'development' package.
**cmake**               >=3.1.0
**Python**              >=3.5.2
**Gurobi Optimizer**    >=7.0.0            Free with `academic license or limited time trial <https://www.gurobi.com/free-trial>`_.
====================    ================   ====

In general, we ensure the project will function on Ubuntu 16.04 and a recently updated Arch Linux installation.
Other Linux distributions should generally work.
The code builds but has some runtime issues on macos.
Other operating systems have not been tested.

.. _installation-label:

Installation & Setup
--------------------

#. Create a directory for CGRA-ME related activities, we'll call it ``cgrame_project`` in this guide.
#. Go to `our download page <http://cgra-me.ece.utoronto.ca/download/>`_, accept the agreement, and get re-directed to our repository hosting.
#. Make an account there and clone the ``UofT/CGRA-ME Releases`` repository into a subdirectory of ``cgrame_project``.
#. If you already have an istallation of Gurobi, ensure that the enviornment variable ``GUROBI_HOME`` is set. Otherwise, follow the steps below:
    #. `Register and request a Gurobi licence <https://www.gurobi.com/free-trial>`_, also `download the Gurobi library <https://www.gurobi.com/downloads/>`_
    #. Extract Gurobi library package inside of ``cgrame_project``
    #. In bin subdirectory of Gurobi directory run ``./grbgetkey <key-you-obtained>`` to activate your Gurobi
#. Build the code: $ ``cd cgrame_project\cgra-me-release && make --jobs $(nproc)``

See `Troubleshooting`_ if you encounter any problems.

.. NOTE::
    * If you are building llvm from source, make sure you put the build or install directory in your ``$PATH`` environment variable.
    * There is a `known <https://bugs.llvm.org/show_bug.cgi?id=23352>`_ `problem <https://bugs.launchpad.net/ubuntu/+source/llvm/+bug/1387011>`_
      with the llvm-dev packages on many Ubuntu versions, where CMake fails to find parts of LLVM.
      Building from source will work, or you can manually change some files in /usr. See `Troubleshooting`_

Environment Setup
-----------------

There is a script ``cgra-me-release/cgrame_env`` that when *run* (not intended to be ``source``'d) will
drop you into another instance of your ``$SHELL`` with
several convenient and essential environment variables set, including:

* ``PATH`` will be augmented so that the most important
  CGRA-ME binaries and scripts can be invoked with out explicit paths.
* ``CGRA_MAPPER`` will have the name of the mapper executable,
  with the expectation that it will be resolved against the above modified path.
* Several other variables related to file locations.

Running Benchmarks
------------------

With the above installation and environment setup complete,
you can now try out the software on some of the included benchmarks and architectures (covered here)
and then move on to making your own (covered elsewhere).

Bulk Benchmark Runs
^^^^^^^^^^^^^^^^^^^

The script ``cgra-me-release/benchmarks/experiment_runner`` was created to make this fairly easy.
It is decently configurable and may be the simplest way to run experiments when using a single computer.
Configuration is done via a python file (specified by ``-f``, though it has a default) and
\    it can run experiments in parallel (``-j`` option).
It can also be killed at any time (eg. control+c) and when restarted,
it will re-run any experiments that did not finish.
For more documentation, and specifically documentation about the configuration input,
\    we direct you to run it with the ``--help`` option.

There is a configuration file example included as a starting point,
so if you just want to run the software, simply:

1. $ ``cd cgra-me-release/benchmarks``
2. $ ``./experiment_runner -j3``

.. NOTE::
    This will take at least several minutes to finish, especially if parallelism is not enabled

Individual Benchmark Running
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``experiment_runner`` script is (of course) capable of running exactly one experiment,
but it is useful to know some details if you plan on making your own,
or want to get to know the software better.

Generate A DFG
""""""""""""""

The first step is to generate a DFG (Data Flow Graph).
A DFG is a representation of a computation kernel that captures the logical movement of data from inputs though operations to outputs.
We'll use one of the included benchmarks as an example.

1. $ ``cd cgra-me-release/benchmarks/microbench/sum``
2. $ ``make clean    # start from scratch``
3. $ ``ls            # see what files are present before``
4. $ ``make          # compile with clang, optimize, and emit the DFG``
5. $ ``ls            # note graph_loop.dot was created``

This will generate several files, the most important of which is ``graph_loop.dot`` which holds the DFG
that resulted from the processing of the loop in the C file.
The format used is the classic 'dot' format and so can be viewed with several programs, for example ``xdot``.
More details about how this step works can be found at :ref:`dfg-generation-label`.

.. NOTE::
    Due to commonly occurring difficulties installing and using LLVM's development packages,
    we include pre-generated DFG files for most benchmarks.
    They can be found within the individual benchmark directories as ``pre-gen-*.dot``.
    When ``experiment_runner`` tries to build a benchmark from C code, the build will detect if our LLVM plugin was built,
    \    and if not, will automatically use the pre-generated DFG.
    This means that the benchmark will not reflect any changes to the C source.
    A warning message is printed when building the benchmark this way.


Mapping
"""""""

This is the process of taking a DFG and a CGRA architecture and
finally mapping the computation kernel onto the CGRA.
We will use one of the included architectures for demonstration.
After building the benchmark, now run this:

  $ ``"$CGRA_MAPPER" --II 2 -g graph_loop.dot --xml "$CGRA_ME_ROOTDIR/arch/simple/archfiles/adres-no-torroid.xml"``

If you looked into the ``experiment_runner``'s help message, or investigate the Makefiles, you may wish to know that an equivalent way to run this would be to do:

  $ ``make run_mapper "CGRA_ARCH_ARGS=--xml $CGRA_ME_ROOTDIR/arch/simple/archfiles/adres-no-torroid.xml" 'CGRA_MAPPER_ARGS=--II 2'``

Now if everything is setup correctly, you should see the mapper running, and it should complete in less than a minute.
We are mapping a benchmark that does sum over an array,
we are using an architecture similar to ADRES,
the architecture is using our default parameters of a four-by-four functional unit array,
and we are specifying that there are two contexts to context-switch between.

The current default mapper is a hybrid of an exact and heuristic solver, both integer linear program-based.
It will use up to two threads to concurrently attempt the exact and heuristic solve under iteratively increasing time-outs,
\   with the goal of minimizing overall runtime.
Either of these component mappers can be specifically chosen via the ``--mapper`` option,
\   and all mappers can be configured via the ``--mapper-opts`` option.
Run ``cgrame --help`` for more details.

Visualization
-------------

If the ``--gen-visual`` option is specified, the ``$CGRA_ME_ROOTDIR/output`` directory is created.
Opening ``CGRA.html`` inside will show a visualization of all the MRRG nodes within the CGRA,
\  as well as just the nodes used by the mapping.
The top navigation bar has a tab for each context, and one for the mapping.
Yellow nodes represent unused resources.
Teal nodes represent inputs or outputs of cross-context elements such as registers.
Blue nodes are composite nodes in the hierarchy.
Double-clicking on these nodes descends into the hierarchy.
Selecting and right-clicking on any node within the expanded hierarchy will collapse the node back into the composite node.
Red nodes represent used resources. Hovering over these nodes will inform the user within a tool-tip the part of the mapped DFG used that resource.

An attempt has been made to position the MRRG nodes in a grid-like manner for the C++ architectures,
\  with the exception of the Clustered architecture (WIP).
The positions differ depending on architecture,
but in general the IO and mem units lie on the perimeter of the visualization and surround the PE units.
Each cluster of nodes is arranged relative to their parent,
which will be located at the top left corner of the area reserved for the cluster.
Some caveats of the visualization:

 * The time needed for the visualization to load increases with the number of contexts being visualization. One should expect that it may take a few minutes to load.
 * Occasionally there edges that appear to go nowhere. These edges actually go to places where a node would be but it has been clustered into another node.
   This is a bug with the visualization framework.

Conclusion
----------

This is only a getting started guide to CGRA-ME, there are many other features, you can read the :ref:`userguide-label` if you want to know more about them.

.. _install-troubleshoot-label:

Troubleshooting
---------------

I have cmake error about cannot find llvm package
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* Using apt-get: to fix the `known <https://bugs.llvm.org/show_bug.cgi?id=23352>`_ `problem <https://bugs.launchpad.net/ubuntu/+source/llvm/+bug/1387011>`_,
  we have provided some ``.cmake`` files in ``cgra-me-release/doc/ubuntu-llvm-fix``. **Intended for 16.04.**
  Copy these files to to ``/usr/share/llvm-3.8/cmake`` and make this symbolic link:

    ``ln -s /usr/lib/x86_64-linux-gnu/libLLVM-3.8.so.1 /usr/lib/llvm-3.8/lib/libLLVM-3.8.so.1``.

  Inspired by `a stack overflow answer <https://stackoverflow.com/a/44311146/2256231>`_.
* Using apt-get: the package manager could mess up stuff sometimes, please check if you have any of the following directory in you ``$PATH``: ``/usr/`` or ``/usr/bin/`` or ``/usr/sbin/``.
  If you don't see any of the above add ``/usr/`` to your ``$PATH``, then try build again
* Build from llvm source: try to add ``<LLVM_DIR>/build/`` to your ``$PATH``, then try build again

.. NOTE::
    If the above did not help you, it might be that you have a existing llvm installed and it is tricking the package searcher from cmake. Reference the user guide and run cmake with the argument ``-DCMAKE_PREFIX_PATH=<path/to/your/llvm>`` to override the default llvm cmake file.

Send us an e-mail if you have any other problem!
