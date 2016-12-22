#include <Windows.h>
#include <iostream>
#include <string>

#include <osg/Image>
#include <osgDB/WriteFile>
#include "weave.h"

class Mix : public Weave
{
	enum overlap_direction
	{
		RIGHT = 1,
		DOWN = 2,
	};

	int tileCount;

	int _patchSize;
	int _overlapPixels;

	int _inputWidth;
	int _inputHeight;

	int _patchColCount; //количство патчей по столбцам
	int _patchRowCount; //количство патчей по строкам

	//результирующее разрешение итоговой картинки
	int _outputWidth;
	int _outputHeight;
	//---

	std::vector<int> _minimalErrorRim;
	unsigned char* _test;

public:
	Mix() : tileCount(0){};
	void init(int patchSize, int overlapPixels, int inputWidth, int inputHeight);
	void boundaryCut(std::vector<osg::ref_ptr<osg::Image>> & formingImages);
	void saveFile(std::string resultFileDest);

private:
	void overlapMinimalErrorBoundary(osg::Image* firstPatch, osg::Image* secondPatch, overlap_direction c);
	void getMinimalErrorBoundary(osg::Image* firstOverlap, osg::Image* secondOverlap, bool flagDirection);
	//osg::Image* getSubImage(osg::Image* sourceImage, int startX, int startY, int stopX, int stopY);
};
