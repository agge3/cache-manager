/**
 * @file test-runner.hpp
 * @author agge3
 * @data 2025-16-10
 *
 * Test runner, configuration, and configuration read-in (includes random
 * distribution generators).
 */

#pragma once

#include "cache-manager.hpp"
#include "macros.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <any>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <thread>
#include <vector>
#include <format>

enum class DistrType { INVALID, STRING, INT, DOUBLE };

enum class DistrGen { INVALID, NORMAL, UNIFORM };

struct DistrCfg {
	DistrType type;
	DistrGen gen;
	size_t count;
	size_t min;
	size_t max;
};

template <typename T> class Distribution {
  public:
	Distribution(const DistrCfg &cfg)
		: _cfg(cfg), _gen(std::random_device{}()) {
		switch (_cfg.gen) {
		case DistrGen::NORMAL:
			_genFn = [this]() { return normal(); };
			break;
		case DistrGen::UNIFORM:
			_genFn = [this]() { return uniform(); };
			break;
		default:
			_genFn = [this]() { return normal(); };
		}
		if constexpr (std::is_same_v<T, int>) {
			_type = "int";
			_emitFn = [this](double val) {
				_data.push_back(static_cast<int>(val));
			};
		} else if constexpr (std::is_same_v<T, double>) {
			_type = "double";
			_emitFn = [this](double val) { _data.push_back(val); };
		} else if constexpr (std::is_same_v<T, std::string>) {
			_type = "string";
			_emitFn = [this](double val) {
				// xxx accumulate multi-length strings
				_data.push_back(
					std::string(1, 'a' + static_cast<int>(val) % 26));
			};
		} else {
			_type = "invalid";
			_emitFn = [](double) {}; // noop for invalid type
		}
	}
	void generate() {
		_data.clear();
		for (auto i = 0; i < _cfg.count; ++i) {
			_emitFn(_genFn());
		}
	}
	const std::vector<T> &data() const { return _data; }
	std::string type() const { return _type; }

  private:
	double uniform() {
		std::uniform_real_distribution<double> dist(_cfg.min, _cfg.max);
		return dist(_gen);
	}
	double normal() {
		std::normal_distribution<double> dist((_cfg.min + _cfg.max) / 2.0,
											  (_cfg.max - _cfg.min) / 6.0);
		return std::clamp(dist(_gen), static_cast<double>(_cfg.min),
						  static_cast<double>(_cfg.max));
	}

	std::string _type;
	DistrCfg _cfg;
	std::mt19937 _gen;
	std::vector<T> _data;
	std::function<double()> _genFn;
	std::function<void(double)> _emitFn;
};

// incomplete struct that is retrieved, with template specialization, from
// behind TestCfgHandle.
// @see makeTestCfg factory to make complete handle.
struct TestCfg {
	using TestFns = std::vector<std::string>;
	size_t threads;
	size_t iter;
	size_t capacity;
	std::string name;
	DistrCfg distr_cfg;
	std::any distr_data;
	TestFns fns;
	TestCfg(size_t threads, size_t iter, size_t capacity, const std::string &name,
			const DistrCfg &distr_cfg, const TestFns &fns)
		: threads(threads), iter(iter), capacity(capacity), name(name),
		  distr_cfg(distr_cfg), fns(fns) {}
};

// opaque pointer for storing different TestCfg template specializations
using TestCfgHandle = std::unique_ptr<TestCfg>;

using TestCfgs = std::vector<TestCfgHandle>;


TestCfgHandle makeTestCfg(size_t threads, size_t iter, size_t capacity, const std::string &name,
						  const DistrCfg &distr_cfg,
						  std::vector<std::string> &fns) {
	switch (distr_cfg.type) {
	case DistrType::INT: {
		auto test =
			std::make_unique<TestCfg>(threads, iter, capacity, name, distr_cfg, fns);
		Distribution<int> dist(distr_cfg);
		dist.generate();
		test->distr_data = dist.data();
		return test;
	}
	case DistrType::DOUBLE: {
		auto test =
			std::make_unique<TestCfg>(threads, iter, capacity, name, distr_cfg, fns);
		Distribution<double> dist(distr_cfg);
		dist.generate();
		test->distr_data = dist.data();
		return test;
	}
	case DistrType::STRING: {
		auto test =
			std::make_unique<TestCfg>(threads, iter, capacity, name, distr_cfg, fns);
		Distribution<std::string> dist(distr_cfg);
		dist.generate();
		test->distr_data = dist.data();
		return test;
	}
	default:
		throw std::runtime_error("invalid distribution type");
	}
}

DistrCfg parseDistrCfg(const nlohmann::json &j) {
	DistrCfg cfg;
	std::string type = j["distr"]["type"];
	if (type == "string") {
		cfg.type = DistrType::STRING;
	} else if (type == "int") {
		cfg.type = DistrType::INT;
	} else if (type == "double") {
		cfg.type = DistrType::DOUBLE;
	} else {
		cfg.type = DistrType::INVALID;
	}
	std::string gen = j["distr"]["gen"];
	if (gen == "normal") {
		cfg.gen = DistrGen::NORMAL;
	} else if (gen == "uniform") {
		cfg.gen = DistrGen::UNIFORM;
	} else {
		cfg.gen = DistrGen::INVALID;
	}
	cfg.count = static_cast<size_t>(j["distr"]["count"]);
	cfg.min = static_cast<size_t>(j["distr"]["min"]);
	cfg.max = static_cast<size_t>(j["distr"]["max"]);
	return cfg;
}

/**
 * Reads in JSON configuration from path and returns populated TestCfgs.
 */
TestCfgs readConfig(const std::string &path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "failed to open file: " << path << "\n";
		return TestCfgs{};
	}

	nlohmann::json j;
	try {
		file >> j;
	} catch (const std::exception &e) {
		std::cerr << "failed to parse json: " << e.what() << "\n";
		return TestCfgs{};
	}

	TestCfgs tests;
	for (const auto &t : j["tests"]) {
		std::string name = t["name"];
		size_t threads = static_cast<size_t>(t["threads"]);
		size_t iter = static_cast<size_t>(t["iter"]);
		size_t capacity = static_cast<size_t>(t["capacity"]);
		auto functions = t["functions"];
		std::vector<std::string> fns;
		fns.reserve(functions.size());
		for (const auto &f : functions) {
			fns.push_back(f.get<std::string>());
		}
		DistrCfg distr_cfg = parseDistrCfg(t);
		tests.push_back(makeTestCfg(threads, iter, capacity, name, distr_cfg, fns));
	}

	return tests;
}

class TestRunner {
  public:
	TestRunner(TestCfgs tests) : _tests(std::move(tests)) {}

	void run() {
		for (auto &t : _tests) {
			std::cout << "INFO: running test: " << t->name << "\n";
			switch (t->distr_cfg.type) {
			case DistrType::INT:
				runTest<int>(*t);
				break;
			case DistrType::DOUBLE:
				runTest<double>(*t);
				break;
			case DistrType::STRING:
				runTest<std::string>(*t);
				break;
			default:
				throw std::runtime_error("invalid distribution type");
			}
			std::cout << "INFO: completed test: " << t->name << "\n";
		}
	}

	void expect() {}

  private:
	template <typename T> void runTest(const TestCfg &test) {
		cm::CacheManager<T, T, cm::TbbBench> cache(test.capacity);
		const auto &data =
			std::any_cast<const std::vector<T> &>(test.distr_data);
		std::vector<T> keys = data;
		size_t size = data.size();
		std::reverse(keys.begin(), keys.end());
		for (auto i = 0; i < test.iter; ++i) {
			std::vector<std::thread> pool;
			for (auto j = 0; j < test.threads; ++j) {
				pool.emplace_back([&]() {
					for (const auto &f : test.fns) {
						for (auto k = 0; k < size; ++k) {
							const T &key = keys[k];
							const T &val = data[k];
							if (f == "add") {
								auto res = cache.add(key, val);
							} else if (f == "get") {
								auto res = cache.getItem(key);
							} else if (f == "contains") {
								auto res = cache.contains(key);
							} else if (f == "remove") {
								auto res = cache.remove(key);
							} else {
								std::cerr << "ERROR: invalid function in "
											 "configuration: "
										  << f << "\n";
							}
						}
					}
				});
			}

			for (auto &th : pool) {
				th.join();
			}
		}

		DPRINT("XXX END BENCHMARK");
		DPRINT("XXX BEGIN BENCHMARK PRINT");
		auto bm = cache.benchmark();
		cm::printBenchmark(bm);
		cm::writeBenchmark(bm);
		DPRINT("XXX END BENCHMARK PRINT");
	}

	TestCfgs _tests;
};

// EOF
