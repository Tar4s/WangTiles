#include "weave.h"

//инициализатор
void Weave::init(osg::Image* image)
{
	_image = image;
}

//вспомогательные функции
osg::Image* Weave::getSubImage(osg::Image* sourceImage, int startX, int startY, int stopX, int stopY)
{
	int subImageWidth = stopX - startX;
	int subImageHeight = stopY - startY;
	if (subImageWidth <= 0 || subImageHeight <= 0)
	{
		osg::notify(osg::ALWAYS) << "Error the subimage size is not correct. Aborting.";
		return NULL;
	}

	const unsigned char *sourceData = sourceImage->data();

	struct pixelStruct
	{
		unsigned char r, g, b;
	};

	osg::Image* subImage = new osg::Image();

	// allocate memory for image data
	const unsigned char *targetData = new unsigned char[subImageWidth * subImageHeight * 3];

	struct pixelStruct* pPixelSource = (struct pixelStruct *) (sourceData);
	struct pixelStruct* pPixelTarget = (struct pixelStruct *) (targetData);
	struct pixelStruct* pCurrentPixelSource = NULL;
	struct pixelStruct* pCurrentPixelTarget = NULL;

	int subX = 0;
	int subY = 0;

	for (int x = startX; x < stopX; x++)
	{
		for (int y = startY; y < stopY; y++)
		{
			pCurrentPixelSource = &pPixelSource[y * sourceImage->s() + x];
			pCurrentPixelTarget = &pPixelTarget[subY * subImageWidth + subX];

			pCurrentPixelTarget->r = pCurrentPixelSource->r;
			pCurrentPixelTarget->g = pCurrentPixelSource->g;
			pCurrentPixelTarget->b = pCurrentPixelSource->b;

			subY++;
		}
		subY = 0;
		subX++;
	}

	subImage->setImage(subImageWidth, subImageHeight, sourceImage->r(), sourceImage->getInternalTextureFormat(), sourceImage->getPixelFormat(), 5121, (unsigned char*)targetData, osg::Image::NO_DELETE);

	return subImage;
}