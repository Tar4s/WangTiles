#include <Windows.h>
#include <iostream>
#include <string>
#include <locale>

#include <osg/Image>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include "create.h"
#include "mix.h"
#include "wang.h"

int main(int argc, char *argv[])
{

#pragma region checking_input_parameters

	setlocale(LC_ALL, "rus");

	if (argc < 5)
	{
		std::cout << "Не все параметры введены!\n\n[ширина текстуры] [высота текстуры] [сторона блока] [наложение]\n\n";
		system("pause");
		return 1;
	}

	int outputWidth = atoi(argv[1]);
	int outputHeight = atoi(argv[2]);
	int wangSize = atoi(argv[3]);
	int overlapPixels = atoi(argv[4]);

	if (outputWidth <= 0 || outputHeight <= 0 || wangSize <= 0 || overlapPixels <= 0)
	{
		std::cout << "Некоторые параметры введены неверно!\n\n";
		system("pause");
		return 1;
	}
	
	if (outputWidth % wangSize != 0 || outputHeight % wangSize != 0)
	{
		std::cout << "Внимание! Заданные размеры текстуры и размер стороны блока несовместимы. Финальная текстура будет со швами\n";
		system("pause");
	}

#pragma endregion Предварительная проверка входных параметров

#pragma region input_data

	osg::Image* image = osgDB::readImageFile("D:\\Orion\\img\\image.bmp");

#pragma endregion Определение входных параметорв

	Wang objWangTiles;

	//инизиализируем поля класса
	objWangTiles.init(image, outputWidth, outputHeight, wangSize, overlapPixels);
	//запускаем
	objWangTiles.run();

	system("pause");

	return 0;
}