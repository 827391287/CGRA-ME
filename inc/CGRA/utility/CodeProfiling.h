#ifndef __UTILITY_TIMING_H__
#define __UTILITY_TIMING_H__

#include <CGRA/Exception.h>

#include <chrono>
#include <iostream>
#include <string>

/**
 * For taking timing measurements of a large piece of code, especially with multiple sections.
 *
 * Ex.
 *   ChronoSequence timing_seq("my sequence");
 *   std::this_thread::sleep_for(std::chrono::seconds(4));
 *   timing_seq.tick("takes 4 seconds");
 *   std::this_thread::sleep_for(std::chrono::seconds(2));
 *   timing_seq.tick("takes 2 seconds");
 *   std::cout << timing_seq << '\n';
 *
 *   // prints something like:
 *   // start of my sequence +0.000000 @0.000000
 *   //      takes 4 seconds +4.000000 @4.000000
 *   //      takes 2 seconds +2.000000 @6.000000
 */
struct ChronoSequence {
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using TimeEntry = std::pair<std::string, TimePoint>;

    struct TickResult {
        bool recorded;
        double seconds_since_last;
        double seconds_to_here;
    };

    ChronoSequence(std::string sequence_name)
        : sequence_name(std::move(sequence_name))
        , times()
    {
        recordTick(makeNowTick("start of " + this->sequence_name), nullptr);
    }

    virtual ~ChronoSequence() = default;

    auto now() const { return Clock::now(); }
    TimeEntry makeNowTick(std::string timepoint_name) const { return {std::move(timepoint_name), now()}; }

    /**
     * Record how much time it has been since the last tick under a name
     * No effect if it has been less than `threshold` seconds since the last tick.
     * Will immediately print the tick information to the given ostream, if it is not null.
     */
    TickResult tick(std::string timepoint_name, double threshold = 0.0, std::ostream* os = nullptr) {
        auto new_entry = makeNowTick(std::move(timepoint_name));
        auto result = TickResult{
            false,
            differenceAsSecondFraction(times.back().second, new_entry.second),
            differenceAsSecondFraction(times.front().second, new_entry.second),
        };
        if (threshold < result.seconds_since_last) {
            result.recorded = true;
            recordTick(new_entry, os);
        }
        return result;
    }

    static double differenceAsSecondFraction(const TimePoint& start, const TimePoint& end) {
        return std::chrono::duration<double>(end - start).count();
    }

    void printTo(std::ostream& os) const {
        if (times.size() < 2) { return; }
        std::ptrdiff_t longest_name = 0;
        for (auto& name_and_time : times) {
            longest_name = std::max(longest_name, (std::ptrdiff_t)name_and_time.first.size());
        }

        for (auto prev = times.begin(), it = prev; it != times.end(); prev = it, ++it) {
            printEntry(*it, *prev, os, longest_name);
            if (std::next(it) != times.end()) {
                os << '\n';
            }
        }
    }

    void printEntry(const TimeEntry& entry, const TimeEntry& previous, std::ostream& os, std::ptrdiff_t name_width = 0) const {
        os << std::setw(name_width) << entry.first
            << " +" << std::fixed << differenceAsSecondFraction(previous.second, entry.second)
            << " @" << std::fixed << differenceAsSecondFraction(times.front().second, entry.second);
    }

    double getTotal() { return differenceAsSecondFraction(times.front().second, times.back().second); }
    double secondsSinceStart() { return differenceAsSecondFraction(times.front().second, now()); }

    friend std::ostream& operator<<(std::ostream& os, const ChronoSequence& cs) {
        cs.printTo(os); return os;
    }

protected:
    /**
     * Unconditionally append the given tick, @p new_entry, to the list of ticks.
     */
    void recordTick(TimeEntry new_entry, std::ostream* os) {
        times.emplace_back(std::move(new_entry));
        if (os) {
            *os << "TICK: ";
            printEntry(times.back(), *std::prev(std::prev(times.end())), *os);
            *os << std::endl;
        }
    }

    std::string sequence_name;
    std::vector<TimeEntry> times;
};

/**
 * An extension of ChronoSequence that prints itself to the given ostream when this object is destroyed.
 * It creates an "end of" tick at the time of the destructor call.
 */
struct PrintOnDestructionChronoSequence : ChronoSequence {

    PrintOnDestructionChronoSequence(std::string sequence_name, std::ostream* print_into_on_destruction)
        : ChronoSequence(std::move(sequence_name))
        , print_into_on_destruction(print_into_on_destruction)
    { }

    PrintOnDestructionChronoSequence(const PrintOnDestructionChronoSequence&) = default;
    PrintOnDestructionChronoSequence& operator=(const PrintOnDestructionChronoSequence&) = default;
    PrintOnDestructionChronoSequence(PrintOnDestructionChronoSequence&) = default;
    PrintOnDestructionChronoSequence& operator=(PrintOnDestructionChronoSequence&) = default;

    ~PrintOnDestructionChronoSequence() override {
        if (not print_into_on_destruction) { return; }

        this->tick("end of " + sequence_name);
        auto& os = *print_into_on_destruction;
        try {
            printTo(os);
            os << '\n';
        } catch (...) {
            tryToSafelyPrintExceptionInfo(std::current_exception(), os, "printing PrintOnDestructionChronoSequence in it's destructor");
        }
    }

private:
    std::ostream* print_into_on_destruction;
};

#endif /* __UTILITY_TIMING_H__ */
