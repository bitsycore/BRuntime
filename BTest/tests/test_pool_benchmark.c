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
	double totalElapsed = 0.0;
	TITLE("Autorelease Pool Benchmark");

	// =========================================================
	// Benchmark 1: Basic Push/Pop Performance
	// =========================================================
	TEST("Push/Pop Performance");
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPop();
	}
	end = clock();
	elapsed = GetTimeMicroseconds(start, end);
	totalElapsed += elapsed;
	log_fmt("    %d push/pop pairs: %.2f μs (%.2f ns per operation)\n",
		BENCHMARK_ITERATIONS, elapsed, (elapsed * 1000.0) / BENCHMARK_ITERATIONS);

	// =========================================================
	// Benchmark 2: Pool Reuse Efficiency
	// =========================================================
	TEST("Pool Reuse Efficiency (5 nested levels)");
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPop();
		BFAutoreleasePoolPop();
		BFAutoreleasePoolPop();
		BFAutoreleasePoolPop();
		BFAutoreleasePoolPop();
	}
	end = clock();
	elapsed = GetTimeMicroseconds(start, end);
	totalElapsed += elapsed;
	log_fmt("    %d iterations (5 levels each): %.2f μs (%.2f ns per push/pop)\n",
		BENCHMARK_ITERATIONS, elapsed, (elapsed * 1000.0) / (BENCHMARK_ITERATIONS * 10));

	// =========================================================
	// Benchmark 3: Autorelease Performance
	// =========================================================
	TEST("Autorelease 10 Objects per Pool");
	BFAutoreleasePoolPush();
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
		BFAutoreleasePoolPush();
		for (int j = 0; j < 10; j++) {
			BFAutorelease($OBJ BO_NumberCreateInt32(j));
		}
		BFAutoreleasePoolPop();
	}
	end = clock();
	BFAutoreleasePoolPop();
	elapsed = GetTimeMicroseconds(start, end);
	totalElapsed += elapsed;
	log_fmt("    %d pools × 10 objects: %.2f μs (%.2f ns per autorelease)\n",
		BENCHMARK_ITERATIONS, elapsed, (elapsed * 1000.0) / (BENCHMARK_ITERATIONS * 10));

	// =========================================================
	// Benchmark 4: High Object Count per Pool
	// =========================================================
	TEST("Autorelease 100 Objects per Pool");
	BFAutoreleasePoolPush();
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS / 10; i++) {
		BFAutoreleasePoolPush();
		for (int j = 0; j < 100; j++) {
			BFAutorelease($OBJ BO_NumberCreateInt32(j));
		}
		BFAutoreleasePoolPop();
	}
	end = clock();
	BFAutoreleasePoolPop();
	elapsed = GetTimeMicroseconds(start, end);
	totalElapsed += elapsed;
	log_fmt("    %d pools × 100 objects: %.2f μs (%.2f ns per autorelease)\n",
		BENCHMARK_ITERATIONS / 10, elapsed, (elapsed * 1000.0) / ((BENCHMARK_ITERATIONS / 10) * 100));

	// =========================================================
	// Benchmark 5: Overflow Chain Performance
	// =========================================================
	TEST("Pool Overflow (200 objects, 2× capacity)");
	BFAutoreleasePoolPush();
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS / 100; i++) {
		BFAutoreleasePoolPush();
		for (int j = 0; j < 200; j++) {  // Force overflow chain creation
			BFAutorelease($OBJ BO_NumberCreateInt32(j));
		}
		BFAutoreleasePoolPop();
	}
	end = clock();
	BFAutoreleasePoolPop();
	elapsed = GetTimeMicroseconds(start, end);
	totalElapsed += elapsed;
	log_fmt("    %d pools × 200 objects: %.2f μs (%.2f ns per autorelease)\n",
		BENCHMARK_ITERATIONS / 100, elapsed, (elapsed * 1000.0) / ((BENCHMARK_ITERATIONS / 100) * 200));

	// =========================================================
	// Benchmark 6: Mixed Workload (Realistic Usage Pattern)
	// =========================================================
	TEST("Mixed Workload (varying object counts)");
	BFAutoreleasePoolPush();
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS / 10; i++) {
		// Small pool
		BFAutoreleasePoolPush();
		for (int j = 0; j < 5; j++) {
			BFAutorelease($OBJ BO_NumberCreateInt32(j));
		}
		BFAutoreleasePoolPop();

		// Medium pool
		BFAutoreleasePoolPush();
		for (int j = 0; j < 50; j++) {
			BFAutorelease($OBJ BO_NumberCreateInt32(j));
		}
		BFAutoreleasePoolPop();

		// Large pool
		BFAutoreleasePoolPush();
		for (int j = 0; j < 150; j++) {
			BFAutorelease($OBJ BO_NumberCreateInt32(j));
		}
		BFAutoreleasePoolPop();
	}
	end = clock();
	BFAutoreleasePoolPop();
	elapsed = GetTimeMicroseconds(start, end);
	totalElapsed += elapsed;
	log_fmt("    %d iterations (mixed sizes): %.2f μs total\n",
		BENCHMARK_ITERATIONS / 10, elapsed);

	// =========================================================
	// Benchmark 7: String Creation (Common Real-World Usage)
	// =========================================================
	TEST("String Creation with Autorelease");
	BFAutoreleasePoolPush();
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS / 10; i++) {
		BFAutoreleasePoolPush();
		for (int j = 0; j < 20; j++) {
			BFAutorelease($OBJ BO_StringCreate("Test string %d", j));
		}
		BFAutoreleasePoolPop();
	}
	end = clock();
	BFAutoreleasePoolPop();
	elapsed = GetTimeMicroseconds(start, end);
	totalElapsed += elapsed;
	log_fmt("    %d pools × 20 strings: %.2f μs\n",
		BENCHMARK_ITERATIONS / 10, elapsed);

	// =========================================================
	// Benchmark 8: Stress Test - Deep Nesting
	// =========================================================
	TEST("Deep Nesting Stress Test (20 levels)");
	start = clock();
	for (int iter = 0; iter < BENCHMARK_ITERATIONS / 100; iter++) {
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();

		for (int i = 0; i < 10; i++) {
			BFAutorelease($OBJ BO_NumberCreateInt32(i));
		}

		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();
		BFAutoreleasePoolPush();

		for (int i = 0; i < 10; i++) {
			BFAutorelease($OBJ BO_NumberCreateInt32(i));
		}

		// Pop all 20 levels
		for (int i = 0; i < 20; i++) {
			BFAutoreleasePoolPop();
		}
	}
	end = clock();
	elapsed = GetTimeMicroseconds(start, end);
	totalElapsed += elapsed;
	log_fmt("    %d iterations (20 levels): %.2f μs\n",
		BENCHMARK_ITERATIONS / 100, elapsed);

	log_fmt("\n" BC_AE_GREEN "✓ Benchmark Complete" BC_AE_RESET "\n");
	log_fmt("Total elapsed time: %.2f ms\n", totalElapsed / 1000.0);
}
