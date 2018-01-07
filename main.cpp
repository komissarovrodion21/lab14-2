#include <iostream>
#include <chrono>
#include <boost/program_options.hpp>
#include <boost/process.hpp>

namespace bp = boost::process;
namespace po = boost::program_options;

int _time = 0;

bp::child makeProject(std::string build = "Debug") {
	std::string path = "cmake -H. -Bexamples/_build -DCMAKE_INSTALL_PREFIX=_install -DCMAKE_BUILD_TYPE=" + build;

	bp::child c(path, bp::std_out > stdout);
	if (_time) {
		if (!c.wait_for(std::chrono::seconds(_time)))
			c.terminate();

	}
	else {
		c.wait();
	}
	return c;
}

bp::child buildProject() {
	std::string path = "cmake --build examples/_build";

	bp::child c(path, bp::std_out > stdout);
	if (_time) {
		if (!c.wait_for(std::chrono::seconds(_time)))
			c.terminate();

	}
	else {
		c.wait();
	}
	return c;
}

bp::child setTargets(std::string Target) {
	std::string path = "cmake --build examples/_build --target " + Target;

	bp::child c(path, bp::std_out > stdout);
	//c.wait();
	return c;
}

int main(int argc, char const *argv[]) {
	try {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "выводим вспомогательное сообщение")
			("config", po::value<std::string>(), "указываем конфигурацию сборки (по умолчанию Debug)")
			("install", "добавляем этап установки (в директорию _install)")
			("pack", "добавляем этап упакови (в архив формата tar.gz)")
			("timeout", po::value<int>(), "указываем время ожидания (в секундах)");

		po::variables_map vm; 
		po::store(po::parse_command_line(argc, argv, desc), vm);
																 
		po::notify(vm);

		if (vm.count("help"))
		{
			std::cout << desc << std::endl;
			return 1;
		}
    
		else if (vm.count("config")) {
			std::string conf(vm["config"].as<std::string>());

			if (makeProject(conf).exit_code())
				throw std::runtime_error("ERROR: Make project fail!");

			std::cout << "Result:\t" << buildProject().exit_code() << std::endl;
		}

		else if (vm.count("install")) {
			if (makeProject().exit_code())
				throw std::runtime_error("ERROR: Make project fail!");

			if (buildProject().exit_code())
				throw std::runtime_error("ERROR: Build project fail!");

			std::cout << "Result:\t" << setTargets("install").exit_code() << std::endl;
		}

		else if (vm.count("pack")) {
			if (makeProject().exit_code())
				throw std::runtime_error("ERROR: Make project fail!");

			if (buildProject().exit_code())
				throw std::runtime_error("ERROR: Build project fail!");

			std::cout << "Result:\t" << setTargets("package").exit_code() << std::endl;
		}

		else if (vm.count("timeout")) {
			int time = vm["timeout"].as<int>();

			if (makeProject().exit_code())
				throw std::runtime_error("ERROR: Make project fail!");

			std::cout << "Result:\t" << buildProject().exit_code() << std::endl;
		}

		else {
			if (makeProject().exit_code())
				throw std::runtime_error("ERROR: Make project fail!");

			buildProject();
			std::cout << "Result:\t" << buildProject().exit_code() << std::endl;
		}

	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
