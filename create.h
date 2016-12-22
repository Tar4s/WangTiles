#include <Windows.h>
#include <iostream>
#include <string>
#include <omp.h>

#include <osg/Image>
#include "weave.h"

class Create : public Weave
{

	int _inputWidth;
	int _inputHeight;

	int _patchSize;
	int _patchSteps;
	int _overlapPixels;

	int _patchColCount; //количство патчей по столбцам
	int _patchRowCount; //количство патчей по строкам
	int _patchNewCount;

public:
	void init(osg::Image* image, int patchSize, int overlapPixels, int patchSteps, int inputHeight, int inputWidth); //инициализируем необходимые переменные

	void createPatches(std::map<int, osg::ref_ptr<osg::Image>> & images); //создаем патчи
	void formingPatches(std::map<int, osg::ref_ptr<osg::Image>> & images, std::vector<osg::ref_ptr<osg::Image>> & formingImages); //формируем подходящий порядок

	enum overlap_direction
	{
		RIGHT = 1,
		DOWN = 2,
	};
	long overlapCompareError(osg::Image* firstPatch, osg::Image* secondPatch, overlap_direction c);

private:
	long getOverlapError(osg::Image* firstOverlap, osg::Image* secondOverlap);
};