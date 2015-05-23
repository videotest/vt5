	template<class T>
	class matrix:public std::valarray<T>{
	public:
		matrix():_nCols(0){};
		matrix(size_t mRows, size_t _nCols):std::valarray<T>(mRows*_nCols),_nCols(_nCols){}
		matrix(T* arr,size_t mRows,size_t _nCols):valarray<T>(arr,mRows*_nCols),_nCols(_nCols){}
		T* operator[](size_t row) {
			_ASSERTE(row < size()/_nCols);
			return &(valarray<T>::operator[](_nCols*row));
		}
		const T* operator[](size_t row) const {
			_ASSERTE(row < size()/_nCols);
			return &(valarray<T>::operator[](_nCols*row));
		}
		~matrix(){};
		matrix& operator=(const T& val) {valarray<T>::operator=(val);return *this;}
		void resize(size_t m_Rows, size_t n_Cols) {
			_nCols=n_Cols;
			valarray<T>::resize(m_Rows*n_Cols);
		}
		//slice_array<T>& row(size_t aRow) {return this[slice(aRow*_nCols, _nCols, 1)]};
		//slice_array<T>& col(size_t aCol) {return this[slice(aCol, size()/_nCols, _nCols)]};
		int GetColsCount()const{return _nCols;}
		int GetRowsCount()const{return _nCols ? size()/_nCols : 0;}
		T& operator[](POINT pt) {
			_ASSERTE(pt.x < (long)size()/(long)_nCols);
			return std::valarray<T>::operator[](_nCols*pt.x+pt.y);
		}
		const T& operator[](POINT pt) const {
			_ASSERTE(pt.x < (int)size()/_nCols);
			return valarray<T>::operator[](_nCols*pt.x+pt.y);
		}
	private:
		matrix(const matrix&):_nCols(0){};
		size_t _nCols;
	};
