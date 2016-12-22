#include <Windows.h>
#include <osg/Image>
#include <osgDB/WriteFile>

//библиотеки для поворота
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Texture2D>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgViewer/Viewer>
#include <osg/Math>
//библиотеки для поворота

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <map>
#include <random> //для равномерного распределения
#include "weave.h"

class Wang : public Weave
{
	//перечисление сторон тайла
	enum _tileSide
	{
		Left = 0,
		Bottom = 1,
		Top = 2,
		Right = 3
	};
	
	//перечисление количества цветов
	enum _wangColors
	{
		Green = 1,
		Blue = 2,
		Red = 3,
		Yellow = 4,
		lghtRed = 5
	};
	
	//перечисление позиций тайла в текстуре
	enum _tileCombine
	{
		//в комментариях - грани по которым надо подбирать искомый тайл (0-ой по умолчанию, следовательно 1-ый тайл искомый)

		L = 1,		/* 'LEFT'					- для промежуточного тайла первой строки*/
		LR = 2,		/* 'LEFT-RIGHT'				- первой строки последнего тайла*/
		B = 3,		/* 'BOTTOM'					- первого тайла новой строки*/
		LB = 4,		/* 'LEFT-BOTTOM'			- промежуточного тайла каждой новой строки*/
		LRB = 5,	/* 'LEFT-RIGHT-BOTTOM'		- последнего тайла каждой новой строки*/
		BT = 6,		/* 'BOTTOM-TOP'				- первого тайла последней строки*/
		LBT = 7,	/* 'LEFT-BOTTOM-TOP'		- промежуточного тайла последней строки*/
		LRBT = 8	/* 'LEFT-RIGHT-BOTTOM-TOP'	- последнего тайла последней строки*/
	};
	
	//структура тайла и его сторон
	struct _wangTile
	{
		int *direction;
		osg::ref_ptr<osg::Image> tile;
	};

	static const int _wangCmpntCount = 4;					//количество патчей из которых состоит 1 тайл
	static const int _wangPatchCount = 4;					//количество цветов
	static const int _wangTilesCount = 16;					//количество вэн-тайлов

	int _outputWidth;
	int _outputHeight;

	int _wangSize;
	int _patchSize;
	int _patchStep;
	int _overlapPixels;
	int _newTextureSize;

	//сторона финального тайла
	int _tileSize;

	//контейнер для 5-и патчей
	std::vector<osg::ref_ptr<osg::Image>> _wangPatches;
	
	//контейнер для финальныйх вэн-тайлов
	std::vector<osg::ref_ptr<osg::Image>> _wangTiles;	
	
	//массив комбинаций 4-рех патчей для дальнейшего генерирования тайлов
	int _direction[_wangTilesCount][_wangCmpntCount];

	unsigned char *_test;

public:
	//конструктор
	void init(osg::Image* image, int outputWidth, int outputHeight, int wangSize, int overlapPixels);

	//запускаем программу
	void run();

private:
	//создаем 4 патча для дальнейших тайлов
	void createPatches();

	//генерируем вэн-тайлы и передаем их в контейнер
	void createWangTiles();

	//смешиваем 4-ре изображения; поворачиваем его
	void wangRotate(std::vector<std::vector<osg::ref_ptr<osg::Image>>> & preWangTiles);

	//процесс проворота изображения
	osg::ref_ptr<osg::Image> getRotatedWang(osg::ref_ptr<osg::Image> inputImage);

	//вырез центра повернутого изображения
	void cutRotatedWangSector();

	//выстраиваем тайлы в финальную текстуру
	void generateWangTexture();

	//подбор необходимого тайла
	void addFittingTile(_wangTile temp, //временный контейнер найденного тайла
							int ind,	//индекс найденного тайла
							int curr, //индекс текущего тайла в векторе 
							int tileColCount,	//количество тайлов в строке
							int firstTileInLine,	//индекс первого тайла текущей строки
							std::vector<_wangTile> & wangTiles,	//финальный вектор с тайлами
							_tileCombine c); //объект перечисления комбинаций возможных позиций тайлов в текстуре












	void createPatches_test();
};

