#include <CGRA/Mapper.h>

#include <future>

struct BothSmallTimeThenHeurFullTime : Mapper {
	static AutoRegisterMapper arm;
	static constexpr const char* mapper_name = "BothSmallTimeThenHeurFullTime";

	ConfigStore untouched_args; //< passed through to the mappers
	ConfigStore args; //< local use

	/**
	 * Constructor called from registrar
	 */
	BothSmallTimeThenHeurFullTime(std::shared_ptr<CGRA> cgra, int timelimit, const ConfigStore& args)
		: BothSmallTimeThenHeurFullTime(cgra, timelimit, args, getEntriesForPrefix(args, mapper_name + std::string(".")))
	{ }

	/**
	 * Should be called with pre-processed @p args
	 */
	BothSmallTimeThenHeurFullTime(std::shared_ptr<CGRA> cgra, int timelimit, ConfigStore untouched_args, ConfigStore args)
		: Mapper(cgra, timelimit)
		, untouched_args(std::move(untouched_args))
		, args(std::move(args))
	{ }

	template<typename T>
	void printStatus(const T& msg) const {
		if (args.getInt("verbosity") > 0) {
			std::cout << mapper_name << "mapper: " << msg << '\n';
		}
	}

	Mapping mapOpGraph(std::shared_ptr<OpGraph> opgraph, int II, const MRRG& mrrg, std::unordered_map<std::string, std::string> fix_port) override {
		return mapOpGraph_internal(opgraph, II, mrrg, fix_port);
	}

	Mapping mapOpGraph_internal(std::shared_ptr<OpGraph> opgraph, int II, const MRRG& mrrg, std::unordered_map<std::string, std::string> fix_port) const {
		// The idea is to simulate being able to stop one mapper or the other before it takes too long.
		// Unfortunately, there is currently no support for some sort of stop token, (async or otherwise)
		// so first run them with a short time limit, then the full one.
		// TODO: share caches between HeurILP runs

		struct Times {
			long heur, ilp;
			long heur_wait;
		};

		double last_solve_time = -1;

		int ilp_mapper_seed = args.getInt("seed");

		const auto advance_ilp_mapper_seed = [&] { ilp_mapper_seed += 1; };

		for (const auto& timeouts : {
			Times{std::min<long>(args.getInt("initial_timeout"),timelimit), std::min<long>(args.getInt("initial_timeout"),timelimit), args.getInt("initial_heur_wait")},
			Times{timelimit,                                                std::min<long>(args.getInt("secondary_limit"),timelimit), 0L},
		}) {
			const auto ilp_mapper_computed_options = [&] {
				ConfigStore result;
				result.setInt("ILPMapper.seed", ilp_mapper_seed);
				return result;
			};

			const auto heur_mapper_computed_options = [&] {
				ConfigStore result;
				return result;
			};

			printStatus("Staring the ILPMapper");
			auto ilp_future = std::async(std::launch::async, [&]() {
				auto ilp_mapper = AutoRegisterMapper::getDefaultRegistry().createMapper(
					"ILPMapper", cgra, timeouts.ilp, with_set(untouched_args, ilp_mapper_computed_options()));
				return ilp_mapper->mapOpGraph(opgraph, II, mrrg, fix_port);
			});

			if (args.getInt("max_threads") < 2) { ilp_future.wait(); }

			if (timeouts.heur_wait > 0 && ilp_future.wait_for(std::chrono::seconds(timeouts.heur_wait)) == std::future_status::ready) {
				printStatus("ILPMapper completed quickly!");
				auto ilp_mapping = ilp_future.get();
				switch (analyzeILPMapping(ilp_mapping, mrrg)) {
					case ILPMapperAnalysisResult::return_mapping: printStatus("Using ILPMapper result"); return ilp_mapping;
					case ILPMapperAnalysisResult::advance_seed: printStatus("ILPMapper mapping unusable"); advance_ilp_mapper_seed(); break;
					default: [[fallthrough]];
					case ILPMapperAnalysisResult::ignore_mapping: printStatus("ILPMapper failed/mapping not used"); break;
				}
			}

			printStatus("Starting the ILPHeuristicMapper");
			auto heur_future = std::async(std::launch::async, [&]() {
				auto heur_mapper = AutoRegisterMapper::getDefaultRegistry().createMapper(
					"ILPHeuristicMapper", cgra, timeouts.heur - timeouts.heur_wait, with_set(untouched_args, heur_mapper_computed_options()));
				return heur_mapper->mapOpGraph(opgraph, II, mrrg, fix_port);
			});

			auto heur_mapping = heur_future.get();
			if (heur_mapping.isMapped()) {
				printStatus("Using successful ILPHeuristicMapper mapping");
				return heur_mapping;
			}

			// possibly examine the ILPMapper result
			if (ilp_future.valid()) { // only if the future is not consumed
				auto ilp_mapping = ilp_future.get();
				switch (analyzeILPMapping(ilp_mapping, mrrg)) {
					case ILPMapperAnalysisResult::return_mapping: printStatus("Using ILPMapper result"); return ilp_mapping;
					case ILPMapperAnalysisResult::advance_seed: printStatus("ILPMapper mapping unusable"); advance_ilp_mapper_seed(); break;
					default: [[fallthrough]];
					case ILPMapperAnalysisResult::ignore_mapping: printStatus("ILPMapper failed/mapping not used"); break;
				}
			}

			if (not heur_mapping.isTimeout()) {
				printStatus("Everything returned failure without timing out, giving up");
				return heur_mapping;
			}

			last_solve_time = heur_mapping.solve_time_in_seconds;
		}

		printStatus("Schedule ended, returning failure");
		auto failure_result = Mapping(cgra, II, opgraph);
		failure_result.solve_time_in_seconds = last_solve_time;
		return failure_result;
	}

	enum class ILPMapperAnalysisResult {
		return_mapping,
		advance_seed,
		ignore_mapping,
	};

	static ILPMapperAnalysisResult analyzeILPMapping(const Mapping& ilp_mapping, const MRRG& mrrg) {
		switch (ilp_mapping.getStatus()) {
			case MappingStatus::success:
				if (mappingIsOK(ilp_mapping, mrrg)) {
					return ILPMapperAnalysisResult::return_mapping;
				} else {
					return ILPMapperAnalysisResult::advance_seed; // try to get lucky
				}
			case MappingStatus::failure:
				return ILPMapperAnalysisResult::return_mapping; // return failure,  must be impossible to map
			default: [[fallthrough]];
			case MappingStatus::timeout:
				return ILPMapperAnalysisResult::ignore_mapping;
		}
	}

	/**
	 * @brief Checks latency balancing. Only needed for the ILP mapper
	 */
	static bool mappingIsOK(const Mapping& mapping, const MRRG& mrrg) {
		const auto create_mapping_graph_result = createMappingGraph(mrrg, mapping);
		const auto final_mapping_graph = removeIsolatedRoutingNodes(create_mapping_graph_result.mg, mrrg, create_mapping_graph_result.toMRRG);
		const auto latency_check_result = latencyCheck(final_mapping_graph, mrrg, create_mapping_graph_result.toMRRG);
		return latency_check_result.first;
	}
};

AutoRegisterMapper BothSmallTimeThenHeurFullTime::arm  = AutoRegisterMapper(
	BothSmallTimeThenHeurFullTime::mapper_name,
	[](std::shared_ptr<CGRA> cgra, int timelimit, const ConfigStore& args) {
		return std::make_unique<BothSmallTimeThenHeurFullTime>(cgra, timelimit, args);
	},
	true, // is a composite
	"In parallel, runs both ILPHeuristicMapper and ILPMapper with a short timeout, "
	"then ILPHeuristicMapper with the full timeout while running ILPMapper with a capped timeout. "
	"Any successful & correct mapping is returned",
	{ // required args
		{"seed", 0, ""},
		{"verbosity", 0, ""},
		{"max_threads", 2, ""},
		{"initial_timeout", 25, "Max time of the first iteration"},
		{"initial_heur_wait", 10,
			"The first run of ILPHeuristicMapper will be launched this many seconds after ILPMapper\n"
			"The point is to try to avoid the cache warm-up if possible, as that takes a while, and isn't included in the timeout."
			"(cache warm-up can easily take tens of seconds)"
		},
		{"secondary_limit", 3*60, "Time limit applied to second run of ILPMapper"},
	},
	{ // optional arg regexes
	}
);
