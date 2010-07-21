#pragma once

namespace zerO
{
	class CBackground
	{
	public:
		CBackground(void);
		~CBackground(void);

		const D3DXMATRIX GetMatrix()const;

		virtual void Update();

		virtual void Render() = 0;

	private:
		D3DXMATRIX m_Matrix;
	};

	inline const D3DXMATRIX CBackground::GetMatrix()const
	{
		return m_Matrix;
	}
}