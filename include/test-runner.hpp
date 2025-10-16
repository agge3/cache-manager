/**
 * @file test-runner.hpp
 * @author agge3
 * @data 2025-16-10
 *
 * Test runner, configuration, and configuration read-in (includes random
 * distribution generators).
 */

#pragma once

#include <nlohmann/json.hpp>

#include <thread>
#include <vector>
#include <algorithm>
#include <random>
#include <memory>
#include <any>
#include <functional>
#include <iostream>
#include <fstream>

enum class DistrType {
	INVALID,
	STRING,
	INT,
	DOUBLE
};

enum class DistrGen {
	INVALID,
	NORMAL,
	UNIFORM
};

struct DistrCfg {
	DistrType type;
	DistrGen gen;
	size_t count;
	size_t min;
	size_t max;
};

template <typename T>
class Distribution {
public:
	Distribution(const DistrCfg &cfg) : _cfg(cfg), _gen(std::random_device{}()) {
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
				_data.push_back(static_cast<int>(val);
			};
		} else if constexpr (std::is_same_v<T, double>) {
			_type = "double";
			_emitFn = [this](double val) {
				_data.push_back(val);
			};
		} else if constexpr (std::is_same_v<T, std::string>) {
			_type = "string";
			_emitFn = [this](double val) {
				// xxx accumulate multi-length strings
				_data.push_back(std::string(1, 'a' + static_cast<int>(val) % 26));
			};
		} else {
			_type = "invalid";
			_emitFn = [](double) {};	// noop for invalid type
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
		std::normal_distribution<double> dist(
			(_cfg.min + _cfg.max) / 2.0,
			(_cfg.max - _cfg.min) / 6.0);
		return std::clamp(dist(_gen), static_cast<double>(_cfg.min), static_cast<double>(_cfg.max));
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
	size_t threads;
	size_t iter;
	DistrCfg distr_cfg;
	std::any distr_data;
	std::vector<std::function<void(std::any&)>> fns;
	TestCfg(size_t threads, size_t iter, const DistrCfg& distr_cfg) :
		threads(threads), iter(iter), distr_cfg(distr_cfg) {}
};

// opaque pointer for storing different TestCfg template specializations
using TestCfgHandle = std::unique_ptr<TestCfg>;

using TestCfgs = std::vector<TestCfgHandle>;

TestCfgHandle makeTestCfg(size_t threads, size_t iter, const DistrCfg &distr_cfg, const std::vector<std::function<void(std::any&)>> &fns) {
	switch (distr_cfg.type) {
	case DistrType::INT: {
		std::vector<std::function<void(const std::vector<int>&)>> typed_fns;
		for (const auto &f : fns) {
			typed_fns.push_back([f](const std::vector<int> &v) {
				f(const_cast<std::any&>(v));
			});
		}
		auto test = std::make_unique<TestCfg>(threads, iter, distr_cfg);
		Distribution<int> dist(dist_cfg);
		dist.generate();
		test->distr_data = dist.data();
		test->fns = typed_fns;
		return test;
	}
	case DistrType::DOUBLE: {
		std::vector<std::function<void(const std::vector<double>&)>> typed_fns;
		for (const auto &f : fns) {
			typed_fns.push_back([f](const std::vector<double> &v) {
				f(const_cast<std::any&>(v));
			});
		}
		auto test = std::make_unique<TestCfg>(threads, iter, distr_cfg);
		Distribution<double> dist(dist_cfg);
		dist.generate();
		test->distr_data = dist.data();
		test->fns = typed_fns;
		return test;
	}
	case DistrType::STRING: {
		std::vector<std::function<void(const std::vector<std::string>&)>> typed_fns;
		for (const auto &f : fns) {
			typed_fns.push_back([f](const std::vector<std::string> &v) {
				f(const_cast<std::any&>(v));
			});
		}
		auto test = std::make_unique<TestCfg>(threads, iter, distr_cfg);
		Distribution<std::string> dist(dist_cfg);
		dist.generate();
		test->distr_data = dist.data();
		test->fns = typed_fns;
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
		std::vector<std::function<void(std::any&)>> fns;
		auto functions = t["functions"];
		DistrCfg distr_cfg = parseDistrCfg(t);
		tests.push_back(makeTestCfg(threads, iter, distr_cfg, functions));
	}

	return tests;
}


class TestRunner {
public:
	TestRunner(TestCfgs tests) : _tests(std::move(tests)) {}

	void run() {
		for (auto &t : _tests) {
			for (auto i = 0; i < t->iter; ++i) {
				runTest(*t);
			}
		}
	}

	void expect() {
	}
private:
	void runTest(const TestCfg &test) {
		std::vector<std::thread> pool;

		for (auto i = 0; i < test.threads; ++i) {
			pool.emplace_back([&test]() {
				for (const auto &f : test.fns) {
					f(test.distr_data);
				}
			});
		}

		for (auto &th : pool) {
			th.join();
		}
	}

	TestCfgs _tests;
};

// EOF
