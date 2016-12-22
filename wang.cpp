#include "wang.h"
#include "mix.h"
#include "create.h"

//������ ������������� �������
#define DIRECTION_INIT( par1, c1, c2, c3, c4 ) par1[0] = c1; par1[1] = c2; par1[2] = c3; par1[3] = c4;

//������ ������ ����� ���������� � �� �������� �� �����
#define OUTPUT( var ) std::cout << #var << "=" << var << std::endl;

//�������������
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

	DIRECTION_INIT(_direction[0], Red, Blue, Blue, Red); //������, ������, �����, �������
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


//������� �������
void Wang::run()
{
	this->createPatches();
	this->createWangTiles();
}
void Wang::createPatches()
{
	srand(time(NULL));

	//�������� ������ �� �����������
	int imgWidth = _image->s();
	int imgHeight = _image->t();

	int startX, stopX;
	int startY, stopY;

	std::cout << "\n1. �������� �����.\n\n";
	//��������� 4 ����� �� osg::image
	for (int patchNum = 0; patchNum < _wangPatchCount; patchNum++)
	{
		//���������� ��������� �����
		startX = rand() % imgWidth;
		startY = rand() % imgHeight;

		//�������� �������� �����
		stopX = startX + (_patchSize + _overlapPixels); //_overlapPixels ����� ��� ������������ �������� �������� �����������
		stopY = startY + (_patchSize + _overlapPixels); //����� ��������� Crop � ������ ���������� ������������� �����������

		//���� �������� ����� �� ������ � �������� ��������
		while (stopX > imgWidth || stopY > imgHeight)
		{
			//���������� ����� �����, ����� ������� � ��������
			startX = rand() % imgWidth;
			startY = rand() % imgHeight;

			stopX = startX + (_patchSize + _overlapPixels); //_overlapPixels ����� ��� ������������ �������� �������� �����������
			stopY = startY + (_patchSize + _overlapPixels); //����� ��������� Crop � ������ ���������� ������������� �����������
		}

		//�������� n-�� ����
		osg::Image* patch = Weave::getSubImage(_image, startX, startY, stopX, stopY);

		//�������� ��� ��� (�����+1)
		patch->setFileName(std::to_string(patchNum + 1));

		//�������� ��� � ������
		_wangPatches.push_back(patch);

		//������ ��������� �� �����
		std::cout << "���� �" << patchNum << " ��������;\n";
	}
	std::cout << std::endl;
	system("pause");
}
void Wang::createWangTiles()
{
	std::vector<std::vector<osg::ref_ptr<osg::Image>>> preWangTiles;

	//��������� ��������� ������ ��� �������� 4-� ������
	std::vector<osg::ref_ptr<osg::Image>> temp;

	std::cout << "\n2. ��������� ����� �� ������ ������.\n\n";
	//���� ������������ 8 ���-������
	for (int i = 0; i < _wangTilesCount; i++)
	{
		//��������� ��������� ������ 4-�� ����������
		//� ������ ��� ������������������
		for (int k = 0; k < _wangCmpntCount; k++)
		{
			//��������� � temp ������ ��� ����
			temp.push_back(_wangPatches[_direction[i][k] - 1]);
		}

		//��������� � ������� ������, ������ � ����������
		//� ������ ������������������ (����)
		preWangTiles.push_back(temp);
		temp.clear();
		std::cout << "���� �" << i << " �������������;\n";
	}
	std::cout << std::endl;
	system("pause");

	wangRotate(preWangTiles);
}















void Wang::createPatches_test()
{
	srand(time(NULL));

	//�������� ������ �� �����������
	int imgWidth = _image->s();
	int imgHeight = _image->t();

	int patchCount = 0;
	int patchSize = _patchSize + _overlapPixels;

	int stopY = imgHeight - patchSize;
	int stopX = imgWidth - patchSize;

	for (int startY = 0; startY <= stopY; startY += _patchStep) //������� �� ���������, ���� ���� �� ������� �� ������
	{
		for (int startX = 0; startX <= stopX; startX += _patchStep) //������� �� �����������, ���� ���� �� ������� �� ������
		{
			osg::Image* patch = getSubImage(_image, startX, startY, startX + patchSize, startY + patchSize); //�������� �����
			patch->setFileName(std::to_string(patchCount));	//������� ���
			_wangPatches.push_back(patch);	//�������� � ������ ������ �����

			//osgDB::writeImageFile(*_images[patch_count], patch_name.append(line).append(std::to_string(patch_count)).append(".bmp"));	//��������� � ����

			patchCount++;
			//patch_name.clear();
		}
	}
	std::cout << "��������� " << patchCount + 1 << " ������.\n\n";
	system("pause");
}


















//��������������� �������
void Wang::wangRotate(std::vector<std::vector<osg::ref_ptr<osg::Image>>> & preWangTiles)
{
	std::vector<osg::ref_ptr<osg::Image>> wangRotatedTiles;
	std::cout << "\n3. ��������� �������� ���-�����.\n\n";

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

#pragma endregion ��������� ����� ��� ��������� ������


	for (int i = 0; i < _wangTilesCount; i++)
	{
		osg::Image* inputImage = preWangTiles[i][0];
		osg::ref_ptr<osg::Image> outputImage = getRotatedWang(inputImage);

		outputImage->setFileName(std::to_string(i));

		wangRotatedTiles.push_back(outputImage);
		std::cout << "���-���� �" << i << " �������� � ��������;\n";

		//osgDB::writeImageFile(*inputImage, "D:\\Orion\\img\\" + inputImage->getFileName() + ".bmp");	//��������� �����
		//osgDB::writeImageFile(*outputImage, "D:\\Orion\\img\\" + preWangTiles[i][0]->getFileName() + "_rtd.bmp");	//��������� �����
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

		osgDB::writeImageFile(*patch, "D:\\Orion\\img\\" + std::to_string(i) + "_cut_.bmp");	//��������� �����
		temp.push_back(patch);
	}

	_wangTiles.swap(temp);
	_tileSize = _wangTiles[0]->t(); //�������� ������ ������� ���������� �����
	temp.clear();

	generateWangTexture();
}
void Wang::generateWangTexture()
{
#pragma region init_variables

	srand(time(NULL));

	_tileSize = _wangTiles[0]->t();

	int firstTileInLine = 0; //����� ������� ����� � ������
	int lastTileInLine; //����� ���������� ����� � ������
	int curTileLine = 1; //����� ������� ������ ������

	//���������� ������ �� ���������/�����������
	int tileColCount = _outputWidth / _tileSize;
	if (_outputWidth % _tileSize != 0) tileColCount++;

	int tileRowCount = _outputHeight / _tileSize;
	if (_outputHeight % _tileSize != 0) tileRowCount++;

	//����� ������
	int allTilesCount = tileColCount * tileRowCount;

	//���������� ���� �� ��� �����
	int allTilesByte = allTilesCount * ((_tileSize * _tileSize) * 3);

	//������ ����������
	_test = new unsigned char[allTilesByte];

	//�������� �������� ����� � �������� ��� ������
	std::vector<_wangTile> wangTiles;
	_wangTile temp;

	//������������������ #10 (����=���), ���������� ��� ������, ����� ������ ������ ������������
	int ind = rand() % _wangTilesCount;
	temp.direction = _direction[ind];

	//��������� � ������ ��������������� ������������������ ����
	temp.tile = _wangTiles[ind];
	wangTiles.push_back(temp);

#pragma endregion ��������� ����������

	std::cout << "\n4. ��������� ��������.\n";
#pragma region forming_wang_tiles

	//���� �������� ���� ������, ������� � 1, �.�. 0-�� ��� �������� ����
	for (int j = 1; j < allTilesCount; j++)
	{
		//����� ������� ������
		if (j % tileColCount == 0) curTileLine++;

		//����� ���������� ����� � ������� ������
		lastTileInLine = firstTileInLine + (tileColCount - 1); //����� ������� ����� + (����� ���������� ������ � ������ - 1) �.�. � ����)

#pragma region first_line

		if (j < tileColCount) //��� ���������� ������ �������
		{
			//������ �������� �����
			ind = rand() % _wangTiles.size();

			//���� ������ �� ���������
			if (curTileLine != tileRowCount)
			{
				//���� ������� ���� �� ���������
				if (j != lastTileInLine)
				{
					addFittingTile(temp, ind, j, tileColCount, firstTileInLine, wangTiles, L); //LEFT
				}
				else //���� ������� ���� ���������
				{
					addFittingTile(temp, ind, j, tileColCount, firstTileInLine, wangTiles, LR); //LEFT-RIGHT
				}
			}
		}

#pragma endregion ��� ���������� ������ �������

#pragma region first_tile_fisrt_line

		else if (j % tileColCount == 0) //��� ������� �������� ����� �������
		{
			firstTileInLine = j;
			ind = rand() % _wangTiles.size();

			//���� ������ �� ���������
			if (curTileLine != tileRowCount)
			{
				addFittingTile(temp, ind, j, tileColCount, firstTileInLine, wangTiles, B); //BOTTOM
			}
			//���� ������ ���������
			else
			{
				addFittingTile(temp, ind, j, tileColCount, firstTileInLine, wangTiles, BT); //BOTTOM-TOP
			}
		}

#pragma endregion ��� ������� �������� ����� �������

#pragma region for_all_other

		else //��� ���� ���������
		{
			ind = rand() % _wangTiles.size();

			//���� ������ �� ���������
			if (curTileLine != tileRowCount)
			{
				//���� ������� ���� �� ���������
				if (j != lastTileInLine)
				{
					addFittingTile(temp, ind, j, tileColCount, firstTileInLine, wangTiles, LB); //LEFT-BOTTOM
				}
				else //���� ������� ���� ���������
				{
					addFittingTile(temp, ind, j, tileColCount, firstTileInLine, wangTiles, LRB); //LEFT-RIGHT-BOTTOM
				}
			}
			//���� ������ ���������
			else
			{
				//���� ������� ���� �� ���������
				if (j != lastTileInLine)
				{
					addFittingTile(temp, ind, j, tileColCount, firstTileInLine, wangTiles, LBT); //LEFT-BOTTOM-TOP
				}
				else //���� ������� ���� ���������
				{
					addFittingTile(temp, ind, j, tileColCount, firstTileInLine, wangTiles, LRBT); //LEFT-RIGHT-BOTTOM-TOP
				}
			}

		}

#pragma endregion ��� ���� ���������

	}

#pragma endregion ����������� ����� � ������ �������

	std::cout << "\n5. ��������� � ����.\n";
#pragma region save_file_wang_tiles

	//������ �������� ���������� ����� � �������������� ��������
	int lastByteInTexture = allTilesByte;

	//��� = ����� ������ ����������� ������� � 1 �������
	int rowStep = (tileColCount * _tileSize) * 3;

	//��� = ����� ������ ����������� ������� � 1 ����
	int rowTileStep = rowStep * _tileSize;

	//��� = ������ 1 ����� �� RGB-�����
	int colStep = _tileSize * 3;

	//������ �����
	int n = 0;

	//������ �������� �����
	int tileNum = 0;;

	//�� 0 �� ���������� ������� � ����� � ������ ����������� ������� � ����
	for (int j = 0; j < lastByteInTexture; j += rowTileStep)
	{
		//���� ��� ���������� ������ ����� � ������
		for (int i = j; i < j + rowStep; i += colStep)
		{
			n = i;
			if (tileNum < tileColCount) //��� ������ �������
			{
				//���������� �����, ��������� � ������ � ������
				for (int k = 0; k < _tileSize; k++)
				{
					unsigned char* ptr = wangTiles[tileNum].tile->data(0, k);
					memcpy(_test + n, ptr, colStep);
					n = n + rowStep;
				}
			}
			else if (tileNum % tileColCount == 0) //��� ������� �������� ������ �������
			{
				for (int k = 0; k < _tileSize; k++)
				{
					unsigned char* ptr = wangTiles[tileNum].tile->data(0, k);
					memcpy(_test + n, ptr, colStep);
					n = n + rowStep;
				}
			}
			else //��� ���� ���������
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

	//��������� ������ � ������ �����������, �.�. ����� �������
	int tempWidth = tileColCount * _tileSize;
	int tempHeight = tileRowCount * _tileSize;

	//��������� � ����
	osg::Image* pImage = new osg::Image;
	pImage->setImage(tempWidth, tempHeight, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, _test, osg::Image::USE_NEW_DELETE, 1);
	pImage = getSubImage(pImage, 0, 0, _outputWidth, _outputHeight);
	osgDB::writeImageFile(*pImage, "D:\\Orion\\img\\_test.bmp");	//��������� � ����


#pragma endregion ��������� � ���� �������� ��������

	std::ofstream f("D:\\out.txt");
	for (int i = 0; i < wangTiles.size(); i++)
	{
		f << wangTiles[i].tile->getFileName() << '\n';
	}
	f.close();

}

osg::ref_ptr<osg::Image> Wang::getRotatedWang(osg::ref_ptr<osg::Image> inputImage)
{
	int textureWidth = _wangSize * 2; //��������� ����� = ������� ����� ����������
	int textureHeight = textureWidth; //�.�. �������

	const int t = textureWidth * sqrt(2) / 2;

	//������� ����� = ������� 2
	osg::Image* temp = new osg::Image;
	temp->copySubImage(textureWidth - inputImage->s(), textureHeight - inputImage->s(), 0, inputImage);
	//osgDB::writeImageFile(*temp, "D:\\Orion\\img\\" + std::to_string(kkk) + ".bmp");	//��������� �����

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
	//osgDB::writeImageFile(*outputImage, "D:\\Orion\\img\\" + std::to_string(kkk++) + "_rot_.bmp");	//��������� �����

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
			clause = _direction[ind][Left] == wangTiles[curr - 1].direction[Right]; //������� ���� (����) = ����� ����� (�����)
			break;
#pragma endregion	��� �������������� ����� ������ ������

		case LR:
#pragma region LR
			clause = _direction[ind][Left] == wangTiles[curr - 1].direction[Right] //������� ���� (����) = ����� ����� (�����)
				&& _direction[ind][Right] == wangTiles[0].direction[Left]; //������� ���� (�����) = ������ ���� ������ (����)
			break;
#pragma endregion	���������� ����� ������ ������

		case B:
#pragma region B
			clause = _direction[ind][Bottom] == wangTiles[curr - tileColCount].direction[Top]; //������� ���� (���) = ����� ����� (�����)
			break;
#pragma endregion	������� ����� ����� ������

		case LB:
#pragma region LB
			clause = _direction[ind][Left] == wangTiles[curr - 1].direction[Right] //������� ���� (����) = ����� ����� (�����)
				&& _direction[ind][Bottom] == wangTiles[curr - tileColCount].direction[Top]; //������� ���� (���) = ����� ����� (�����)
			break;
#pragma endregion	�������������� ����� ������ ����� ������

		case LRB:
#pragma region LRB
			clause = _direction[ind][Left] == wangTiles[curr - 1].direction[Right] //������� ���� (����) = ����� ����� (�����)
				&& _direction[ind][Right] == wangTiles[firstTileInLine].direction[Left] //������� ���� (�����) = ������ ���� ������ (����)
				&& _direction[ind][Bottom] == wangTiles[curr - tileColCount].direction[Top]; //������� ���� (���) = ����� ����� (�����)
			break;
#pragma endregion	���������� ����� ������ ����� ������

		case BT:
#pragma region BT
			clause = _direction[ind][Bottom] == wangTiles[curr - tileColCount].direction[Top] //������� ���� (���) = ����� ����� (����)
				&& _direction[ind][Top] == wangTiles[curr - firstTileInLine].direction[Bottom]; //������� ���� (����) = ����� ������ ���� (���)
			break;
#pragma endregion	������� ����� ��������� ������

		case LBT:
#pragma region LBT
			clause = _direction[ind][Left] == wangTiles[curr - 1].direction[Right] //������� ���� (����) = ����� ����� (�����)
				&& _direction[ind][Bottom] == wangTiles[curr - tileColCount].direction[Top] //������� ���� (���) = ����� ����� (�����)
				&& _direction[ind][Top] == wangTiles[curr - firstTileInLine].direction[Bottom];
			break;
#pragma endregion	�������������� ����� ��������� ������

		case LRBT:
#pragma region LRBT
			clause = _direction[ind][Left] == wangTiles[curr - 1].direction[Right] //������� ���� (����) = ����� ����� (�����)
				&& _direction[ind][Right] == wangTiles[firstTileInLine].direction[Left] //������� ���� (�����) = ������ ���� ������ (����)
				&& _direction[ind][Bottom] == wangTiles[curr - tileColCount].direction[Top] //������� ���� (���) = ����� ����� (�����)
				&& _direction[ind][Top] == wangTiles[curr - firstTileInLine].direction[Bottom];
			break;
#pragma endregion	���������� ����� ��������� ������
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