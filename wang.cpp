#include "wang.h"
#include "mix.h"
#include "create.h"

//макрос инициализации массива
#define DIRECTION_INIT( par1, c1, c2, c3, c4 ) par1[0] = c1; par1[1] = c2; par1[2] = c3; par1[3] = c4;

//макрос вывода имени переменной и ее значения на экран
#define OUTPUT( var ) std::cout << #var << "=" << var << std::endl;

//инициализатор
void Wang::init(osg::Image* image, int outputWidth, int outputHeight, int wangSize, int overlapPixels)
{
	Weave::init(image);

	_outputWidth = outputWidth;
	_outputHeight = outputHeight;

	_wangSize = wangSize;
	_patchSize = wangSize / sqrt(2);
	_patchStep = 35;
	_overlapPixels = overlapPixels;

	OUTPUT(_outputWidth);
	OUTPUT(_outputHeight);
	OUTPUT(_wangSize);
	OUTPUT(_patchSize);
	OUTPUT(_overlapPixels);

	DIRECTION_INIT(_direction[0], Red, Blue, Blue, Red); //нижний, правый, левый, верхний
	DIRECTION_INIT(_direction[1], Red, Blue, Yellow, Red);
	DIRECTION_INIT(_direction[2], Green, Blue, Blue, Red);
	DIRECTION_INIT(_direction[3], Green, Blue, Yellow, Red);
	DIRECTION_INIT(_direction[4], Red, Yellow, Blue, Red);
	DIRECTION_INIT(_direction[5], Red, Yellow, Yellow, Red);
	DIRECTION_INIT(_direction[6], Green, Yellow, Blue, Red);
	DIRECTION_INIT(_direction[7], Green, Yellow, Yellow, Red);
	DIRECTION_INIT(_direction[8], Red, Blue, Blue, Green);
	DIRECTION_INIT(_direction[9], Red, Blue, Yellow, Green);
	DIRECTION_INIT(_direction[10], Green, Blue, Blue, Green);
	DIRECTION_INIT(_direction[11], Green, Blue, Yellow, Green);
	DIRECTION_INIT(_direction[12], Red, Yellow, Blue, Green);
	DIRECTION_INIT(_direction[13], Red, Yellow, Yellow, Green);
	DIRECTION_INIT(_direction[14], Green, Yellow, Blue, Green);
	DIRECTION_INIT(_direction[15], Green, Yellow, Yellow, Green);

}


//главные функции
void Wang::run()
{
	this->createPatches();
	this->createWangTiles();
}
void Wang::createPatches()
{
	srand(time(NULL));

	//получаем данные об изображении
	int imgWidth = _image->s();
	int imgHeight = _image->t();

	int startX, stopX;
	int startY, stopY;

	std::cout << "\n1. Получаем патчи.\n\n";
	//извлекаем 4 патча из osg::image
	for (int patchNum = 0; patchNum < _wangPatchCount; patchNum++)
	{
		//генерируем начальную точку
		startX = rand() % imgWidth;
		startY = rand() % imgHeight;

		//получаем конечную точку
		stopX = startX + (_patchSize + _overlapPixels); //_overlapPixels нужен для формирования заведомо большего изображения
		stopY = startY + (_patchSize + _overlapPixels); //чтобы выполнить Crop и размер результата соответсвовал заявленному

		//если конечная точка не входит в диапазон картинки
		while (stopX > imgWidth || stopY > imgHeight)
		{
			//генерируем новую точку, чтобы входила в картинку
			startX = rand() % imgWidth;
			startY = rand() % imgHeight;

			stopX = startX + (_patchSize + _overlapPixels); //_overlapPixels нужен для формирования заведомо большего изображения
			stopY = startY + (_patchSize + _overlapPixels); //чтобы выполнить Crop и размер результата соответсвовал заявленному
		}

		//вырезали n-ый патч
		osg::Image* patch = Weave::getSubImage(_image, startX, startY, stopX, stopY);

		//записали его имя (номер+1)
		patch->setFileName(std::to_string(patchNum + 1));

		//добавили его в вектор
		_wangPatches.push_back(patch);

		//вывели сообщение на экран
		std::cout << "Патч №" << patchNum << " извлечен;\n";
	}
	std::cout << std::endl;
	system("pause");
}
void Wang::createWangTiles()
{
	std::vector<std::vector<osg::ref_ptr<osg::Image>>> preWangTiles;

	//объявляем временный вектор для хранения 4-х тайлов
	std::vector<osg::ref_ptr<osg::Image>> temp;

	std::cout << "\n2. Формируем тайлы на основе патчей.\n\n";
	//цикл формирования 8 вэн-тайлов
	for (int i = 0; i < _wangTilesCount; i++)
	{
		//заполняем временный вектор 4-мя картинками
		//в нужной нам последовательности
		for (int k = 0; k < _wangCmpntCount; k++)
		{
			//добавляем в temp нужный нам патч
			temp.push_back(_wangPatches[_direction[i][k] - 1]);
		}

		//добавляем в главный вектор, вектор с картинками
		//в нужной последовательности (тайл)
		preWangTiles.push_back(temp);
		temp.clear();
		std::cout << "Тайл №" << i << " скомбинирован;\n";
	}
	std::cout << std::endl;
	system("pause");

	wangRotate(preWangTiles);
}















void Wang::createPatches_test()
{
	srand(time(NULL));

	//получаем данные об изображении
	int imgWidth = _image->s();
	int imgHeight = _image->t();

	int patchCount = 0;
	int patchSize = _patchSize + _overlapPixels;

	int stopY = imgHeight - patchSize;
	int stopX = imgWidth - patchSize;

	for (int startY = 0; startY <= stopY; startY += _patchStep) //перебор по вертикали, пока патч не вылезет за предел
	{
		for (int startX = 0; startX <= stopX; startX += _patchStep) //перебор по горизонтали, пока патч не вылезет за предел
		{
			osg::Image* patch = getSubImage(_image, startX, startY, startX + patchSize, startY + patchSize); //вырезаем кусок
			patch->setFileName(std::to_string(patchCount));	//именуем его
			_wangPatches.push_back(patch);	//добавили в вектор объект патча

			//osgDB::writeImageFile(*_images[patch_count], patch_name.append(line).append(std::to_string(patch_count)).append(".bmp"));	//сохранили в файл

			patchCount++;
			//patch_name.clear();
		}
	}
	std::cout << "Извлечено " << patchCount + 1 << " патчей.\n\n";
	system("pause");
}


















//вспомогательные функции
void Wang::wangRotate(std::vector<std::vector<osg::ref_ptr<osg::Image>>> & preWangTiles)
{
	std::vector<osg::ref_ptr<osg::Image>> wangRotatedTiles;
	std::cout << "\n3. Формируем конечные вэн-тайлы.\n\n";

#pragma region mix_data

	{
		Mix objMixingTiles;

		const int outputWidth = _patchSize * 2;
		const int outputHeight = outputWidth;

		objMixingTiles.init(_patchSize, _overlapPixels, outputWidth, outputHeight);

		for (int i = 0; i < _wangTilesCount; i++)
		{
			objMixingTiles.boundaryCut(preWangTiles[i]);
		}
	}

	std::cout << std::endl;
	system("pause");

#pragma endregion Смешиваем патчи для получения тайлов


	for (int i = 0; i < _wangTilesCount; i++)
	{
		osg::Image* inputImage = preWangTiles[i][0];
		osg::ref_ptr<osg::Image> outputImage = getRotatedWang(inputImage);

		outputImage->setFileName(std::to_string(i));

		wangRotatedTiles.push_back(outputImage);
		std::cout << "Вэн-тайл №" << i << " повернут и извлечен;\n";

		//osgDB::writeImageFile(*inputImage, "D:\\Orion\\img\\" + inputImage->getFileName() + ".bmp");	//сохранили вфайл
		//osgDB::writeImageFile(*outputImage, "D:\\Orion\\img\\" + preWangTiles[i][0]->getFileName() + "_rtd.bmp");	//сохранили вфайл
	}
	wangRotatedTiles.swap(_wangTiles);

	std::cout << std::endl;
	system("pause");

	generateWangTexture();
}
void Wang::cutRotatedWangSector()
{
	struct coord
	{
		int x;
		int y;
	};

	int wangSize = _wangTiles.size();
	std::vector<osg::ref_ptr<osg::Image>> temp;
	coord startPoint, stopPoint;

	startPoint.x = _newTextureSize / 4;
	startPoint.y = startPoint.x;

	stopPoint.x = _newTextureSize - _newTextureSize / 4;
	stopPoint.y = stopPoint.x;

	for (int i = 0; i < wangSize; i++)
	{
		osg::ref_ptr<osg::Image> patch = getSubImage(_wangTiles[i], startPoint.x, startPoint.y, stopPoint.x, stopPoint.y);
		patch->setFileName(std::to_string(i));

		osgDB::writeImageFile(*patch, "D:\\Orion\\img\\" + std::to_string(i) + "_cut_.bmp");	//сохранили вфайл
		temp.push_back(patch);
	}

	_wangTiles.swap(temp);
	_tileSize = _wangTiles[0]->t(); //получаем размер стороны финального тайла
	temp.clear();

	generateWangTexture();
}
void Wang::generateWangTexture()
{
#pragma region init_variables

	srand(time(NULL));

	_tileSize = _wangTiles[0]->t();

	int firstTileInLine = 0; //номер первого тайла в строке
	int lastTileInLine; //номер последнего тайла в строке
	int curTileLine = 1; //номер текущей строки тайлов

	//количество тайлов по вертикали/горизонтали
	int tileColCount = _outputWidth / _tileSize;
	if (_outputWidth % _tileSize != 0) tileColCount++;

	int tileRowCount = _outputHeight / _tileSize;
	if (_outputHeight % _tileSize != 0) tileRowCount++;

	//всего тайлов
	int allTilesCount = tileColCount * tileRowCount;

	//количество байт на все тайлы
	int allTilesByte = allTilesCount * ((_tileSize * _tileSize) * 3);

	//массив результата
	_test = new unsigned char[allTilesByte];

	//стрктура хранения тайла и индексов его сторон
	std::vector<_wangTile> wangTiles;
	_wangTile temp;

	//последовательность #10 (верх=низ), необходимо для случая, когда строка тайлов единственная
	int ind = rand() % _wangTilesCount;
	temp.direction = _direction[ind];

	//добалвяем в вектор соответствующий последовательности тайл
	temp.tile = _wangTiles[ind];
	wangTiles.push_back(temp);

#pragma endregion Объявляем переменные

	std::cout << "\n4. Формируем текстуру.\n";
#pragma region forming_wang_tiles

	//цикл перебора всех тайлов, начиная с 1, т.к. 0-ой уже добавлен выше
	for (int j = 1; j < allTilesCount; j++)
	{
		//номер текущей строки
		if (j % tileColCount == 0) curTileLine++;

		//номер последнего тайла в текущей строке
		lastTileInLine = firstTileInLine + (tileColCount - 1); //номер первого тайла + (общее количество тайлов в строке - 1) т.к. с нуля)

#pragma region first_line

		if (j < tileColCount) //для заполнения первой строчки
		{
			//индекс искомого тайла
			ind = rand() % _wangTiles.size();

			//если строка не последняя
			if (curTileLine != tileRowCount)
			{
				//если искомый тайл не последний
				if (j != lastTileInLine)
				{
					addFittingTile(temp, ind, j, tileColCount, firstTileInLine, wangTiles, L); //LEFT
				}
				else //если текущий тайл последний
				{
					addFittingTile(temp, ind, j, tileColCount, firstTileInLine, wangTiles, LR); //LEFT-RIGHT
				}
			}
		}

#pragma endregion Для заполнения первой строчки

#pragma region first_tile_fisrt_line

		else if (j % tileColCount == 0) //для первого элемента новой строчки
		{
			firstTileInLine = j;
			ind = rand() % _wangTiles.size();

			//если строка не последняя
			if (curTileLine != tileRowCount)
			{
				addFittingTile(temp, ind, j, tileColCount, firstTileInLine, wangTiles, B); //BOTTOM
			}
			//если строка последняя
			else
			{
				addFittingTile(temp, ind, j, tileColCount, firstTileInLine, wangTiles, BT); //BOTTOM-TOP
			}
		}

#pragma endregion Для первого элемента новой строчки

#pragma region for_all_other

		else //для всех остальных
		{
			ind = rand() % _wangTiles.size();

			//если строка не последняя
			if (curTileLine != tileRowCount)
			{
				//если искомый тайл не последний
				if (j != lastTileInLine)
				{
					addFittingTile(temp, ind, j, tileColCount, firstTileInLine, wangTiles, LB); //LEFT-BOTTOM
				}
				else //если текущий тайл последний
				{
					addFittingTile(temp, ind, j, tileColCount, firstTileInLine, wangTiles, LRB); //LEFT-RIGHT-BOTTOM
				}
			}
			//если строка последняя
			else
			{
				//если искомый тайл не последний
				if (j != lastTileInLine)
				{
					addFittingTile(temp, ind, j, tileColCount, firstTileInLine, wangTiles, LBT); //LEFT-BOTTOM-TOP
				}
				else //если текущий тайл последний
				{
					addFittingTile(temp, ind, j, tileColCount, firstTileInLine, wangTiles, LRBT); //LEFT-RIGHT-BOTTOM-TOP
				}
			}

		}

#pragma endregion Для всех остальных

	}

#pragma endregion Выстраиваем тайлы в нужном порядке

	std::cout << "\n5. Сохраняем в файл.\n";
#pragma region save_file_wang_tiles

	//хранит значение последнего байта в результирующей картинке
	int lastByteInTexture = allTilesByte;

	//шаг = длине ширине изображения высотой в 1 пиксель
	int rowStep = (tileColCount * _tileSize) * 3;

	//шаг = длине ширине изображения высотой в 1 тайл
	int rowTileStep = rowStep * _tileSize;

	//шаг = ширине 1 тайла по RGB-маске
	int colStep = _tileSize * 3;

	//индекс байта
	int n = 0;

	//индекс текущего тайла
	int tileNum = 0;;

	//от 0 до последнего пикселя с шагом в ширину изображения высотой в тайл
	for (int j = 0; j < lastByteInTexture; j += rowTileStep)
	{
		//цикл для построчной записи тайла в массив
		for (int i = j; i < j + rowStep; i += colStep)
		{
			n = i;
			if (tileNum < tileColCount) //для первой строчки
			{
				//считывание тайла, построчно и запись в массив
				for (int k = 0; k < _tileSize; k++)
				{
					unsigned char* ptr = wangTiles[tileNum].tile->data(0, k);
					memcpy(_test + n, ptr, colStep);
					n = n + rowStep;
				}
			}
			else if (tileNum % tileColCount == 0) //для первого элемента первой строчки
			{
				for (int k = 0; k < _tileSize; k++)
				{
					unsigned char* ptr = wangTiles[tileNum].tile->data(0, k);
					memcpy(_test + n, ptr, colStep);
					n = n + rowStep;
				}
			}
			else //для всех остальных
			{
				for (int k = 0; k < _tileSize; k++)
				{
					unsigned char* ptr = wangTiles[tileNum].tile->data(0, k);
					memcpy(_test + n, ptr, colStep);
					n = n + rowStep;
				}
			}
			tileNum++;
		}
	}

	//временная ширина и высота изображения, т.к. будет обрезка
	int tempWidth = tileColCount * _tileSize;
	int tempHeight = tileRowCount * _tileSize;

	//сохраняем в файл
	osg::Image* pImage = new osg::Image;
	pImage->setImage(tempWidth, tempHeight, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, _test, osg::Image::USE_NEW_DELETE, 1);
	pImage = getSubImage(pImage, 0, 0, _outputWidth, _outputHeight);
	osgDB::writeImageFile(*pImage, "D:\\Orion\\img\\_test.bmp");	//сохранили в файл


#pragma endregion Сохраняем в файл конечную текстуру

	std::ofstream f("D:\\out.txt");
	for (int i = 0; i < wangTiles.size(); i++)
	{
		f << wangTiles[i].tile->getFileName() << '\n';
	}
	f.close();

}

osg::ref_ptr<osg::Image> Wang::getRotatedWang(osg::ref_ptr<osg::Image> inputImage)
{
	int textureWidth = _wangSize * 2; //диагональ тайла = стороне после переворота
	int textureHeight = textureWidth; //т.к. квадрат

	const int t = textureWidth * sqrt(2) / 2;

	//сторона тайла = степени 2
	osg::Image* temp = new osg::Image;
	temp->copySubImage(textureWidth - inputImage->s(), textureHeight - inputImage->s(), 0, inputImage);
	//osgDB::writeImageFile(*temp, "D:\\Orion\\img\\" + std::to_string(kkk) + ".bmp");	//сохранили вфайл

	//textureWidth = textureWidth * sqrt(2);
	textureHeight = textureWidth = _wangSize;

	_newTextureSize = textureWidth;

	osgViewer::Viewer viewer;
	osg::Group* sceneGraph = new osg::Group;
	{
		//Setup rotated geometry 
		osg::ref_ptr<osg::Geode> geode = new osg::Geode();

		osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
		geode->addDrawable(geometry);

		osg::Vec3Array& n = *(new osg::Vec3Array(1));
		n[0] = osg::Vec3(0, 0, 1);
		geometry->setNormalArray(&n, osg::Array::BIND_OVERALL);

		int diagsize = min(textureWidth / 2, textureHeight / 2);
		osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array();
		vertexArray->push_back(osg::Vec3(_wangSize, 0, -1));
		vertexArray->push_back(osg::Vec3(_wangSize - t, t, -1));
		vertexArray->push_back(osg::Vec3(_wangSize - t * 2, 0, -1));
		vertexArray->push_back(osg::Vec3(_wangSize - t, -t, -1));
		geometry->setVertexArray(vertexArray);

		osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array();
		colorArray->push_back(osg::Vec4(1.f, 1.f, 1.f, 1.f));
		geometry->setColorArray(colorArray);
		geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

		osg::ref_ptr<osg::Vec2Array> texCoordArray = new osg::Vec2Array();
		texCoordArray->push_back(osg::Vec2(1.f, 1.f));
		texCoordArray->push_back(osg::Vec2(0.f, 1.f));
		texCoordArray->push_back(osg::Vec2(0.f, 0.f));
		texCoordArray->push_back(osg::Vec2(1.f, 0.f));

		geometry->setTexCoordArray(0, texCoordArray);

		geometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_FAN, 0, 4));

		if (temp != NULL)
		{
			osg::ref_ptr<osg::Texture2D> texture2D = new osg::Texture2D();
			texture2D->setImage(temp);
			geode->getOrCreateStateSet()->setTextureAttributeAndModes(
				0, texture2D, osg::StateAttribute::ON);
		}

		geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		sceneGraph->addChild(geode);

	}

	//Setup camera
	osg::Vec4 clearColor(0, 0, 0, 1);
	osg::Camera* camera = viewer.getCamera();
	camera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
	camera->setProjectionMatrixAsOrtho2D(-textureHeight / 2, textureHeight / 2, -textureWidth / 2, textureWidth / 2);
	camera->setViewMatrixAsLookAt(osg::Vec3(0.0f, 0.0f, 1.0f), osg::Vec3(0, 0, 0), osg::Vec3(0.0f, 1.0f, 0.0f));
	camera->setViewport(0, 0, textureWidth, textureHeight);
	camera->setClearColor(clearColor);
	camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Create the texture image
	osg::ref_ptr<osg::Image> outputImage = new osg::Image();
	outputImage->allocateImage(textureWidth,
		textureHeight,
		1,   // 2D texture is 1 pixel deep
		GL_RGB,
		GL_UNSIGNED_BYTE);
	outputImage->setInternalTextureFormat(GL_RGB8);
	camera->attach(osg::Camera::COLOR_BUFFER, outputImage.get(), 0, 0);

	{
		//Setup graphics context for offsreen rendering
		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
		traits->x = 0;
		traits->y = 0;
		traits->width = textureWidth;
		traits->height = textureHeight;
		traits->red = 8;
		traits->green = 8;
		traits->blue = 8;
		traits->alpha = 8;
		traits->depth = 24;
		traits->windowDecoration = false;
		traits->pbuffer = true;
		traits->doubleBuffer = true;
		traits->sharedContext = 0;

		osg::ref_ptr<osg::GraphicsContext> pbuffer = osg::GraphicsContext::createGraphicsContext(traits.get());
		if (pbuffer.valid())
		{
			osg::ref_ptr<osg::Camera> camera = viewer.getCamera();
			camera->setGraphicsContext(pbuffer.get());
		}
		else
		{
			std::cout << "Failed initializing pixel buffer" << std::endl;
		}
	}

	//Setup viewer
	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
	viewer.setSceneData(sceneGraph);
	viewer.realize();

	//Make one frame	
	viewer.frame();
	//osgDB::writeImageFile(*outputImage, "D:\\Orion\\img\\" + std::to_string(kkk++) + "_rot_.bmp");	//сохранили вфайл

	return outputImage;
}
void Wang::addFittingTile(_wangTile temp, int ind, int curr, int tileColCount, int firstTileInLine, std::vector<_wangTile> & wangTiles, _tileCombine c)
{
	std::random_device rd;
	std::mt19937 gen(rd());  // to seed mersenne twister.
	std::uniform_int_distribution<> dist(0, _wangTiles.size() - 1); // distribute results between 1 and 6 inclusive.

	bool clause = false;
	while (!clause)
	{
		switch (c)
		{
		case L:
#pragma region L
			clause = _direction[ind][Left] == wangTiles[curr - 1].direction[Right]; //искомый тайл (лево) = сосед слева (право)
			break;
#pragma endregion	для промежуточного тайла первой строки

		case LR:
#pragma region LR
			clause = _direction[ind][Left] == wangTiles[curr - 1].direction[Right] //искомый тайл (лево) = сосед слева (право)
				&& _direction[ind][Right] == wangTiles[0].direction[Left]; //искомый тайл (право) = первый тайл строки (лево)
			break;
#pragma endregion	последнего тайла первой строки

		case B:
#pragma region B
			clause = _direction[ind][Bottom] == wangTiles[curr - tileColCount].direction[Top]; //искомый тайл (низ) = сосед снизу (вверх)
			break;
#pragma endregion	первого тайла новой строки

		case LB:
#pragma region LB
			clause = _direction[ind][Left] == wangTiles[curr - 1].direction[Right] //искомый тайл (лево) = сосед слева (право)
				&& _direction[ind][Bottom] == wangTiles[curr - tileColCount].direction[Top]; //искомый тайл (низ) = сосед снизу (вверх)
			break;
#pragma endregion	промежуточного тайла каждой новой строки

		case LRB:
#pragma region LRB
			clause = _direction[ind][Left] == wangTiles[curr - 1].direction[Right] //искомый тайл (лево) = сосед слева (право)
				&& _direction[ind][Right] == wangTiles[firstTileInLine].direction[Left] //искомый тайл (право) = первый тайл строки (лево)
				&& _direction[ind][Bottom] == wangTiles[curr - tileColCount].direction[Top]; //искомый тайл (низ) = сосед снизу (вверх)
			break;
#pragma endregion	последнего тайла каждой новой строки

		case BT:
#pragma region BT
			clause = _direction[ind][Bottom] == wangTiles[curr - tileColCount].direction[Top] //искомый тайл (низ) = сосед снизу (верх)
				&& _direction[ind][Top] == wangTiles[curr - firstTileInLine].direction[Bottom]; //искомый тайл (верх) = самый первый тайл (низ)
			break;
#pragma endregion	первого тайла последней строки

		case LBT:
#pragma region LBT
			clause = _direction[ind][Left] == wangTiles[curr - 1].direction[Right] //искомый тайл (лево) = сосед слева (право)
				&& _direction[ind][Bottom] == wangTiles[curr - tileColCount].direction[Top] //искомый тайл (низ) = сосед снизу (вверх)
				&& _direction[ind][Top] == wangTiles[curr - firstTileInLine].direction[Bottom];
			break;
#pragma endregion	промежуточного тайла последней строки

		case LRBT:
#pragma region LRBT
			clause = _direction[ind][Left] == wangTiles[curr - 1].direction[Right] //искомый тайл (лево) = сосед слева (право)
				&& _direction[ind][Right] == wangTiles[firstTileInLine].direction[Left] //искомый тайл (право) = первый тайл строки (лево)
				&& _direction[ind][Bottom] == wangTiles[curr - tileColCount].direction[Top] //искомый тайл (низ) = сосед снизу (вверх)
				&& _direction[ind][Top] == wangTiles[curr - firstTileInLine].direction[Bottom];
			break;
#pragma endregion	последнего тайла последней строки
		}

		if (clause)
		{
			temp.direction = _direction[ind];
			temp.tile = _wangTiles[ind];
			wangTiles.push_back(temp);

			//std::ofstream f("D:\\rnd.txt", std::ios::app);
			//f << ind << std::endl;
			//f.close();
		}
		else
		{
			ind = dist(gen);
		}
	}
}