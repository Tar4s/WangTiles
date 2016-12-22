#include "create.h"

//инициализатор
void Create::init(osg::Image* image, int patchSize, int overlapPixels, int patchSteps, int inputHeight, int inputWidth)
{
	Weave :: init(image);

	_patchSize = patchSize + overlapPixels;
	_overlapPixels = overlapPixels;
	_patchSteps = patchSteps;
	_inputHeight = inputHeight;
	_inputWidth = inputWidth;

	_patchColCount = ceil((double)inputWidth / (double)patchSize); //количство патчей по столбцам
	_patchRowCount = ceil((double)inputHeight / (double)patchSize); //количство патчей по строкам
	_patchNewCount = _patchColCount * _patchRowCount;
}

//главные функции
void Create::createPatches(std::map<int, osg::ref_ptr<osg::Image>> & images)
{
	int imageWidth = _image->s(); //ширина изображения
	int imageHeight = _image->t(); //высота изображения
	int patchCount = 0; //количество патчей

	std::cout << "Start creating patches..." << std::endl;
	unsigned long start = clock();

#pragma omp parallel for schedule(dynamic) shared(imageHeight, imageWidth, images)  /*num_threads(2)*/
	for (int startY = 0; startY <= (imageHeight - _patchSize); startY += _patchSteps) //перебор по вертикали, пока патч не вылезет за предел
	{
		for (int startX = 0; startX <= (imageWidth - _patchSize); startX += _patchSteps) //перебор по горизонтали, пока патч не вылезет за предел
		{
			osg::Image* patch = getSubImage(_image, startX, startY, startX + _patchSize, startY + _patchSize); //вырезаем кусок
			patch->setFileName(std::to_string(patchCount));	//именуем его
			images[patchCount++] = patch;	//добавили в вектор объект патча
			//std::cout << "Patch # " << patchCount + 1 << " created;" << std::endl;

			//osgDB::writeImageFile(*_images[patch_count], patch_name.append(line).append(std::to_string(patch_count)).append(".bmp"));	//сохранили в файл

			//patchCount++;
			//patch_name.clear();
		}
	}

	unsigned long end = clock();
	std::cout << patchCount << " patches are created!" << std::endl;
	std::cout << "Time created: " << end - start << std::endl;
}
void Create::formingPatches(std::map<int, osg::ref_ptr<osg::Image>> & images, std::vector<osg::ref_ptr<osg::Image>> & formingImages)
{
	srand(time(NULL));

	long overlapErrorRight = LONG_MAX;
	long overlapErrorDown = LONG_MAX;
	long overlapErrorBoth = LONG_MAX;

	osg::ref_ptr<osg::Image> buf;

	//images[rand() % images.size()]
	formingImages.push_back(images[0]); //запись в 1 элемент нового вектора, любого тайла из ветора патчей

	std::cout << std::endl << "Forming patches..." << std::endl;
	unsigned long start = clock();

	for (int c = 1; c < _patchNewCount; c++)
	{
		std::cout << ".";
		//=========================================================================================================================================
		//=========================================================================================================================================
		//=========================================================================================================================================

		if (c < _patchColCount) //для первой строчки (ошибка только по правому борту)
		{
			for (int i = 0; i < images.size(); i++)
			{
				if (formingImages[c - 1] == images[i]) continue; //чтобы не проверять текущий патч с самим собой

				long err = overlapCompareError(formingImages[c - 1], images[i], RIGHT); //получили ошибку i-го патча

				if (err < overlapErrorRight) //если она минимальна и меньше глобального минимума
				{
					//добавляем ее в буферный объект
					overlapErrorRight = err;
					buf = images[i];
				}
			}

			formingImages.push_back(buf); //вставляем в вектор результата
			overlapErrorRight = LONG_MAX;
			buf = 0;
		}
		//=========================================================================================================================================
		//=========================================================================================================================================
		//=========================================================================================================================================

		else if (c % _patchColCount == 0) //для 1 элеманта каждой новой строки (ошибка по нижнему борту)
		{
			for (int i = 0; i < images.size(); i++)
			{
				if (formingImages[c - 1] == images[i]) continue; //чтобы не проверять текущий патч с самим собой

				long err = overlapCompareError(formingImages[c - _patchColCount], images[i], DOWN); //получили ошибку i-го патча

				if (err < overlapErrorDown) //если она минимальна
				{
					//добавляем ее в буферный объект
					overlapErrorDown = err;
					buf = images[i];
				}
			}

			formingImages.push_back(buf); //вставляем в вектор результата
			overlapErrorDown = LONG_MAX;
			buf = 0;
		}

		//=========================================================================================================================================
		//=========================================================================================================================================
		//=========================================================================================================================================

		else //для всех остальных (ошибка по правому и нижнему борту)
		{
			for (int i = 0; i < images.size(); i++)
			{
				if (formingImages[c - 1] == images[i]) continue; //чтобы не проверять текущий патч с самим собой

				long err = overlapCompareError(formingImages[c - 1], images[i], RIGHT) +
					overlapCompareError(formingImages[c - _patchColCount], images[i], DOWN);

				if (err < overlapErrorBoth)
				{
					buf = images[i];
					overlapErrorBoth = err;
				}

			}

			formingImages.push_back(buf); //вставляем в вектор результата
			overlapErrorBoth = LONG_MAX;

			buf = 0;
		}

	}

	unsigned long end = clock();
	std::cout<< std::endl << "All patches are formed!" << std::endl;
	std::cout << "Time to forming: " << end - start << std::endl;
}

//вспомогательные функции
long Create::overlapCompareError(osg::Image* firstPatch, osg::Image* secondPatch, overlap_direction c)
{
	switch (c)
	{
		case RIGHT:
		{
			osg::Image* rightPatchOverlap = getSubImage(firstPatch, _patchSize - _overlapPixels, 0, _patchSize, _patchSize); //левый оверлэп
			osg::Image* leftPatchOverlap = getSubImage(secondPatch, 0, 0, _overlapPixels, _patchSize); //правый оверлэп

			return getOverlapError(leftPatchOverlap, rightPatchOverlap);
		}
		case DOWN:
		{
			osg::Image* upPatchOverlap = getSubImage(firstPatch, 0, _patchSize - _overlapPixels, _patchSize, _patchSize); //верхний оверлэп
			osg::Image* downPatchOverlap = getSubImage(secondPatch, 0, 0, _patchSize, _overlapPixels); //нижний оверлэп

			return getOverlapError(upPatchOverlap, downPatchOverlap);
		}
	}
}
long Create::getOverlapError(osg::Image* firstOverlap, osg::Image* secondOverlap)
{
	long overlapError = 0;

	int overlapWidth = firstOverlap->s();
	int overlapHeight = firstOverlap->t();

	long overlapErrCount = overlapWidth * overlapHeight * 3;

	//int *overlapErr = new int[overlapErrCount];

	unsigned char* ptrOne = firstOverlap->data();
	unsigned char* ptrTwo = secondOverlap->data();

	for (int i = 0; i < overlapErrCount; i++)
	{
		overlapError += abs(ptrOne[i] - ptrTwo[i]);
	}

	return overlapError;
}