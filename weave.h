#include <Windows.h>
#include <osg/Image>

#pragma once
class Weave
{
public:
	void init(osg::Image* image);
	osg::Image* getSubImage(osg::Image* sourceImage, int startX, int startY, int stopX, int stopY);		//функция извлечения изображения

protected:
	osg::Image* _image;
};

