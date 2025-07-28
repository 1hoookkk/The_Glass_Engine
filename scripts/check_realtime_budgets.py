#!/usr/bin/env python3
# scripts/check_realtime_budgets.py
import json
import sys

BUDGETS = {
    "48k_64": {"p99_ms": 0.25, "max_ms": 0.35},
    "48k_128": {"p99_ms": 0.50, "max_ms": 0.70},
    "48k_256": {"p99_ms": 1.00, "max_ms": 1.40}
}

def check_budgets(json_file):
    with open(json_file, 'r') as f:
        data = json.load(f)
    
    audio = data.get('audio', {})
    passed = True
    
    print("Performance Budget Check")
    print("========================")
    
    for config, budget in BUDGETS.items():
        metrics = audio.get(config, {})
        p99 = metrics.get('p99_ms', float('inf'))
        max_ms = metrics.get('max_ms', float('inf'))
        grains = metrics.get('grains_avg', 0)
        
        p99_ok = p99 <= budget['p99_ms']
        max_ok = max_ms <= budget['max_ms']
        
        print(f"\n{config}:")
        print(f"  p99: {p99:.3f}ms (budget: {budget['p99_ms']}ms) {'✓' if p99_ok else '✗'}")
        print(f"  max: {max_ms:.3f}ms (budget: {budget['max_ms']}ms) {'✓' if max_ok else '✗'}")
        print(f"  avg grains: {grains:.1f}")
        
        if not (p99_ok and max_ok):
            passed = False
    
    print(f"\nOverall: {'PASS' if passed else 'FAIL'}")
    return 0 if passed else 1

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python check_realtime_budgets.py <perf_results.json>")
        sys.exit(1)
    
    sys.exit(check_budgets(sys.argv[1]))