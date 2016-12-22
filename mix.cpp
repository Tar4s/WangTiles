#include "mix.h"

//�������������
void Mix::init(int patchSize, int overlapPixels, int inputWidth, int inputHeight) //�������������� ����������� ����������
{
	_patchSize = patchSize + overlapPixels; //��� ������������ ������� ����, ���� ������ � ���������������� ������ ��������� �����.
	_overlapPixels = overlapPixels;
	_inputWidth = inputWidth;
	_inputHeight = inputHeight;

	_patchColCount = ceil((double)inputWidth / (double)patchSize);; //��������� ������ �� ��������
	_patchRowCount = ceil((double)inputHeight / (double)patchSize); //��������� ������ �� �������

	_outputWidth = (_patchColCount * patchSize) + _overlapPixels;
	_outputHeight = (_patchRowCount * patchSize) + _overlapPixels;
}

//������� �������
void Mix::boundaryCut(std::vector<osg::ref_ptr<osg::Image>> & formingImages)
{
	_minimalErrorRim.clear();
	_minimalErrorRim.resize(_patchSize, 0);
	std::vector<osg::ref_ptr<osg::Image>> temp;

	int n = 0, imageNumber = 0;
	_test = new unsigned char[_outputWidth * _outputHeight * 3];

	for (int j = 0; j < (_outputWidth * (_outputHeight - _overlapPixels)) * 3; j += (_outputWidth * (_patchSize - _overlapPixels)) * 3)
	{
		for (int i = j; i < j + (_outputWidth - _overlapPixels) * 3; i += (_patchSize - _overlapPixels) * 3)
		{
			n = i;

			//==================================================================================================================================
			//==================================================================================================================================
			//==================================================================================================================================

			if (imageNumber < _patchColCount) //��� ������ ������� (���� ������ �� ������� �����)
			{
				for (int k = 0; k < _patchSize; k++)
				{
					unsigned char* ptr = formingImages[imageNumber]->data(_minimalErrorRim[k], k);
					memcpy(_test + (n + _minimalErrorRim[k] * 3), ptr, (_patchSize * 3) - (_minimalErrorRim[k] * 3));
					n = n + _outputWidth * 3;
				}
				overlapMinimalErrorBoundary(formingImages[imageNumber], formingImages[imageNumber + 1], RIGHT);
			}

			//===============================================================================================================================
			//===============================================================================================================================
			//===============================================================================================================================

			else if (imageNumber % _patchColCount == 0) //��� 1 �������� ������ ����� ������ (���� �� ������� �����)
			{
				int m = n;
				overlapMinimalErrorBoundary(formingImages[imageNumber], formingImages[imageNumber - _patchColCount], DOWN);

				for (int k = 0; k < _patchSize; k++)
				{
					if (k < _overlapPixels)
					{
						for (int col = 0; col < _patchSize; col++)
						{
							if (_minimalErrorRim[col] <= k)
							{
								unsigned char* ptr = formingImages[imageNumber]->data(col, k);
								memcpy(_test + m, ptr, 3);
							}
							m = m + 3;
						}
						m = m + (_outputWidth * 3) - (_patchSize * 3);
					}
					else
					{
						unsigned char* ptr = formingImages[imageNumber]->data(0, k);
						memcpy(_test + m, ptr, _patchSize * 3);
						m = m + _outputWidth * 3;
					}
				}
			}

			//=================================================================================================================================
			//=================================================================================================================================
			//=================================================================================================================================

			else //��� ��������� ��������� (���� �� ������� � ������� �����)
			{

				int m = n;
				overlapMinimalErrorBoundary(formingImages[imageNumber], formingImages[imageNumber - _patchColCount], DOWN);

				for (int k = 0; k < _patchSize; k++)
				{
					if (k < _overlapPixels)
					{
						for (int col = 0; col < _patchSize; col++)
						{
							if (_minimalErrorRim[col] <= k)
							{
								unsigned char* ptr = formingImages[imageNumber]->data(col, k);
								memcpy(_test + m, ptr, 3);
							}
							m = m + 3;
						}
						m = m + (_outputWidth * 3) - (_patchSize * 3);
					}
					else
					{
						overlapMinimalErrorBoundary(formingImages[imageNumber - 1], formingImages[imageNumber], RIGHT);

						unsigned char* ptr = formingImages[imageNumber]->data(_minimalErrorRim[k], k);
						memcpy(_test + (m + _minimalErrorRim[k] * 3), ptr, (_patchSize * 3) - (_minimalErrorRim[k] * 3));
						m = m + _outputWidth * 3;
					}
				}
			}

			//=========================================================================================================================================
			//=========================================================================================================================================
			//=========================================================================================================================================

			imageNumber++;
		}
	}

	osg::Image* pImage = new osg::Image;
	//��������� �����������
	pImage->setImage(_outputWidth, _outputHeight , 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, _test, osg::Image::USE_NEW_DELETE, 1);

	//crop ����������� �� _outputWidth � _outputHeight
	pImage = getSubImage(pImage, 0, 0, _inputWidth, _inputHeight);
	
	//������� ���
	pImage->setFileName("Tile #" + std::to_string(tileCount));

	//osgDB::writeImageFile(*pImage, "D:\\Orion\\img\\" + pImage->getFileName() + ".bmp");	//��������� � ����
	
	temp.push_back(pImage);
	formingImages.swap(temp);

	std::cout << "���� #" << tileCount++ << " ���������;" << std::endl;
}
void Mix::saveFile(std::string resultFileDest)

{
	osg::Image* pImage = new osg::Image;
	pImage->setImage(_outputWidth, _outputHeight, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, _test, osg::Image::USE_NEW_DELETE, 1);
	osgDB::writeImageFile(*pImage, resultFileDest);	//��������� � ����
}

//��������������� �������
void Mix::overlapMinimalErrorBoundary(osg::Image* firstPatch, osg::Image* secondPatch, overlap_direction c)
{
	switch (c)
	{
	case RIGHT:
	{
				  osg::Image* leftPatchOverlap = Weave :: getSubImage(secondPatch, 0, 0, _overlapPixels, _patchSize); //������ �������
				  osg::Image* rightPatchOverlap = Weave::getSubImage(firstPatch, _patchSize - _overlapPixels, 0, _patchSize, _patchSize); //����� �������

				  getMinimalErrorBoundary(leftPatchOverlap, rightPatchOverlap, true);

	}
		break;

	case DOWN:
	{
				 osg::Image* upPatchOverlap = Weave :: getSubImage(firstPatch, 0, 0, _patchSize, _overlapPixels); //������ �������
				 osg::Image* downPatchOverlap = Weave :: getSubImage(secondPatch, 0, _patchSize - _overlapPixels, _patchSize, _patchSize); //������� �������

				 getMinimalErrorBoundary(upPatchOverlap, downPatchOverlap, false);
	}
		break;

	}
}
void Mix::getMinimalErrorBoundary(osg::Image* firstOverlap, osg::Image* secondOverlap, bool flagDirection)
{
	int root, body, rowMinInd, rowMin = INT_MAX, globalMin = INT_MAX;

	std::vector<int> minIndArray;

	if (flagDirection) //���� 1
	{
		root = firstOverlap->s(); //������ ��� ������ ����������
		body = firstOverlap->t(); //���� - ������
	}
	else
	{
		root = firstOverlap->t(); //������ ��� ������ ����������
		body = firstOverlap->s(); //���� - ������
	}


	int errArray = 0; //��������� ������ �� �������

	unsigned char* ptrFirst;
	unsigned char* ptrSecond;

	for (int i = 0; i < root; i++) //������� �������� � ���������
	{
		int node = i;

		if (flagDirection)
		{
			ptrFirst = firstOverlap->data(i, 0);
			ptrSecond = secondOverlap->data(i, 0);
		}
		else
		{
			ptrFirst = firstOverlap->data(0, i);
			ptrSecond = secondOverlap->data(0, i);
		}

		errArray = abs(ptrFirst[0] - ptrSecond[0]) + abs(ptrFirst[1] - ptrSecond[1]) + abs(ptrFirst[2] - ptrSecond[2]);
		minIndArray.push_back(i);

		for (int k = 1; k < body; k++) //������� �������� � ����
		{
			int buf = 0;
			for (int m = max(0, node - 1); m <= min(root - 1, node + 1); m++) //������� ���� ��������
			{

				if (flagDirection)
				{
					ptrFirst = firstOverlap->data(m, k);
					ptrSecond = secondOverlap->data(m, k);
				}
				else
				{
					ptrFirst = firstOverlap->data(k, m);
					ptrSecond = secondOverlap->data(k, m);
				}

				buf = abs(ptrFirst[0] - ptrSecond[0]) + abs(ptrFirst[1] - ptrSecond[1]) + abs(ptrFirst[2] - ptrSecond[2]);

				if (buf < rowMin)
				{
					rowMin = buf;
					rowMinInd = m;
				}
			};

			errArray += rowMin;
			minIndArray.push_back(rowMinInd);

			node = rowMinInd;
			rowMin = INT_MAX;
		}

		if (errArray < globalMin)
		{
			globalMin = errArray;
			minIndArray.swap(_minimalErrorRim);
		}

		errArray = 0;
		minIndArray.clear();
	}
}