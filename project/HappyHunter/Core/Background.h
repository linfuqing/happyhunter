#pragma once

namespace zerO
{
	class CBackground
	{
	public:
		CBackground(void);
		~CBackground(void);

		bool GetFogEnable()const;
		void SetFogEnable(bool bValue);

		const D3DXMATRIX GetMatrix()const;

		virtual void Update();

		virtual void Render() = 0;

	private:
		D3DXMATRIX m_Matrix;

		bool m_bFogEnable;
	};

	inline const D3DXMATRIX CBackground::GetMatrix()const
	{
		return m_Matrix;
	}

	inline bool CBackground::GetFogEnable()const
	{
		return m_bFogEnable;
	}

	inline void CBackground::SetFogEnable(bool bValue)
	{
		m_bFogEnable = bValue;
	}
}