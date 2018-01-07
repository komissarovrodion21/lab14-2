#include <iostream>
#include <chrono>
#include <boost/program_options.hpp>
#include <boost/process.hpp>

namespace bp = boost::process; //подпространство bp для системных процессов
namespace po = boost::program_options; //подпространство po для разбора командной строки

int _time = 0;

//#1
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

//#2
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

//#3
bp::child setTargets(std::string Target) {
	std::string path = "cmake --build examples/_build --target " + Target;

	bp::child c(path, bp::std_out > stdout);
	//c.wait();
	return c;
}

int main(int argc, char const *argv[]) {
	try {
		// Declare the supported options. (Объявить поддерживаемые параметры.)
		po::options_description desc("Allowed options"); //хранит все разрешенные опции
		desc.add_options()
			("help", "выводим вспомогательное сообщение") //имя опции, описание параметра для пользователя, 
			("config", po::value<std::string>(), "указываем конфигурацию сборки (по умолчанию Debug)") //второй аргумент говорит о типе, к которому будет приводится аргумент
			("install", "добавляем этап установки (в директорию _install)")
			("pack", "добавляем этап упакови (в архив формата tar.gz)")
			("timeout", po::value<int>(), "указываем время ожидания (в секундах)");

		po::variables_map vm; //variables_map класс для хранения значений опций(для хранения значений параметров) 
		po::store(po::parse_command_line(argc, argv, desc), vm); //парсинг аргументов и сохранение их в переменную vm
																 //вызов функции store-добавить значения,которые нашли в командной строке; parse_command_line - функция для парсинга компонентов; 

		po::notify(vm); //notify может использоваться для сообщения о любых ошибках при передаче параметров


		if (vm.count("help")) // Check if an option was passed in the command line (Передана ли опция в командной строке)
		{
			std::cout << desc << std::endl;
			return 1;
		}

		//конфигурация сборки <Release|Debug>
		else if (vm.count("config")) {
			std::string conf(vm["config"].as<std::string>()); // Функция.as<Type>(), применённая к этой структуре, возвращает значение аргумента опции, расценивая информацию для значения как объект типа Type

			if (makeProject(conf).exit_code())
				throw std::runtime_error("ERROR: Make project fail!");

			std::cout << "Result:\t" << buildProject().exit_code() << std::endl;
		}

		//этап установки в директорию _install
		else if (vm.count("install")) {
			if (makeProject().exit_code())
				throw std::runtime_error("ERROR: Make project fail!");

			if (buildProject().exit_code())
				throw std::runtime_error("ERROR: Build project fail!");

			std::cout << "Result:\t" << setTargets("install").exit_code() << std::endl;
		}

		//этап упаковки в архив формата tar.gz
		else if (vm.count("pack")) {
			if (makeProject().exit_code())
				throw std::runtime_error("ERROR: Make project fail!");

			if (buildProject().exit_code())
				throw std::runtime_error("ERROR: Build project fail!");

			std::cout << "Result:\t" << setTargets("package").exit_code() << std::endl;
		}

		//время ожидания
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
