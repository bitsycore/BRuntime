#include "tests.h"
#include <time.h>

#define BENCHMARK_ITERATIONS 100000

// Helper to measure time in microseconds
static double GetTimeMicroseconds(clock_t start, clock_t end) {
	return ((double)(end - start) / CLOCKS_PER_SEC) * 1000000.0;
}

void benchmarkAutoreleasePool(void) {
	clock_t start, end;
	double elapsed;

	TITLE("Autorelease Pool Benchmark");

	// =========================================================
	// Benchmark 1: Basic Push/Pop Performance
	// =========================================================
	TEST("Push/Pop Performance");
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPop();
	}
	end = clock();
	elapsed = GetTimeMicroseconds(start, end);
	log_fmt("    %d push/pop pairs: %.2f μs (%.2f ns per operation)\n",
		BENCHMARK_ITERATIONS, elapsed, (elapsed * 1000.0) / BENCHMARK_ITERATIONS);

	// =========================================================
	// Benchmark 2: Pool Reuse Efficiency
	// =========================================================
	TEST("Pool Reuse Efficiency (5 nested levels)");
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPop();
		BCAutoreleasePoolPop();
		BCAutoreleasePoolPop();
		BCAutoreleasePoolPop();
		BCAutoreleasePoolPop();
	}
	end = clock();
	elapsed = GetTimeMicroseconds(start, end);
	log_fmt("    %d iterations (5 levels each): %.2f μs (%.2f ns per push/pop)\n",
		BENCHMARK_ITERATIONS, elapsed, (elapsed * 1000.0) / (BENCHMARK_ITERATIONS * 10));

	// =========================================================
	// Benchmark 3: Autorelease Performance
	// =========================================================
	TEST("Autorelease 10 Objects per Pool");
	BCAutoreleasePoolPush();
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
		BCAutoreleasePoolPush();
		for (int j = 0; j < 10; j++) {
			BCAutorelease($OBJ BCNumberCreateInt32(j));
		}
		BCAutoreleasePoolPop();
	}
	end = clock();
	BCAutoreleasePoolPop();
	elapsed = GetTimeMicroseconds(start, end);
	log_fmt("    %d pools × 10 objects: %.2f μs (%.2f ns per autorelease)\n",
		BENCHMARK_ITERATIONS, elapsed, (elapsed * 1000.0) / (BENCHMARK_ITERATIONS * 10));

	// =========================================================
	// Benchmark 4: High Object Count per Pool
	// =========================================================
	TEST("Autorelease 100 Objects per Pool");
	BCAutoreleasePoolPush();
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS / 10; i++) {
		BCAutoreleasePoolPush();
		for (int j = 0; j < 100; j++) {
			BCAutorelease($OBJ BCNumberCreateInt32(j));
		}
		BCAutoreleasePoolPop();
	}
	end = clock();
	BCAutoreleasePoolPop();
	elapsed = GetTimeMicroseconds(start, end);
	log_fmt("    %d pools × 100 objects: %.2f μs (%.2f ns per autorelease)\n",
		BENCHMARK_ITERATIONS / 10, elapsed, (elapsed * 1000.0) / ((BENCHMARK_ITERATIONS / 10) * 100));

	// =========================================================
	// Benchmark 5: Overflow Chain Performance
	// =========================================================
	TEST("Pool Overflow (200 objects, 2× capacity)");
	BCAutoreleasePoolPush();
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS / 100; i++) {
		BCAutoreleasePoolPush();
		for (int j = 0; j < 200; j++) {  // Force overflow chain creation
			BCAutorelease($OBJ BCNumberCreateInt32(j));
		}
		BCAutoreleasePoolPop();
	}
	end = clock();
	BCAutoreleasePoolPop();
	elapsed = GetTimeMicroseconds(start, end);
	log_fmt("    %d pools × 200 objects: %.2f μs (%.2f ns per autorelease)\n",
		BENCHMARK_ITERATIONS / 100, elapsed, (elapsed * 1000.0) / ((BENCHMARK_ITERATIONS / 100) * 200));

	// =========================================================
	// Benchmark 6: Mixed Workload (Realistic Usage Pattern)
	// =========================================================
	TEST("Mixed Workload (varying object counts)");
	BCAutoreleasePoolPush();
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS / 10; i++) {
		// Small pool
		BCAutoreleasePoolPush();
		for (int j = 0; j < 5; j++) {
			BCAutorelease($OBJ BCNumberCreateInt32(j));
		}
		BCAutoreleasePoolPop();

		// Medium pool
		BCAutoreleasePoolPush();
		for (int j = 0; j < 50; j++) {
			BCAutorelease($OBJ BCNumberCreateInt32(j));
		}
		BCAutoreleasePoolPop();

		// Large pool
		BCAutoreleasePoolPush();
		for (int j = 0; j < 150; j++) {
			BCAutorelease($OBJ BCNumberCreateInt32(j));
		}
		BCAutoreleasePoolPop();
	}
	end = clock();
	BCAutoreleasePoolPop();
	elapsed = GetTimeMicroseconds(start, end);
	log_fmt("    %d iterations (mixed sizes): %.2f μs total\n",
		BENCHMARK_ITERATIONS / 10, elapsed);

	// =========================================================
	// Benchmark 7: String Creation (Common Real-World Usage)
	// =========================================================
	TEST("String Creation with Autorelease");
	BCAutoreleasePoolPush();
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS / 10; i++) {
		BCAutoreleasePoolPush();
		for (int j = 0; j < 20; j++) {
			BCAutorelease($OBJ BCStringCreate("Test string %d", j));
		}
		BCAutoreleasePoolPop();
	}
	end = clock();
	BCAutoreleasePoolPop();
	elapsed = GetTimeMicroseconds(start, end);
	log_fmt("    %d pools × 20 strings: %.2f μs\n",
		BENCHMARK_ITERATIONS / 10, elapsed);

	// =========================================================
	// Benchmark 8: Stress Test - Deep Nesting
	// =========================================================
	TEST("Deep Nesting Stress Test (20 levels)");
	start = clock();
	for (int iter = 0; iter < BENCHMARK_ITERATIONS / 100; iter++) {
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();

		for (int i = 0; i < 10; i++) {
			BCAutorelease($OBJ BCNumberCreateInt32(i));
		}

		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();
		BCAutoreleasePoolPush();

		for (int i = 0; i < 10; i++) {
			BCAutorelease($OBJ BCNumberCreateInt32(i));
		}

		// Pop all 20 levels
		for (int i = 0; i < 20; i++) {
			BCAutoreleasePoolPop();
		}
	}
	end = clock();
	elapsed = GetTimeMicroseconds(start, end);
	log_fmt("    %d iterations (20 levels): %.2f μs\n",
		BENCHMARK_ITERATIONS / 100, elapsed);

	log_fmt("\n" BC_AE_GREEN "✓ Benchmark Complete" BC_AE_RESET "\n");
	log_fmt("Note: Lower times indicate better performance\n");
	log_fmt("Pool reuse should show consistent performance across iterations\n\n");
}
