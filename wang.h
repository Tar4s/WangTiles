#include <Windows.h>
#include <osg/Image>
#include <osgDB/WriteFile>

//���������� ��� ��������
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Texture2D>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgViewer/Viewer>
#include <osg/Math>
//���������� ��� ��������

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <map>
#include <random> //��� ������������ �������������
#include "weave.h"

class Wang : public Weave
{
	//������������ ������ �����
	enum _tileSide
	{
		Left = 0,
		Bottom = 1,
		Top = 2,
		Right = 3
	};
	
	//������������ ���������� ������
	enum _wangColors
	{
		Green = 1,
		Blue = 2,
		Red = 3,
		Yellow = 4,
		lghtRed = 5
	};
	
	//������������ ������� ����� � ��������
	enum _tileCombine
	{
		//� ������������ - ����� �� ������� ���� ��������� ������� ���� (0-�� �� ���������, ������������� 1-�� ���� �������)

		L = 1,		/* 'LEFT'					- ��� �������������� ����� ������ ������*/
		LR = 2,		/* 'LEFT-RIGHT'				- ������ ������ ���������� �����*/
		B = 3,		/* 'BOTTOM'					- ������� ����� ����� ������*/
		LB = 4,		/* 'LEFT-BOTTOM'			- �������������� ����� ������ ����� ������*/
		LRB = 5,	/* 'LEFT-RIGHT-BOTTOM'		- ���������� ����� ������ ����� ������*/
		BT = 6,		/* 'BOTTOM-TOP'				- ������� ����� ��������� ������*/
		LBT = 7,	/* 'LEFT-BOTTOM-TOP'		- �������������� ����� ��������� ������*/
		LRBT = 8	/* 'LEFT-RIGHT-BOTTOM-TOP'	- ���������� ����� ��������� ������*/
	};
	
	//��������� ����� � ��� ������
	struct _wangTile
	{
		int *direction;
		osg::ref_ptr<osg::Image> tile;
	};

	static const int _wangCmpntCount = 4;					//���������� ������ �� ������� ������� 1 ����
	static const int _wangPatchCount = 4;					//���������� ������
	static const int _wangTilesCount = 16;					//���������� ���-������

	int _outputWidth;
	int _outputHeight;

	int _wangSize;
	int _patchSize;
	int _patchStep;
	int _overlapPixels;
	int _newTextureSize;

	//������� ���������� �����
	int _tileSize;

	//��������� ��� 5-� ������
	std::vector<osg::ref_ptr<osg::Image>> _wangPatches;
	
	//��������� ��� ���������� ���-������
	std::vector<osg::ref_ptr<osg::Image>> _wangTiles;	
	
	//������ ���������� 4-��� ������ ��� ����������� ������������� ������
	int _direction[_wangTilesCount][_wangCmpntCount];

	unsigned char *_test;

public:
	//�����������
	void init(osg::Image* image, int outputWidth, int outputHeight, int wangSize, int overlapPixels);

	//��������� ���������
	void run();

private:
	//������� 4 ����� ��� ���������� ������
	void createPatches();

	//���������� ���-����� � �������� �� � ���������
	void createWangTiles();

	//��������� 4-�� �����������; ������������ ���
	void wangRotate(std::vector<std::vector<osg::ref_ptr<osg::Image>>> & preWangTiles);

	//������� ��������� �����������
	osg::ref_ptr<osg::Image> getRotatedWang(osg::ref_ptr<osg::Image> inputImage);

	//����� ������ ����������� �����������
	void cutRotatedWangSector();

	//����������� ����� � ��������� ��������
	void generateWangTexture();

	//������ ������������ �����
	void addFittingTile(_wangTile temp, //��������� ��������� ���������� �����
							int ind,	//������ ���������� �����
							int curr, //������ �������� ����� � ������� 
							int tileColCount,	//���������� ������ � ������
							int firstTileInLine,	//������ ������� ����� ������� ������
							std::vector<_wangTile> & wangTiles,	//��������� ������ � �������
							_tileCombine c); //������ ������������ ���������� ��������� ������� ������ � ��������












	void createPatches_test();
};

