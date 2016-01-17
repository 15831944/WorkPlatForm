#include "stdafx.h"
#include <GdiplusImaging.h>
#include <xstring>
using namespace std;
using namespace  Gdiplus;
int GetEncoderClsid( const WCHAR* format, CLSID* pClsid )
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure

}
Bitmap * StretchBitmap(Bitmap * pSrcBmp, int nWidth,int nHeight)
{
	if (nWidth<=0 || nHeight <=0)
		return NULL;

	 Bitmap *bmpStretched = new Bitmap(nWidth,nHeight);
	 if (NULL == bmpStretched)
		 return NULL;

	 Graphics * pGraphics = NULL;
	 pGraphics = Graphics::FromImage(bmpStretched);
	 if (pGraphics!=NULL)
	 {
		 //将pSrcBmp画到画布pGraphics上
		 pGraphics->DrawImage(pSrcBmp,Rect(0,0,nWidth,nHeight));
		 return bmpStretched;
	 }
	 else
	 {
		 delete bmpStretched;
		 return NULL;
	 }
}
/************************************************************************/
/* 当输出宽高指定为-1时，以原始宽高保存，否则以指定的宽高拉伸保存                                                                     */
/************************************************************************/
BOOL SavePngFile(HICON hIcon, LPCSTR strPngFile,int iOutWith = -1,int iOutHeight = -1)
{
	if (hIcon == NULL)
		return FALSE;

	ICONINFO icInfo = { 0 };
	if (!::GetIconInfo(hIcon, &icInfo))
		return FALSE;


	BITMAP bitmap; 
	GetObject(icInfo.hbmColor, sizeof(BITMAP), &bitmap);

	Bitmap* pBitmap = NULL;
	Bitmap* pWrapBitmap = NULL;
	if (bitmap.bmBitsPixel != 32) 
	{   
		pBitmap = Bitmap::FromHICON(hIcon); 
	} 
	else
	{
		pWrapBitmap = Bitmap::FromHBITMAP(icInfo.hbmColor, NULL);
		BitmapData bitmapData;
		Rect rcImage(0,0, pWrapBitmap->GetWidth(), pWrapBitmap->GetHeight());
		pWrapBitmap->LockBits(&rcImage, ImageLockModeRead, pWrapBitmap->GetPixelFormat(), &bitmapData); 

		pBitmap = new Bitmap(bitmapData.Width, bitmapData.Height, bitmapData.Stride, 
			PixelFormat32bppARGB, (BYTE*)bitmapData.Scan0);

		pWrapBitmap->UnlockBits(&bitmapData);

		Gdiplus::Color Bmpcolor;
		BOOL BmpcolorTag=TRUE;
		for (UINT bmpx=0;bmpx<pBitmap->GetWidth();bmpx++)
		{
			for (UINT bmpy=0;bmpy<pBitmap->GetHeight();bmpy++)
			{
				pBitmap->GetPixel(bmpx,bmpy,&Bmpcolor);
				if(Bmpcolor.GetA()!=0)
				{
					BmpcolorTag=FALSE;
					break;	
				}
			}
			if (BmpcolorTag==FALSE)
				break;
		}
		if (BmpcolorTag==TRUE)
		{
			delete pWrapBitmap;
			pWrapBitmap = NULL;
			delete pBitmap;
			pBitmap = NULL;
			pBitmap = Gdiplus::Bitmap::FromHICON(hIcon); 
		} 
	}

	DeleteObject(icInfo.hbmColor); 
	DeleteObject(icInfo.hbmMask);

	static bool GetPngCode = false;
	static CLSID pngid;
	if (!GetPngCode)
	{
		GetPngCode = true;
		GetEncoderClsid(L"image/png", &pngid);
	}

	if (iOutWith !=-1 && iOutHeight!=-1)
	{
		Bitmap *pStretched =  StretchBitmap(pBitmap,128,128);
		if (pStretched)
		{
			delete pBitmap; 
			pBitmap = pStretched;
		}
	}

	_bstr_t bstr(strPngFile);
	wstring wstrPng = (WCHAR*)bstr;
	Gdiplus::Status s = pBitmap->Save(wstrPng.c_str(),&pngid,NULL);
	delete pBitmap; 
	if (pWrapBitmap)
		delete pWrapBitmap;

	DeleteObject(icInfo.hbmColor); 
	DeleteObject(icInfo.hbmMask); 

	if (s==Gdiplus::Ok)
		return TRUE;

	return FALSE;
}