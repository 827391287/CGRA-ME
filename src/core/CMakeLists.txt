if(GUROBI_HOME)

add_library(
  core
  OBJECT
  BitSetting.cpp
  BitStream.cpp
  CGRA.cpp
  CompositeMappers.cpp
  ConstraintSet.cpp
  Control.cpp
  HeuristicMappers.cpp
  ILPMapper.cpp
  Mapper.cpp
  Mapping.cpp
  MappingProcedures.cpp
  Module.cpp
  ModuleComposites.cpp
  ModuleElastic.cpp
  ModuleFuncUnit.cpp
  ModuleFPUnit.cpp
  ModuleFracUnit.cpp
  ModuleProcedures.cpp
  ModuleRoutingStructures.cpp
  MRRG.cpp
  MRRGProcedures.cpp
  OpGraph.cpp
  OpGraphProcedures.cpp
  OpScheduler.cpp
  Scheduler.cpp
  ModulePredicationUnit.cpp
  AnnealPlacer.cpp
  Pathfinder.cpp
  TMPack.cpp
  ClusteredMapper.cpp
)
else ()
add_library(
  core
  OBJECT
  BitSetting.cpp
  BitStream.cpp
  CGRA.cpp
  CompositeMappers.cpp
  Control.cpp
  Mapper.cpp
  Mapping.cpp
  MappingProcedures.cpp
  Module.cpp
  ModuleComposites.cpp
  ModuleElastic.cpp
  ModuleFuncUnit.cpp
  ModuleFPUnit.cpp
  ModuleFracUnit.cpp
  ModuleProcedures.cpp
  ModuleRoutingStructures.cpp
  MRRG.cpp
  MRRGProcedures.cpp
  OpGraph.cpp
  OpGraphProcedures.cpp
  ModulePredicationUnit.cpp
  AnnealPlacer.cpp
  Pathfinder.cpp
  TMPack.cpp
  ClusteredMapper.cpp
  Scheduler.cpp
)
endif()


target_link_libraries_for_object_libs(
  core
  cgrame-project-wide-interface
)

set_target_properties(
    core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

