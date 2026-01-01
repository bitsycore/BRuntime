#include "BT_Benchmarks.h"

#include <time.h>

#define BENCHMARK_ITERATIONS 100000

void BT_BenchmarkAutoreleasePool(void) {
	clock_t start, end;
	double elapsed;
	double totalElapsed = 0.0;
	BT_Title("Autorelease Pool Benchmark");

	// =========================================================
	// Benchmark 1: Basic Push/Pop Performance
	// =========================================================
	BT_Test("Push/Pop Performance");
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPop();
	}
	end = clock();
	elapsed = BT_GetTimeMicroseconds(start, end);
	totalElapsed += elapsed;
	BT_Print("    %d push/pop pairs: %.2f μs (%.2f ns per operation)\n",
		BENCHMARK_ITERATIONS, elapsed, (elapsed * 1000.0) / BENCHMARK_ITERATIONS);

	// =========================================================
	// Benchmark 2: Pool Reuse Efficiency
	// =========================================================
	BT_Test("Pool Reuse Efficiency (5 nested levels)");
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPop();
		BF_AutoreleasePoolPop();
		BF_AutoreleasePoolPop();
		BF_AutoreleasePoolPop();
		BF_AutoreleasePoolPop();
	}
	end = clock();
	elapsed = BT_GetTimeMicroseconds(start, end);
	totalElapsed += elapsed;
	BT_Print("    %d iterations (5 levels each): %.2f μs (%.2f ns per push/pop)\n",
		BENCHMARK_ITERATIONS, elapsed, (elapsed * 1000.0) / (BENCHMARK_ITERATIONS * 10));

	// =========================================================
	// Benchmark 3: Autorelease Performance
	// =========================================================
	BT_Test("Autorelease 10 Objects per Pool");
	BF_AutoreleasePoolPush();
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
		BF_AutoreleasePoolPush();
		for (int j = 0; j < 10; j++) {
			BF_Autorelease($OBJ BO_NumberCreateInt32(j));
		}
		BF_AutoreleasePoolPop();
	}
	end = clock();
	BF_AutoreleasePoolPop();
	elapsed = BT_GetTimeMicroseconds(start, end);
	totalElapsed += elapsed;
	BT_Print("    %d pools × 10 objects: %.2f μs (%.2f ns per autorelease)\n",
		BENCHMARK_ITERATIONS, elapsed, (elapsed * 1000.0) / (BENCHMARK_ITERATIONS * 10));

	// =========================================================
	// Benchmark 4: High Object Count per Pool
	// =========================================================
	BT_Test("Autorelease 100 Objects per Pool");
	BF_AutoreleasePoolPush();
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS / 10; i++) {
		BF_AutoreleasePoolPush();
		for (int j = 0; j < 100; j++) {
			BF_Autorelease($OBJ BO_NumberCreateInt32(j));
		}
		BF_AutoreleasePoolPop();
	}
	end = clock();
	BF_AutoreleasePoolPop();
	elapsed = BT_GetTimeMicroseconds(start, end);
	totalElapsed += elapsed;
	BT_Print("    %d pools × 100 objects: %.2f μs (%.2f ns per autorelease)\n",
		BENCHMARK_ITERATIONS / 10, elapsed, (elapsed * 1000.0) / ((BENCHMARK_ITERATIONS / 10) * 100));

	// =========================================================
	// Benchmark 5: Overflow Chain Performance
	// =========================================================
	BT_Test("Pool Overflow (200 objects, 2× capacity)");
	BF_AutoreleasePoolPush();
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS / 100; i++) {
		BF_AutoreleasePoolPush();
		for (int j = 0; j < 200; j++) {  // Force overflow chain creation
			BF_Autorelease($OBJ BO_NumberCreateInt32(j));
		}
		BF_AutoreleasePoolPop();
	}
	end = clock();
	BF_AutoreleasePoolPop();
	elapsed = BT_GetTimeMicroseconds(start, end);
	totalElapsed += elapsed;
	BT_Print("    %d pools × 200 objects: %.2f μs (%.2f ns per autorelease)\n",
		BENCHMARK_ITERATIONS / 100, elapsed, (elapsed * 1000.0) / ((BENCHMARK_ITERATIONS / 100) * 200));

	// =========================================================
	// Benchmark 6: Mixed Workload (Realistic Usage Pattern)
	// =========================================================
	BT_Test("Mixed Workload (varying object counts)");
	BF_AutoreleasePoolPush();
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS / 10; i++) {
		// Small pool
		BF_AutoreleasePoolPush();
		for (int j = 0; j < 5; j++) {
			BF_Autorelease($OBJ BO_NumberCreateInt32(j));
		}
		BF_AutoreleasePoolPop();

		// Medium pool
		BF_AutoreleasePoolPush();
		for (int j = 0; j < 50; j++) {
			BF_Autorelease($OBJ BO_NumberCreateInt32(j));
		}
		BF_AutoreleasePoolPop();

		// Large pool
		BF_AutoreleasePoolPush();
		for (int j = 0; j < 150; j++) {
			BF_Autorelease($OBJ BO_NumberCreateInt32(j));
		}
		BF_AutoreleasePoolPop();
	}
	end = clock();
	BF_AutoreleasePoolPop();
	elapsed = BT_GetTimeMicroseconds(start, end);
	totalElapsed += elapsed;
	BT_Print("    %d iterations (mixed sizes): %.2f μs total\n",
		BENCHMARK_ITERATIONS / 10, elapsed);

	// =========================================================
	// Benchmark 7: String Creation (Common Real-World Usage)
	// =========================================================
	BT_Test("String Creation with Autorelease");
	BF_AutoreleasePoolPush();
	start = clock();
	for (int i = 0; i < BENCHMARK_ITERATIONS / 10; i++) {
		BF_AutoreleasePoolPush();
		for (int j = 0; j < 20; j++) {
			BF_Autorelease($OBJ BO_StringCreate("Test string %d", j));
		}
		BF_AutoreleasePoolPop();
	}
	end = clock();
	BF_AutoreleasePoolPop();
	elapsed = BT_GetTimeMicroseconds(start, end);
	totalElapsed += elapsed;
	BT_Print("    %d pools × 20 strings: %.2f μs\n",
		BENCHMARK_ITERATIONS / 10, elapsed);

	// =========================================================
	// Benchmark 8: Stress Test - Deep Nesting
	// =========================================================
	BT_Test("Deep Nesting Stress Test (20 levels)");
	start = clock();
	for (int iter = 0; iter < BENCHMARK_ITERATIONS / 100; iter++) {
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();

		for (int i = 0; i < 10; i++) {
			BF_Autorelease($OBJ BO_NumberCreateInt32(i));
		}

		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();
		BF_AutoreleasePoolPush();

		for (int i = 0; i < 10; i++) {
			BF_Autorelease($OBJ BO_NumberCreateInt32(i));
		}

		// Pop all 20 levels
		for (int i = 0; i < 20; i++) {
			BF_AutoreleasePoolPop();
		}
	}
	end = clock();
	elapsed = BT_GetTimeMicroseconds(start, end);
	totalElapsed += elapsed;
	BT_Print("    %d iterations (20 levels): %.2f μs\n",
		BENCHMARK_ITERATIONS / 100, elapsed);

	BT_Print("\n" BC_AE_GREEN "✓ Benchmark Complete" BC_AE_RESET "\n");
	BT_Print("Total elapsed time: %.2f ms\n", totalElapsed / 1000.0);
}
