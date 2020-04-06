#pragma once

template <class T>
class CProxy_IWebBBSEvents : public IConnectionPointImpl<T, &__uuidof(_IWebBBSEvents), CComDynamicUnkArray>
{
	//경고 - 마법사에서 이 클래스를 다시 생성합니다.
public:
	VOID Fire_InstallCompleteEvent()
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

			if (pConnection)
			{
				DISPPARAMS params = { NULL, NULL, 0, 0 };
				hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, NULL, NULL, NULL);
			}
		}
	}
	//20090304 해시값 추가, jyh
	//VOID Fire_UpSelectComplete(BSTR  strPath, BSTR  strSize, BSTR  strMode)
	VOID Fire_UpSelectComplete(BSTR  strPath, BSTR  strSize, BSTR  strMode, BSTR strMD5)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

			if (pConnection)
			{
				//20090304 해시값 추가, jyh
				//CComVariant avarParams[3];
				CComVariant avarParams[4];

				avarParams[3] = strPath;	avarParams[3].vt = VT_BSTR;
				avarParams[2] = strSize;	avarParams[2].vt = VT_BSTR;
				avarParams[1] = strMode;	avarParams[1].vt = VT_BSTR;
				avarParams[0] = strMD5;		avarParams[0].vt = VT_BSTR;
				//DISPPARAMS params = { avarParams, NULL, 3, 0 };
				DISPPARAMS params = { avarParams, NULL, 4, 0 };
				hr = pConnection->Invoke(2, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, NULL, NULL, NULL);
			}
		}
	}
};
