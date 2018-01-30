// muffin.cpp : Defines the entry point for the console application.

#include "../includes/stdafx.h"

int main(void)
{
	t_env	*env;
	env = init::env();
	//same here
	std::string data_file = "..\\resources\\bs.csv";
	std::string config_file = "..\\resources\\config.txt";
	//start clockwatch
	auto t1 = std::chrono::high_resolution_clock::now();
	//tick::load_all(file_name, env);
	//tick::set(file_name, data);
	//while (tick::next(data));
	//stop clockwatch
	auto t2 = std::chrono::high_resolution_clock::now();
	auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

	printf("Parsing took %d ms\n", (int)int_ms.count());
	//long a = data->epoch->size();
	//long b = data->px->size();
	//long c = data->vol->size();
	//for (int x = data->epoch->size() - 1; x>=0; x--)
	//	printf("%zu,%f,%f\n", data->epoch->at(x), data->px->at(x), data->vol->at(x));

	return (0);
}